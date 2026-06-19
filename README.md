# 🌊 QingLian-I (清涟一号) — Autonomous Surface Cleaning Robot

<div align="center">

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-STM32%20%7C%20OpenMV-orange)]()
[![Detection](https://img.shields.io/badge/Detection-YOLOv8%20%7C%20TensorFlow%20Lite-brightgreen)]()
[![Accuracy](https://img.shields.io/badge/Accuracy-90.27%25-success)]()

**An autonomous water-surface robot for floating debris detection and collection,**

**powered by edge-AI vision and real-time embedded control.**

[中文说明](#中文说明) | [System Architecture](#system-architecture) | [Key Results](#key-results) | [Repository Structure](#repository-structure)

</div>

---

## Overview

**QingLian-I** (清涟一号) is an autonomous surface cleaning robot that integrates **computer vision**, **embedded real-time control**, and **mechanical collection** to detect and retrieve floating debris on water surfaces. The system runs a YOLOv8-based object detector on the edge (OpenMV / Raspberry Pi), communicates detections to an STM32F103 microcontroller via UART, and executes coordinated motion control through cascaded PID loops with IMU-based attitude stabilization.

Built as an independent undergraduate research project (2024–2025), this repository demonstrates the complete **perception → decision → execution** pipeline on a resource-constrained embedded platform.

### Highlights

- **90.27% detection accuracy @ 20 FPS** on a custom-annotated floating-debris dataset
- **Edge-native deployment**: YOLOv8 → ONNX export → TensorFlow Lite conversion for OpenMV
- **Data augmentation pipeline**: Mosaic augmentation + synthetic water-reflection noise (Torchvision + Albumentations)
- **Sensor fusion**: MPU6050 6-axis IMU with Kalman filtering for attitude estimation
- **Cascaded PID control**: outer angle loop + inner rate loop for stable differential-drive maneuvering
- **Multi-modal communication**: Bluetooth telemetry, wireless RC, UART camera link

---

```mermaid
graph TB
    %% ===== STYLES =====
    classDef perception fill:#083344,stroke:#22d3ee,stroke-width:2px,color:#cffafe,font-size:13px
    classDef control fill:#064e3b,stroke:#34d399,stroke-width:2px,color:#d1fae5
    classDef sensor fill:#1e1b4b,stroke:#a78bfa,stroke-width:1.5px,color:#e0e7ff
    classDef comm fill:#3b0764,stroke:#c084fc,stroke-width:1.5px,color:#f3e8ff
    classDef actuator fill:#451a03,stroke:#fbbf24,stroke-width:2px,color:#fef3c7
    classDef external fill:#0f172a,stroke:#94a3b8,stroke-width:1px,color:#e2e8f0

    %% ===== DATA FLOW =====
    CAM[📷 OV5640 Camera<br/>QVGA 320×240]:::perception
    DETECT[🧠 TensorFlow Lite Detector<br/>YOLOv8 → ONNX → TFLite int8<br/>Inference: 20 FPS]:::perception
    CAM --> DETECT

    PROTO[UART Protocol Parser<br/>Frame: 0xB3..0x5B<br/>Payload: x, y, dist, class]:::control

    DETECT -->|UART 115200 bps| PROTO

    IMU[MPU6050 IMU<br/>3-axis Gyro + Accel<br/>I2C @ 400 kHz]:::sensor
    KALMAN[Kalman Filter<br/>Attitude Estimation<br/>Accuracy: ±2°]:::control
    IMU --> KALMAN

    SONAR[HC-SR04 Ultrasonic<br/>Range: 2–200 cm<br/>Obstacle Detection]:::sensor

    PID[Cascaded PID Controller<br/>Outer: Angle → Inner: Rate<br/>Update Rate: 1 kHz]:::control
    PROTO --> PID
    KALMAN --> PID
    SONAR --> PID

    BT[Bluetooth HC-05<br/>Telemetry + RC Commands<br/>UART Transparent Bridge]:::comm
    PID <-.->|Status & Telemetry| BT

    PWM[L298N Dual H-Bridge<br/>TIM1 PWM @ 20 kHz<br/>Differential Drive]:::actuator
    MOTOR_L[Left DC Motor<br/>JGA25-370 @ 100 RPM]:::actuator
    MOTOR_R[Right DC Motor<br/>JGA25-370 @ 100 RPM]:::actuator

    PID -->|PWM Duty Cycle| PWM
    PWM --> MOTOR_L
    PWM --> MOTOR_R

    USER[👤 Operator<br/>Wireless RC / App]:::external
    USER <-.->|2.4 GHz RC| BT
```

### Perception Pipeline

1. **Training**: YOLOv8s trained on thousands of self-annotated images with Mosaic + synthetic reflection augmentation
2. **Export**: PyTorch → ONNX → TensorFlow Lite (int8 quantization)
3. **Deployment**: TFLite model runs on OpenMV Cam H7 Plus at QVGA (320×240), achieving ~20 FPS inference
4. **Communication**: Detected object centroids `(x, y)` and estimated distance sent to STM32 via UART

### Control Pipeline

1. **Attitude Estimation**: MPU6050 raw gyro + accelerometer → Kalman filter → Euler angles (roll, pitch, yaw)
2. **Cascaded PID**: Outer loop (angle error → desired rate) → Inner loop (rate error → motor PWM)
3. **Motion Execution**: Differential drive PWM signals to dual DC motors via L298N H-bridge
4. **Obstacle Avoidance**: HC-SR04 ultrasonic sensor triggers emergency stop / spin-search when debris < 10 cm

---

## Key Results

| Metric | Value |
|--------|-------|
| **Detection Accuracy** | 90.27% (mAP@0.5 on custom floating-debris test set) |
| **Inference Speed** | 20 FPS (OpenMV Cam H7 Plus, TFLite int8) |
| **Model Size** | < 500 KB (quantized TFLite) |
| **Control Loop Frequency** | 1 kHz (IMU read + PID update) |
| **Attitude Estimation Accuracy** | ±2° (Kalman-filtered MPU6050) |
| **Obstacle Detection Range** | 2–200 cm (HC-SR04 ultrasonic) |

---

## Repository Structure

```
SurfaceCleaningRobot/
├── README.md                         # This file
├── LICENSE                           # Apache 2.0
├── .gitignore
├── firmware/
│   ├── openmv/
│   │   ├── main.py                   # Edge-AI object detection + UART communication
│   │   └── pid.py                    # Python PID controller (OpenMV side)
│   └── stm32/
│       ├── control.c / control.h     # Motion control logic + PID orchestration
│       ├── pid.c / pid.h             # C implementation of cascaded PID
│       ├── motor.c / motor.h         # PWM motor driver (TIM1, differential drive)
│       ├── kalman.c / kalman.h       # Kalman filter for IMU sensor fusion
│       ├── mpu6050.c / mpu6050.h     # MPU6050 6-axis IMU I²C driver
│       ├── echo.c / echo.h           # HC-SR04 ultrasonic sensor driver
│       ├── openmv.c / openmv.h       # UART protocol parser for OpenMV data
│       └── control_tracking.c / .h   # Camera-guided tracking controller
├── hardware/
│   ├── schematic.pdf                 # System circuit schematic
│   ├── design.jpg                    # Mechanical / PCB design drawing
│   └── circuit_design.txt            # Pin mapping reference
└── docs/
    └── images/                       # System photos & diagrams
```

---

## Quick Start

> **Note**: This repository showcases the core algorithms and system architecture. Full model weights, training datasets, and complete Keil MDK project files are not included. The provided code demonstrates the engineering patterns sufficient for understanding and re-implementing the approach.

### Prerequisites

- **Hardware**: STM32F103C8T6 (Blue Pill / Mini), OpenMV Cam H7 Plus, MPU6050 IMU, HC-SR04 ultrasonic, L298N motor driver, dual DC motors
- **Software**: Keil MDK-ARM v5, OpenMV IDE, Python 3.8+ (for training pipeline)

### Building the STM32 Firmware

```bash
# 1. Open Keil MDK-ARM v5
# 2. Create a new project for STM32F103C8
# 3. Add source files from firmware/stm32/ to the project
# 4. Configure STM32F10x Standard Peripheral Library
# 5. Build and flash via ST-Link / serial bootloader
```

### Running the OpenMV Detector

```python
# 1. Connect OpenMV Cam H7 Plus via USB
# 2. Open OpenMV IDE
# 3. Copy firmware/openmv/main.py and pid.py to the camera
# 4. Deploy your trained TFLite model as "trained.tflite"
# 5. Create labels.txt with your class names
# 6. Run — the camera will stream detections over UART3
```

### Training a Custom Detector

```python
# Conceptual training pipeline (not included in this repo)
from ultralytics import YOLO

# 1. Train YOLOv8 on annotated debris images
model = YOLO('yolov8s.pt')
model.train(data='debris_dataset.yaml', epochs=100, imgsz=320)

# 2. Export to ONNX
model.export(format='onnx', imgsz=320, int8=True)

# 3. Convert ONNX → TFLite (use edgeimpulse or onnx2tf)
# 4. Deploy to OpenMV as trained.tflite
```

---

## Hardware Specifications

| Component | Model | Notes |
|-----------|-------|-------|
| **Main MCU** | STM32F103C8T6 | 72 MHz Cortex-M3, 64 KB Flash, 20 KB SRAM |
| **Vision Module** | OpenMV Cam H7 Plus | STM32H743, 480 MHz, 1 MB SRAM, OV5640 sensor |
| **IMU** | MPU6050 | 6-axis (3 gyro + 3 accel), I²C interface |
| **Ultrasonic** | HC-SR04 | 2–200 cm range, 3 mm precision |
| **Motor Driver** | L298N / TB6612 | Dual H-bridge, up to 2A per channel |
| **Motors** | DC geared (JGA25-370) | 12V, 100 RPM, encoder optional |
| **Bluetooth** | HC-05 | UART transparent transmission |
| **Power** | 11.4V Li-ion (3S) | ~30 min runtime at full load |

### Pin Mapping (STM32F103C8T6)

| Function | GPIO Pin | Peripheral |
|----------|----------|------------|
| Motor PWM L | PB8, PB9 | TIM4 CH3, CH4 |
| Motor PWM R | PA0, PA1 | TIM2 CH1, CH2 |
| Motor DIR | PB14, PB15, PA6, PA7 | GPIO |
| MPU6050 I²C | PB10 (SCL), PB11 (SDA) | I2C2 |
| OpenMV UART | PA2 (TX), PA3 (RX) | USART2 |
| Bluetooth UART | PB10 (TX), PB11 (RX) | USART3 |
| Ultrasonic Trig | PB7 | GPIO |
| Ultrasonic Echo | PB6 | TIM4 CH1 (input capture) |
| Battery ADC | PA4, PA5, PA6 | ADC1 CH4, CH5, CH6 |
| Status LED | PB5 | GPIO |

---

## 中文说明

### 项目简介

**清涟一号** 是一款基于边缘AI视觉的自主式水面清洁机器人，集成计算机视觉、嵌入式实时控制与机械打捞装置，用于自动检测和收集水面漂浮垃圾。系统在 OpenMV 摄像头上运行 YOLOv8 目标检测模型，通过 UART 将检测结果传输至 STM32F103 主控芯片，经级联 PID 控制算法驱动双电机差速转向，实现自主巡航与定点打捞。

### 核心技术

- **边缘AI推理**：YOLOv8s → ONNX → TensorFlow Lite (int8量化)，在OpenMV H7 Plus上实现 20 FPS 实时检测
- **数据增强**：Mosaic + 合成水面反光噪声 (Torchvision + Albumentations)，提升复杂光照下的鲁棒性
- **姿态解算**：MPU6050 六轴IMU + 卡尔曼滤波，±2° 姿态估计精度
- **级联PID控制**：外环角度环 + 内环角速度环，实现稳定的差速转向控制
- **多模通信**：蓝牙数传、无线遥控、UART视觉链路

### 性能指标

- 漂浮垃圾检测准确率：**90.27%** (自定义数据集，mAP@0.5)
- 推理速度：**20 FPS** (OpenMV, QVGA分辨率)
- 控制回路频率：**1 kHz** (IMU读取 + PID更新)
- 续航时间：约 30 分钟 (11.4V / 3S锂电池)

### 注意事项

本仓库展示核心算法与系统架构，**完整模型权重、训练数据集、Keil MDK 工程文件未包含在内**。所提供的代码足以理解系统设计思路并进行二次开发，但直接复现完整系统需要自行完成模型训练与嵌入式工程配置。

---

## Acknowledgements

- **YOLOv8** by Ultralytics — base detection architecture
- **OpenMV** — embedded machine vision platform
- **STM32 Standard Peripheral Library** — hardware abstraction
- **InvenSense MPU6050 DMP** — motion processing algorithms

## License

This project is licensed under the Apache License 2.0 — see [LICENSE](LICENSE) for details.

---

<div align="center">

*Built with ❤️ for cleaner waters.*

</div>
