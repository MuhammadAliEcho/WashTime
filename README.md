# 🧺 WashTime

WashTime is an Arduino-based embedded system project that converts a semi-automatic washing machine into a fully automatic washing system.  
The project uses a microcontroller, relays, buttons, and an LCD to automate washing operations such as **Wash** and **Rinse**.

---

## ✨ Features

- ⚙️ Fully automatic washing cycle control  
- 🤖 Arduino-based embedded logic  
- 📟 I2C LCD display for real-time status  
- 🔘 Push-button user interface  
- 🔌 Relay-controlled motor and water valves  
- 🧩 Modular and extendable code structure  

---

## 🛠️ Hardware Requirements

- 🧠 Arduino Uno / Nano / compatible board  
- 🔁 4-Channel Relay Module  
- 🖥️ 16x2 LCD with I2C module  
- 🔘 Push Buttons (Start, Mode, Stop)  
- 🔋 5V Power Supply  
- 🔧 Wires and connectors  

> ⚠️ **Warning:**  
> This project may interact with AC-powered devices. Always follow proper electrical safety procedures.

---

## 💻 Software Requirements

- 🧰 Arduino IDE or PlatformIO  
- 🧑‍💻 C++ (Arduino framework)  
- 📚 LiquidCrystal_I2C library  

---

## 🔄 How It Works

1. 🔌 System initializes the LCD, relays, and buttons  
2. 👆 User selects a washing mode using buttons  
3. ▶️ Pressing the Start button begins the cycle  
4. ⚡ Arduino controls the motor and valves through relays  
5. 📟 LCD displays the current operation status  
6. ✅ The cycle completes automatically  

---

## 🔌 Pin Configuration (Example)

### 📟 LCD (I2C)

- VCC → 5V  
- GND → GND  
- SDA → A4  
- SCL → A5  

### 🔘 Buttons

- Start Button → D2  
- Mode Button → D3  
- Stop Button → D4  

### 🔁 Relays

- Relay 1 → Motor Control  
- Relay 2 → Water Inlet  
- Relay 3 → Drain Valve  
- Relay 4 → Reserved  

---

## 📥 Installation

1. 📦 Clone the repository  
   git clone https://github.com/MuhammadAliEcho/WashTime.git

2. 🧑‍💻 Open the project in Arduino IDE or PlatformIO  

3. 📚 Install required libraries  

4. ⚙️ Select the correct board and port  

5. ⬆️ Upload the code to the Arduino  

---

## ▶️ Usage

1. 🔌 Power on the system  
2. 🎛️ Select the desired washing mode  
3. ▶️ Press the Start button  
4. 👀 Monitor progress on the LCD  
5. ✅ The system completes the cycle automatically  

---

## 📁 Project Structure

- 📂 src/        → Main source code  
- 📂 include/    → Header files  
- 📄 README.md   → Project documentation  

---

## 🤝 Contributing

Contributions are welcome.

1. 🍴 Fork the repository  
2. 🌿 Create a new branch  
3. ✏️ Make your changes  
4. 🔁 Submit a pull request  

---

## 📜 License

This project is open source.  
You may add an MIT or Apache 2.0 license.

---

## 👤 Author

Muhammad Ali  
🔗 GitHub: https://github.com/MuhammadAliEcho  
📧 Email: muhammadaliecho@gmail.com  

---

⭐ If you find this project useful, consider giving it a star!
