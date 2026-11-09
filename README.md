 WSN-BASED INTELLIGENT BUILDING AUTOMATION USING PLC

 1. Introduction

Building automation is an important application of industrial automation technology that improves monitoring, safety, energy efficiency and operational control. Wireless Sensor Networks (WSN) enable the collection and transmission of environmental data from distributed sensors.

This project focuses on developing an intelligent building automation system by integrating ESP32-based wireless sensor nodes with a Siemens S7-1200 Programmable Logic Controller (PLC). The system uses Modbus TCP communication to exchange sensor information and facilitate centralized monitoring and automation.

 2. Project Overview

The proposed system combines wireless sensor technology and industrial automation to monitor building environmental conditions. ESP32 collects data from different sensors and communicates with the Siemens PLC through a wireless network.

The PLC processes the received information using programmed control logic and supports monitoring and automated operations. An HMI provides a graphical interface for displaying environmental parameters and system status.

 3. Objectives

- To develop an intelligent building automation system using wireless sensor networks.
- To monitor environmental parameters such as temperature, humidity, smoke and flame conditions.
- To establish communication between ESP32 and Siemens S7-1200 PLC.
- To implement industrial communication using Modbus TCP.
- To understand PLC-based monitoring and automation.
- To provide centralized environmental monitoring through an HMI.
- To improve building safety, operational efficiency and automation.

 4. System Architecture

The system consists of wireless sensor nodes, a central PLC controller and a Human Machine Interface (HMI).

The sensors collect environmental information, which is acquired by ESP32-based wireless nodes. The collected data is transmitted to the Siemens S7-1200 PLC using Modbus TCP communication.

The PLC processes the received information according to predefined control logic. The HMI provides visualization of the monitored parameters and system conditions.

5. Hardware Components

The project incorporates the following hardware components:

- ESP32 Microcontroller
- Siemens S7-1200 PLC
- DHT11 Temperature and Humidity Sensor
- Gas and Smoke Sensor
- Flame Sensor
- Rain Sensor
- Servo Motor
- Siemens HMI
- Power Supply Unit

These components work together to perform environmental monitoring, data acquisition and automation functions.

 6. Software and Technologies

The project involves the use of industrial automation and embedded system technologies.

**ESP32 Programming:** Arduino IDE is used for programming the ESP32 and interfacing environmental sensors.

**PLC Programming:** Siemens TIA Portal V19 is used for configuring the S7-1200 PLC and developing ladder logic.

**Communication Protocol:** Modbus TCP is used for data exchange between the ESP32 and Siemens PLC.

**HMI Visualization:** Siemens HMI is used for displaying sensor readings and monitoring system conditions.

 7. Wireless Sensor Network (WSN)

A Wireless Sensor Network consists of distributed sensor nodes that collect environmental information and communicate the acquired data to a central processing or control system.

In this project, ESP32 functions as a wireless sensor node that collects data from connected sensors. Wireless communication enables sensor information to be transmitted to the central PLC without requiring direct wired connections between every sensor and the controller.

WSN technology provides flexibility, scalability and simplified sensor integration in building automation applications.

 8. Modbus TCP Communication

Modbus TCP is an industrial communication protocol used for exchanging information between electronic devices over Ethernet networks.

It follows a client-server communication architecture in which one device initiates communication requests while another device responds with the requested information.

In this project, the Siemens S7-1200 PLC acts as the Modbus TCP client, while ESP32 operates as the Modbus TCP server.

The MB_CLIENT instruction in Siemens TIA Portal facilitates communication between the PLC and ESP32.

Modbus TCP offers simple implementation, reliable data transmission and compatibility with various industrial automation devices.

 9. PLC-Based Automation

A Programmable Logic Controller (PLC) is an industrial digital controller used to automate processes and control machinery.

The Siemens S7-1200 PLC serves as the central controller in this project. It receives environmental data, processes the information and executes the programmed automation logic.

Ladder Logic is used to develop control sequences and monitoring functions. PLC-based automation provides reliable operation, flexibility and centralized process control.

 10. Human Machine Interface (HMI)

A Human Machine Interface is a graphical interface that enables communication between operators and industrial control systems.

In this project, the Siemens HMI provides a visual representation of environmental parameters and system conditions.

It enables operators to observe temperature, humidity, smoke levels and other monitored conditions through graphical indicators and numerical displays.

HMI technology improves system visibility and simplifies monitoring operations.

 11. Working Principle

The system operates through the integration of wireless sensing, industrial communication and PLC-based automation.

Initially, environmental sensors detect changes in surrounding conditions. ESP32 acquires the sensor information and transmits it to the Siemens S7-1200 PLC through Modbus TCP communication.

The PLC receives and processes the sensor data using programmed control logic. Based on the configured conditions, the controller performs the required monitoring and automation operations.

The HMI displays the relevant environmental information, enabling centralized supervision of the building automation system.

 12. Applications

- Smart Building Automation
- Industrial Environmental Monitoring
- Fire and Smoke Detection Systems
- Energy Management Systems
- Industrial Internet of Things (IIoT)
- Wireless Monitoring Systems
- Process Automation
- Intelligent Infrastructure Management

 13. Advantages

- Enables real-time environmental monitoring.
- Reduces extensive sensor wiring requirements.
- Supports centralized PLC-based control.
- Provides flexible wireless communication.
- Improves monitoring and operational efficiency.
- Supports integration of multiple sensors.
- Offers scalability for future automation requirements.
- Combines industrial automation with wireless technologies.

 14. Conclusion

The WSN-Based Intelligent Building Automation Using PLC project demonstrates the integration of wireless sensor networks, embedded systems, industrial communication and PLC-based automation.

By combining ESP32, Siemens S7-1200 PLC, Modbus TCP and HMI technologies, the proposed system provides an approach to intelligent environmental monitoring and building automation.

The project provides practical exposure to wireless data acquisition, industrial communication protocols, PLC programming and automation system integration.

 15. Future Scope

The system can be further enhanced through cloud-based monitoring, mobile application integration, advanced energy management, predictive maintenance and additional wireless sensor nodes.

Integration with artificial intelligence and IoT platforms can also support intelligent decision-making and advanced building management.

 16. Author

Diksha Meshram 
Instrumentation Engineering Student  
Government College of Engineering, Amravati (GCOEA)

GitHub: [Diksha33](https://github.com/Diksha33)

Academic Engineering Project | 2026–2027
