# ESP32 Edge Vision Station 🚧

> A real-time embedded edge-vision system designed to explore how resource-constrained devices can sense, process, and respond to physical events locally.

**Status: In Development**

## Overview

The **ESP32 Edge Vision Station** is an embedded systems project built around an ESP32 microcontroller and camera. The goal is to develop a small-scale edge-perception platform capable of detecting environmental events, processing information locally, and responding through physical actuators.

Rather than relying entirely on cloud processing, the project explores how sensing, decision-making, and basic computer vision can be performed directly at the edge under constraints in processing power, memory, latency, and connectivity.

## Project Goals

- Build a real-time embedded system using ESP32
- Integrate camera-based sensing with physical sensors
- Develop an event-driven firmware architecture using FreeRTOS
- Implement sensor fusion for more reliable event detection
- Control a servo-based camera scanning system
- Process camera data locally where the hardware permits
- Measure latency, memory usage, and system performance
- Explore the foundations of edge AI and computer vision

## Planned System Architecture

```text
             Camera
                |
                v
        +---------------+
        |     ESP32     |
        |               |
        |  Event Engine |
        +-------+-------+
                |
       +--------+--------+
       |        |        |
       v        v        v
     PIR    IR Sensor  Remote
       |        |        |
       +--------+--------+
                |
                v
          Sensor Fusion
                |
                v
        Decision / Control
                |
        +-------+-------+
        |       |       |
        v       v       v
      Servo  Display  Buzzer
