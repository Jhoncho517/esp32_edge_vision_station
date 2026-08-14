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

## **Project Roadmap**
- [ ] Hardware identification ☑
- [ ] ESP32 development environment ☑
- [ ] GPIO bring-up
- [ ] Sensor integration
- [ ] Interrupt-driven events
- [ ] Servo/PWM control
- [ ] IR remote control
- [ ] Camera integration
- [ ] FreeRTOS architecture
- [ ] Sensor fusion
- [ ] Edge image processing
- [ ] Performance benchmarking
- [ ] Final demonstration

## **Hardware**
The project is being developed using components from an existing ESP32 development kit, including:

* ESP32 microcontroller
* ESP32 camera module/interface
* Camera
* PIR motion sensor
* IR sensor
* Micro servo motor
* Display
* IR remote
* Buzzer
* 74HC595 shift register
* Breadboard and jumper wires

*Note: The moisture sensor is intentionally not part of this project.*

---

## **Software**
Planned software stack:

* C/C++
* ESP-IDF
* FreeRTOS
* Git/GitHub
* Embedded peripheral APIs
* Camera/image processing libraries where appropriate

---

## **Planned Firmware Architecture**
The firmware will be organized around independent real-time tasks and event-driven communication. 

**Possible tasks include:**
* Sensor Task
* Camera Task
* Remote Control Task
* Servo Control Task
* Display Task
* Event/Decision Task

FreeRTOS queues, timers, interrupts, and task priorities will be explored as part of the implementation.

---

## **Planned Features**

### Phase 1 — Hardware Bring-Up
* ESP32 setup
* GPIO configuration
* Sensor testing
* Display testing
* Servo control

### Phase 2 — Real-Time Firmware
* GPIO interrupts
* Event queues
* FreeRTOS tasks
* Remote control
* Non-blocking firmware

### Phase 3 — Camera System
* Camera initialization
* Frame acquisition
* Camera/servo scanning
* Image-data handling

### Phase 4 — Sensor Fusion
Combine multiple sources of information to determine the likelihood of a meaningful event.

### Phase 5 — Edge Vision
Explore lightweight local image processing such as:
* Frame differencing
* Grayscale conversion
* Image resizing
* Region-of-interest processing
* TinyML/quantized inference if feasible on the hardware

### Phase 6 — Performance Analysis
The final system will be benchmarked using metrics such as:
* Event detection latency
* Camera capture latency
* Image processing time
* Servo response time
* RAM usage
* Flash usage
* CPU utilization

---

## **Why Edge Vision?**
Many intelligent systems rely on sending sensor and camera data to a remote server for processing. Edge computing moves some of this processing closer to the physical device. 

**This can potentially reduce:**
* Latency
* Bandwidth requirements
* Dependence on connectivity
* Unnecessary transmission of raw data

This project explores these concepts on a much smaller, resource-constrained embedded platform.

---

## Real-World Applications
The architecture explored in this project could eventually be applied to systems such as:
* Industrial monitoring
* Smart surveillance
* Wildlife monitoring
* Agriculture
* Logistics
* Retail analytics
* Robotics
* Distributed sensor networks

---

## Future Direction
A longer-term goal is to explore how a low-power microcontroller could work alongside more powerful edge-AI hardware.

Potential future work includes TinyML, more advanced computer vision, distributed ESP32 nodes, and integration with higher-performance edge-AI platforms.

---

## **Project Status**
🚧 **Currently in development**

The repository currently contains the project architecture, development plan, and documentation. Hardware implementation and firmware development are planned as the next stages.

---

## **Learning Objectives**
Through this project I aim to strengthen my understanding of:

* Embedded C/C++
* Microcontroller architecture
* Real-time operating systems
* Digital interfaces
* Interrupt-driven programming
* PWM and actuator control
* Camera systems
* Sensor fusion
* Computer vision
* Edge computing
* Performance optimization

---
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


