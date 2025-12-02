# 🌦️ Weather-Based Music System

> A smart IoT system that detects real-time weather conditions and plays music to match the mood.

![Project Status](https://img.shields.io/badge/Status-Active-brightgreen)
![Language](https://img.shields.io/badge/Language-C%2B%2B%20%7C%20Python-blue)
![Hardware](https://img.shields.io/badge/Hardware-ESP32%20%7C%20Sensors-orange)

## 📖 About The Project

This project automates the music selection process based on the local environment. By using sensors (or weather APIs) to read temperature, humidity, and atmospheric conditions, the system intelligently selects a playlist that fits the current "vibe."

**Key Features:**
* **Real-time Sensing:** Reads Temperature and Humidity data instantly.
* **Smart Logic:** Maps weather ranges to specific music genres (e.g., Rain = LoFi, Sunny = Pop).
* **Visual Feedback:** Displays current weather and song info on an OLED screen.
* **Audio Output:** Connects to a buzzer/speaker or triggers a music API (like Spotify).

---

## 🛠️ Tech Stack & Hardware

### Hardware
* **Microcontroller:** ESP32 (or Arduino/Raspberry Pi)
* **Sensors:** DHT11 / DHT22 (Temperature & Humidity)
* **Display:** 0.96 inch OLED Display (I2C)
* **Audio:** Piezo Buzzer / Mini Speaker / DFPlayer Mini
* **Connectivity:** WiFi Module (Built-in on ESP32)

### Software & Tools
* **IDE:** Arduino IDE / PlatformIO / VS Code
* **Libraries:** * `Adafruit_GFX` (for OLED)
    * `DHT.h` (for Sensors)
    * `WiFi.h` (for API connectivity)

---

## ⚙️ Circuit Diagram

*(Replace this text with an image of your wiring or a Fritzing diagram)*
![Circuit Diagram](https://via.placeholder.com/600x300?text=Insert+Circuit+Diagram+Here)

**Pin Connections:**
| Component | ESP32 Pin |
|-----------|-----------|
| DHT Sensor Data | GPIO 4 |
| OLED SDA | GPIO 21 |
| OLED SCL | GPIO 22 |
| Speaker/Buzzer | GPIO 18 |

---

## 🚀 Installation & Setup

1.  **Clone the Repo**
    ```sh
    git clone [https://github.com/your-username/weather-music-system.git](https://github.com/your-username/weather-music-system.git)
    ```

2.  **Install Libraries**
    Open Arduino IDE, go to **Sketch > Include Library > Manage Libraries**, and install:
    * `Adafruit SSD1306`
    * `DHT sensor library`

3.  **Configure WiFi**
    Open the `main.ino` file and update your credentials:
    ```cpp
    const char* ssid = "YOUR_WIFI_NAME";
    const char* password = "YOUR_WIFI_PASSWORD";
    ```

4.  **Upload Code**
    Connect your ESP32 via USB and click the **Upload** button.

---

## 🎼 Logic Overview

The system follows this basic logic to determine the music:

| Temperature / Weather | Mood Category | Output |
|-----------------------|---------------|--------|
| < 15°C (Cold/Rainy) | **Cozy / Sad** | Plays Melancholic or LoFi tracks |
| 15°C - 25°C (Mild) | **Calm / Focus** | Plays Acoustic or Classical |
| > 25°C (Hot/Sunny) | **Energetic** | Plays Pop or Rock |

---
