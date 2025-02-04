# 💧 **Water-Tank Monitoring System** 🚀

## **🌍 Project Overview**

The **Water-Tank Monitoring System** brings smart technology to water management by integrating **ESP8266** and **ESP32 microcontrollers**. Experience real-time monitoring of water levels, temperature, and pressure with intelligent pump control—all in a sleek and user-friendly setup.

---

## **👨‍💻 Developers**
- 🎓 Santanu Roy

---

## **🔧 System Components**

### **🎛️ Main Unit**
- **Key Features:**
  - 🖥️ 16x2 I2C LCD for dynamic data display
  - 🎮 Four navigation buttons: Up, Down, OK, and Menu
  - 📡 Real-time display of water levels, temperature, and pressure
  - 📋 Menu-driven interface to configure pump thresholds
  - 🌐 Wireless communication with Tank and Pump units via **ESP-NOW**

### **🌊 Tank Unit**
- **Key Features:**
  - 📏 Ultrasonic sensor for precise water level measurements
  - 🌡️ BMP280 sensor for temperature and pressure monitoring
  - 🔄 Wireless communication with the Main Unit using **ESP-NOW**
- **Supported Boards:**
  - ⚙️ **ESP8266** for compact applications
  - 💪 **ESP32** for enhanced processing power and GPIO flexibility

### **💦 Pump Unit**
- **Key Features:**
  - ⚡ Efficient control of water pump operations via a relay module
  - 📨 Receives ON/OFF commands from the Main Unit
  - ⏱️ Automatic timeout to reset the pump if no command is received

---

## **🧰 Hardware Requirements**

- **🔩 ESP8266 or ESP32 Boards**
- **📡 Ultrasonic Sensor (HC-SR04)** for water level detection
- **🌡️ BMP280 Sensor** for temperature and pressure monitoring
- **⚙️ Relay Module** for pump control
- **🖥️ 16x2 I2C LCD Display**
- **🎛️ Push Buttons (x4)** for user input
- **🔋 Power Supply (5V/3.3V)**
- **🔗 Wiring and Breadboard** for circuit assembly

---

## **💻 Software Requirements**

- 🚀 Arduino IDE or PlatformIO
- 📡 ESP8266/ESP32 Core for Arduino
- 📜 LiquidCrystal_I2C Library
- 📶 ESP-NOW Communication Library
- 🌡️ Adafruit BMP280 Sensor Library
- 🔊 NewPing Library for ultrasonic sensor

---

## **⚙️ Installation and Setup**

1. **📥 Clone this Repository:**
   ```bash
   git clone https://github.com/YourUsername/WaterTankMonitoringSystem.git
   ```

2. **🛠️ Open the Project:**
   - Launch the project files in **Arduino IDE** or **PlatformIO**.

3. **⬆️ Upload Code:**
   - Upload the respective code files for **Main Unit**, **Tank Unit**, and **Pump Unit** to the corresponding ESP8266 or ESP32 boards.

4. **🔗 Hardware Connections:**
   - Properly connect sensors, buttons, and relays as specified in the code.

5. **⚡ Power On and Test:**
   - Power up the system and verify its functionality through the LCD and pump control behavior.

---

## **🚀 System Usage**

- 🎛️ Navigate the menu on the **Main Unit** to configure pump start and stop thresholds.
- 📊 Monitor real-time water tank parameters on the LCD display.
- ⚡ The system will automatically control the pump based on the configured thresholds.

---

## **📜 License**

This project is open-source and available under the [MIT License](LICENSE).

---

## **🤝 Contribution Guidelines**

We welcome contributions! To contribute:
1. 🍴 Fork the repository.
2. 🌿 Create a feature branch.
3. 💾 Commit changes and submit a pull request.

Thank you for contributing and making this project even better! 🎉

---

✨ **Happy Coding and Innovating!** 🚀

