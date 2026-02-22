# Bournville
Smart Irrigation System using ESP8266 & Blynk
This project is an IoT-based smart irrigation system designed to automate plant watering using real-time sensor data and cloud control. It uses soil moisture, tank level monitoring, and live weather information to make intelligent watering decisions.

The goal of this system is to reduce water wastage, protect the pump from dry running, and provide remote monitoring through the Blynk IoT platform.

🚀 Key Features

Automatic watering based on soil moisture

Ultrasonic tank monitoring (25 cm tank depth)

Pump protection when water level drops below 40%

Weather-aware irrigation (rain detection via API)

Manual pump control from mobile app

Real-time sensor monitoring on cloud dashboard

⚙️ How It Works

The system continuously reads:

Soil moisture level

Tank water level (using ultrasonic sensor)

Temperature & humidity (DHT11)

Weather data from OpenWeatherMap

Tank Level Calculation

Since the tank height is 25 cm, water percentage is calculated as:

Water % = ((25 - distance) / 25) × 100

The pump is allowed to operate only if:

Tank level is above 40%

Soil is dry

Rain is not detected

Auto mode is enabled

In Manual mode, the user can directly control the pump from the Blynk app.

🛠 Hardware Used

ESP8266 (NodeMCU)

Soil Moisture Sensor

HC-SR04 Ultrasonic Sensor

DHT11 Sensor

Relay Module (Active HIGH)

Water Pump

25 cm Water Tank

🌐 Software & Tools

Arduino IDE

Embedded C++

Blynk IoT Cloud

OpenWeatherMap API

WiFi communication

📱 Blynk Pin Configuration
Feature	Virtual Pin
Soil Moisture	V0
Tank Level %	V2
Temperature	V3
Humidity	V4
Pump Status	V5
Weather Condition	V6
Manual Pump	V7
Auto Mode	V8
🔒 Safety & Protection

Stops pump if tank level is below 40%

Prevents unnecessary watering during rain

Allows manual override

Provides remote monitoring for reliability

🌍 Applications

Home garden automation

Small-scale smart farming

Greenhouse monitoring

Water conservation systems

IoT-based academic projects

📈 Future Enhancements

AI-based watering prediction

Data logging & analytics dashboard

Solar-powered version

Mobile notifications

Multi-zone irrigation system
