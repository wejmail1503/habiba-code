# 🐾 Smart IoT Pet Feeder & Water Dispenser

An automated, ESP32-CAM-based smart pet feeding and watering system with real-time Telegram monitoring, motion-triggered photo capture, and automated low-supply alerts.

**Created by:** Habiba & Raheeq

---

## 📋 Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Hardware Components](#hardware-components)
- [Pinout & Wiring](#pinout--wiring)
- [Dependencies & Libraries](#dependencies--libraries)
- [Installation & Setup](#installation--setup)
- [Configuration & Thresholds](#configuration--thresholds)
- [Security Notice](#security-notice)

---

## 📖 Overview
This project automates daily pet care by combining scheduled food dispensing, demand-based water refilling, and remote monitoring via a Telegram Bot. Using an **ESP32-CAM (WROVER)** alongside I2C sensor expanders and an external ADC, the system tracks food and water bowl weights, monitors reservoir levels, and captures photos whenever a pet approaches the station.

---

## ✨ Features
* **Scheduled Automated Feeding:** Dispenses food twice daily (at **8:00 AM** and **8:00 PM**) via a servo motor if the food bowl weight falls below the set threshold.
* **Automatic Water Refilling:** Continuously monitors the water bowl using a Force Sensitive Resistor (FSR) and activates a relay-controlled water pump whenever water levels drop.
* **Motion-Triggered Camera Alerts:** Uses an ultrasonic distance sensor to detect when a pet is within **30 cm** of the station, capturing a JPEG snapshot and sending it directly to Telegram (with a 5-minute cooldown).
* **Low-Reservoir Warnings:**
  * **Food Tank:** Uses a laser break-beam sensor to detect an empty food hopper and sends a Telegram alert.
  * **Water Tank:** Monitors water reservoir levels via analog sensing and notifies the user when a refill is needed.
* **Accurate Timekeeping:** Powered by a DS3231 Real-Time Clock (RTC) module to maintain accurate feeding schedules regardless of Wi-Fi interruptions.

---

## 🛠️ Hardware Components
* **Microcontroller:** ESP32-CAM (`CAMERA_MODEL_WROVER_KIT` pin configuration)
* **Real-Time Clock:** DS3231 I2C RTC Module
* **ADC Module:** ADS1115 16-Bit 4-Channel I2C ADC
* **I/O Expander:** PCF8574 I2C Expander (Address: `0x38`)
* **Actuators:**
  * Standard Servo Motor (Food dispenser gate)
  * 5V Relay Module + Water Pump (Connected to PCF8574 Pin `P0`)
* **Sensors:**
  * 2x Force Sensitive Resistors (FSR) for Food & Water bowl weight detection
  * Water Level Analog Sensor
  * Ultrasonic Distance Sensor (Echo pin read)
  * Laser Receiver / Break-Beam Sensor

---

## 🔌 Pinout & Wiring

### ESP32 Connections
| Component | Pin / Signal | ESP32 GPIO | Notes |
| :--- | :--- | :--- | :--- |
| **I2C Bus** | SDA | `GPIO 32` | Shared by RTC, ADS1115, and PCF8574 |
| **I2C Bus** | SCL | `GPIO 33` | Shared by RTC, ADS1115, and PCF8574 |
| **Feed Servo** | Signal | `GPIO 15` | PWM control for food gate |
| **Ultrasonic** | ECHO | `GPIO 13` | Measures pet proximity (`<= 30 cm`) |
| **Laser Sensor** | Signal | `GPIO 14` | `LOW (0)` triggers empty food tank alert |
| **Status LED** | Anode | `GPIO 2` | Onboard/External status indicator |

### I2C Expander & ADC Channels
| Module | Channel / Pin | Connected Device |
| :--- | :--- | :--- |
| **ADS1115** | `AIN0` | Water Bowl FSR Sensor |
| **ADS1115** | `AIN1` | Food Bowl FSR Sensor |
| **ADS1115** | `AIN2` | Main Water Tank Level Sensor |
| **PCF8574** | `P0` | Water Pump Relay (Active `LOW`) |

*(Camera pins follow the standard `CAMERA_MODEL_WROVER_KIT` layout defined in the source code).*

---

## 📚 Dependencies & Libraries
Install the following libraries through the **Arduino IDE Library Manager** before compiling:

1. [`UniversalTelegramBot`](https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot) by Brian Lough
2. [`ArduinoJson`](https://arduinojson.org/) by Benoit Blanchon
3. [`RTClib`](https://github.com/adafruit/RTClib) by Adafruit
4. [`PCF8574`](https://github.com/xreef/PCF8574_library) by Renzo Mischianti
5. [`Adafruit ADS1X15`](https://github.com/adafruit/Adafruit_ADS1X15) by Adafruit
6. [`ESP32Servo`](https://github.com/madhephaestus/ESP32Servo) by Kevin Harrington

---

## 🚀 Installation & Setup

1. **Clone the Repository:**
   ```bash
   git clone [https://github.com/your-username/smart-pet-feeder.git](https://github.com/your-username/smart-pet-feeder.git)
   cd smart-pet-feeder 