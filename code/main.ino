#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <ESP_Mail_Client.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "secrets.h"  

#define RST_PIN 22
#define SS_PIN 21
#define RED_LED 4
#define GREEN_LED 5

MFRC522 mfrc522(SS_PIN, RST_PIN);


const String sheet_url = "https://script.google.com/macros/s/YOUR_ID/exec?name=";


const String AUTH_UID = "YOUR_CARD_UID";  

SMTPSession smtp;

void setup() {
  Serial.begin(9600);
  Serial.println("Booting...");

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected. IP: " + WiFi.localIP().toString());

  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("RFID reader ready.");
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) return;

  String uid = getCardUID();
  Serial.println("🆔 Card UID: " + uid);

  if (uid == AUTH_UID) {
    Serial.println("✅ Access Granted");
    blinkLED(GREEN_LED, 1);
    logToSheet("AUTHORIZED", "AUTHORIZED");
  } else {
    Serial.println("❌ Unauthorized Access Detected");
    blinkLED(RED_LED, 2);
    logToSheet(uid, "UNAUTHORIZED");

    // Send email in background
    String* uidCopy = new String(uid);
    xTaskCreatePinnedToCore(
      sendEmailTask,
      "SendEmailTask",
      10000,
      uidCopy,
      1,
      NULL,
      1
    );
  }

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}


String getCardUID() {
  String uidStr = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) uidStr += "0";
    uidStr += String(mfrc522.uid.uidByte[i], HEX);
  }
  uidStr.toUpperCase();
  return uidStr;
}

void logToSheet(String name, String status) {
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient https;
  String full_url = sheet_url + name + "&status=" + status;

  Serial.println("🌐 Logging to Google Sheets...");
  if (https.begin(client, full_url)) {
    int httpCode = https.GET();
    if (httpCode > 0) {
      Serial.printf("✅ Sheets log code: %d\n", httpCode);
    } else {
      Serial.printf("❌ Sheets log error: %s\n", https.errorToString(httpCode).c_str());
    }
    https.end();
  } else {
    Serial.println("❌ Unable to connect to Sheets");
  }
}


void blinkLED(int pin, int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(pin, HIGH);
    delay(300);
    digitalWrite(pin, LOW);
    delay(200);
  }
}


void sendEmailTask(void *parameter) {
  String uid = *((String *)parameter);
  delete (String *)parameter;

  ESP_Mail_Session session;
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "gmail.com";

  SMTP_Message message;
  message.sender.name = "ESP32 RFID Alert";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "Unauthorized RFID Access!";
  message.addRecipient("Security", RECEIVER_EMAIL);

  String body = "🚨 Unauthorized RFID Access Detected!\n\nCard UID: " + uid;
  message.text.content = body.c_str();

  smtp.debug(1);
  Serial.println("📧 Sending email in background...");
  smtp.connect(&session);
  if (!MailClient.sendMail(&smtp, &message)) {
    Serial.println("❌ Email failed");
  } else {
    Serial.println("📨 Email sent");
  }
  smtp.sendingResult.clear();

  vTaskDelete(NULL);  
  }s