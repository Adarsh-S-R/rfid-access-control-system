# ESP32 + RC522 RFID Access Control System

A smart access control system using **ESP32**, **RFID RC522**, **Google Sheets**, and **Wi-Fi** . Logs entries automatically to a Google Sheet, with **green/red LED feedback** for authorized/unauthorized users and alerts the admin via email for unauthorized access.

> **Demo Video**: [Watch on YouTube](https://youtu.be/Rjnas63PPxo)


##  Features

-  RFID-based access validation  
-  Real-time logging to Google Sheets via Apps Script  
-  Red & Green LED feedback  
-  Email alerts for unauthorized access  
-  Wi-Fi connectivity


## Hardware Connections

Here's the wiring diagram for connecting **ESP32 ↔ RC522** and LEDs:

![ESP32 RC522 Wiring Diagram](wiring/Connections.png)


 Use a 220Ω resistor with each LED to avoid burning them out.

## Setting Up Google Sheets App Script

1. **Open your Google Sheet** Create a new sheet to store RFID scan logs.
2. **Open Apps Script Editor** Go to `Extensions` → `Apps Script`.
3. **Paste the Script** Replace any existing code with the content from `code/sheeets_app_script.gs`.
4. **Deploy the Script**
   * Click `Deploy` → `New deployment`
   * Select type: **Web App**
   * Description: `RFID Logger`
   * Execute as: `Me`
   * Who has access: `Anyone`
   * Click **Deploy**, and allow permissions.
5. **Copy the Web App URL**
   * Paste this URL into your `secrets.h` file.

## ⚠️ Security Notes

* Keep your `secrets.h` file private — it contains credentials like Wi-Fi and email passwords.

## Acknowledgements

* ESP32
* MFRC522 Arduino Library
* Google Apps Script

## 📧 Contact

Feel free to fork, contribute, or get in touch if you'd like to collaborate!
