# IntelliSync Traffic Management System (ISTMS)

**A Multi-Modal Approach for Adaptive Urban Traffic Control**

Developed as part of the *BCSE316L: Digital Assessment-2 (Winter Semester 2024-25)* by **Tanisha Jauhari** and **Kunj Goel**, the **IntelliSync Traffic Management System (ISTMS)** presents a hybrid, fault-tolerant solution to modern urban traffic congestion using embedded systems and computer vision.

---

## 🚦 Overview

India’s urban centers like Delhi and Bangalore face severe traffic congestion, leading to $22 billion in economic losses annually. ISTMS tackles this with a real-time, sensor-integrated traffic management system that combines:

- Metal detection sensors  
- ESP32-CAM computer vision  
- Adaptive control logic  
- RGB visual indicators  
- Fault-tolerant system design

The system aims to reduce waiting time by 40% and provide 99.9% uptime using multi-sensor redundancy.

---

## 🧠 Key Features

- **Multi-Sensor Fusion**: Combines metal detectors and camera-based computer vision for robust vehicle detection.
- **Adaptive Traffic Logic**: Dynamically adjusts green light durations based on real-time traffic flow and vehicle count.
- **Visual LED Feedback**: WS2812B RGB LEDs inform incoming vehicles of signal status.
- **Fault Tolerance**: Operates seamlessly even if one sensor modality fails.
- **Embedded Control**: ESP32 microcontrollers handle local sensing and actuation, while computer vision runs on a connected laptop/server.

---

## 🛠 System Architecture

### 🔩 Hardware Components

| Component            | Description                                      |
|---------------------|--------------------------------------------------|
| **ESP32**           | Main controller for sensors and LEDs             |
| **ESP32-CAM**       | Streams video for vehicle counting               |
| **Metal Detectors** | Sense vehicle presence with analog output        |
| **WS2812B RGB LEDs**| Color-coded signals (Red, Yellow, Green)         |
| **Servo Motor**     | Rotates camera to active directions              |

### 🧱 Subsystems

1. **Sensing**: Metal sensors + ESP32-CAM module
2. **Processing**: ESP32 + companion laptop/server
3. **Indicators**: RGB LEDs per intersection arm
4. **Communication**: Serial and Wi-Fi between ESP32 and server

---

## 👩‍💻 Software Modules

- **Sensor Management Module**: Reads and filters metal sensor inputs
- **Vision Processing Module**: Controls camera direction and captures images
- **Decision Engine**: Calculates priority based on vehicle counts
- **Indicator Control**: Updates LEDs based on system state
- **Communication Interface**: Handles serial and Wi-Fi links

