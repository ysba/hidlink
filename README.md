# hidlink

DIY hardware that links a Bluetooth keyboard to an USB port.

Useful when you want a Bluetooth keyboard to be available during boot time on desktop computers (BIOS or dual-boot selection).

Based on ESP32 and Raspberry Pi Pico.

# Table of contents

<!--ts-->
* [Introduction](#introduction)
* [Set-up](#set-up)
  * [ESP32 Set-up](#esp32-set-up)
  * [Raspberry Pi Pico Set-up](#raspberry-pi-pico-set-up)
* [Hardware](#hardware)
  * [Breadboard](#breadboard)
  * [PCB](#pcb)
* [Protocols](#protocols)
  * [BLE](#ble)
    * [Advertising](#advertising)
    * [GATT Server](#gatt-server)
    * [Custom Data Transfer Profile](#custom-data-transfer-profile)
    * [hidlink BLE Protocol](#hidlink-ble-protocol)
  * [UART](#uart)
    * [hidlink UART Protocol](#hidlink-uart-protocol)
* [Development](#development)
  * [ESP32 Development](#esp32-development)
  * [Raspberry Pi Pico Development](#raspberry-pi-pico-development)
<!--te-->


# Introduction

[Back to top](#table-of-contents)

# Set-up 

[Back to top](#table-of-contents)

## ESP32 Set-up

[Back to top](#table-of-contents)

## Raspberry Pi Pico Set-up

[Back to top](#table-of-contents)

# Hardware

[Back to top](#table-of-contents)

## Breadboard

![Breadboard](doc/hidlink_bb.png)

Layout made with [Fritzing](https://fritzing.org/).

* Power and USB communication are plugged via the **Raspberry Pi Pico** board (micro USB cable).
  * **ESP32-DevKitV1** ``VIN`` connects to **Raspberry Pi Pico** ``VBUS``.
  * **ESP32-DevKitV1** ``GND`` connects to **Raspberry Pi Pico** ``GND``.
* **ESP32-DevKitV1** board is powered via the ``RED`` and ``BLUE`` wires.
* Communication between **ESP32-DevKitV1** and **Raspberry Pi Pico** is made via the orange wire:
  * **ESP32-DevKitV1** ``U2_TXD`` connects to **Raspberry Pi Pico** ``UART0 RX``.
  * The 1K resistor is not really needed, but is advisable to avoid  damaging of the digital pins due electromagnetic induction.
  

[Back to top](#table-of-contents)

## PCB

[Back to top](#table-of-contents)

# Protocols

[Back to top](#table-of-contents)

## BLE

[Back to top](#table-of-contents)

### Advertising

[Back to top](#table-of-contents)

### GATT Server

[Back to top](#table-of-contents)

### Custom Data Transfer Profile

[Back to top](#table-of-contents)

### hidlink BLE Protocol

[Back to top](#table-of-contents)

## UART

[Back to top](#table-of-contents)

### hidlink UART Protocol

[Back to top](#table-of-contents)

# Development

[Back to top](#table-of-contents)

## ESP32 Development

[Back to top](#table-of-contents)

## Raspberry Pi Pico Development

[Back to top](#table-of-contents)

