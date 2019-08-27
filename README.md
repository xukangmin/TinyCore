Arduino Core Support for Attiny 1 Series MCU
==============================================================================
# Links
  - [Documentation](https://docs.tinycore.dev)
  - [Tiny Core 16 Board]( https://www.tindie.com/products/16834/)
  - [Tiny Core 32 Board]( https://www.tindie.com/products/17279/)
  - [Programmer Board]( https://www.tindie.com/products/16835/)
  
# Feature
 Specifications |  .
------------ | -------------
Flash (program memory)   | 32/16 KB
RAM  | 2 KB
EEPROM | 256 bytes
Bootloader | No
GPIO Pins | 18
ADC Channels | 10
PWM Channels | 3
Peripheral | USART, SPI, I2C, Touch
Clock | 20 MHz
Power Consumption | min 2.9μA, max 10mA

# Arduino Support
 Function |  .
------------ | -------------
UART   | :heavy_check_mark:
GPIO  | :heavy_check_mark:
ADC | :heavy_check_mark:
SPI | :heavy_check_mark:
I²C | :heavy_check_mark:
DAC | :heavy_check_mark:
Servo | :heavy_check_mark:
EEPROM | :heavy_check_mark:
NeoPixel | :heavy_check_mark:
Touch | :heavy_check_mark:
E-ink | :hourglass_flowing_sand:
Motor | :hourglass_flowing_sand:
 
# Programmer
 Attiny 1 Series use UDPI protocol for programming which only use 1 pin for programming which is PA0. 
 I made a ATMega32u4 firmware and a programmer board [link] for Attiny 1 Series.
 ATMega328p will also works.
  
# Installation
- Install the current upstream Arduino IDE at the 1.8.7 level or later. The current version is at the [Arduino website](http://www.arduino.cc/en/main/software).
- Start Arduino and open Preferences window.
- Enter ```https://raw.githubusercontent.com/xukangmin/TinyCore/master/avr/package/package_tinycore_index.json``` into *Additional Board Manager URLs* field. You can add multiple URLs, separating them with commas.
- Open Boards Manager from Tools > Board menu and install *TinyCore* platform (and don't forget to select your TinyCore board from Tools > Board menu after installation).

# Pin Mapping

- Tiny Core 16 Pinout

![Pin Mapping](avr/docs/images/TinyCore16_Pinout.png)

- Tiny Core 32 Pinout

![Pin Mapping](avr/docs/images/TinyCore32_Pinout.png)