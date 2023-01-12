# STM32 IoT Weather Station

A weather station that allows both local and remote monitoring of temperature and relative humidity.

### Equipment

- STM32 Nucleo development board
- Hitachi HD44780U based LCD screen
- DHT11/22 (AM2302) temperature and humidity sensor
- NodeMCU ESP-12E Development Board with ESP8266 WiFi module

### Software Prerequisites

- STM32CubeIDE
- Arduino IDE
- LCD, DHT sensor, and ESP8266WiFi drivers

  #### LiquidCrystal
    A Hitachi HD44780U based LCD screen driver for the STM32.
    An adaptation of a driver originally written by Anas Salah Eddin (California State Polytechnic University, Pomona) for the PIC18F4321 microcontroller. Inspired by Arduino's LiquidCrystal library.  
    The library makes use of STM32's HAL definitions, and therefore should be fairly portable between different platforms within STM32's line of products.  
    The driver is configured to use 11 pins (3 control, and 8 data) to receive text and control information.  
    Future improvement of the driver may include downsizing the number of pins from 11 to 2 through use of an I2C module.  
    
