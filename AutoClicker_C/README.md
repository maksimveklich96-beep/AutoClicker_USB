# STM32 Hardware USB Autoclicker

A robust, hardware-based USB autoclicker built on the STM32 microcontroller. Unlike software autoclickers, this device acts as a physical USB HID (Human Interface Device) mouse, making it virtually undetectable by anti-cheat systems or software monitors. 

It features an OLED display for real-time status monitoring, driven by a fault-tolerant FreeRTOS architecture that survives physical hardware disconnects.

## Features

* **Hardware HID Emulation:** Recognized by the host PC as a standard USB mouse.
* **Real-time OLED Display:** Shows current state, total clicks made, and click frequency.
* **Fault-Tolerant I2C Architecture:** Custom Finite State Machine (FSM) prevents FreeRTOS and system lockups if the OLED display is physically disconnected or experiences wire contact bounce (hot-plug safe).
* **Multitasking:** Built on FreeRTOS with isolated tasks for USB HID reporting and display rendering.
* **On-the-fly Control:** Toggle the clicker seamlessly via an onboard physical button.

##  Hardware Requirements

* **Microcontroller:** STM32 Development Board (e.g., STM32F401 series)
* **Display:** SSD1306 OLED Display (I2C)
* **Input:** Push button (currently mapped to the onboard user button)
* **Connecting Wires:** Standard dupont cables

##  Pinout & Connections

| Component | STM32 Pin | Note |
| :--- | :--- | :--- |
| **OLED SDA** | `PB9` *(adjust if different)* | I2C1 Data |
| **OLED SCL** | `PB8` *(adjust if different)* | I2C1 Clock |   
| **User Button** | `PC13` | Toggles the clicker on/off |
| **Status LED** | `PA5` | Toggles state alongside the clicker |
| **USB D+ / D-** | `PA12` / `PA11` | USB FS pins to host PC |

Below is a pinout diagram for quick reference.

<img width="849" height="661" alt="{E7BBC238-168E-4F3D-92C5-83555F1C1D26}" src="https://github.com/user-attachments/assets/c8df8b23-d4b6-4c71-80e2-245c47408172" />


## Software Architecture

This project is written in **C** using the **STM32 HAL**, **FreeRTOS**, and the **STM32 USB Device Library**.

### I2C Fault Tolerance (Display State Machine)
Total bus failure (HardFault/Deadlock) is a common problem, it happens when an I2C device is suddenly disconnected. This project implements a protective FSM (`Display_state_machine.c`) that shields the RTOS from hardware failures:
1. **Pinging:** The system non-blockingly probes the I2C bus (`HAL_I2C_IsDeviceReady`) before any display update.
2. **State Segregation:** If the ping fails, the display task safely falls back to a `Display_is_lost` state, yielding processor time back to the USB clicker task.
3. **Safe Reboot:** Upon physical reconnection, the FSM implements software debouncing and gracefully re-initializes the SSD1306 registers (`Display_is_rebooting`) before resuming frame updates.

## Build & Flash

1. Clone this repository.
2. Open the project in **STM32CubeIDE**.
3. Build the project (`Project -> Build All`).
4. Connect your STM32 board via ST-Link.
5. Click **Debug/Run** to flash the firmware onto the microcontroller.

##  Roadmap

- [x] Basic USB HID clicker functionality
- [x] FreeRTOS integration
- [x] OLED Status Display
- [x] I2C Hot-plug protection and State Machine
- [ ] UI/Button logic to adjust click frequency on the fly
