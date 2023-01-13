# STM32 IoT Weather Station

A weather station that allows both local and remote monitoring of temperature and relative humidity.

### Equipment

- STM32 Nucleo development board
- Hitachi HD44780U based LCD screen
- DHT11/22 (AM2302) temperature and humidity sensor
- NodeMCU ESP-12E Development Board with ESP8266 WiFi module

<p align="center">
  <img src="https://github.com/YaakovIvanov/STM32-Weather-Station/blob/main/Img/STM32_Weather_Station_edited.jpg" width=70% height=70%>
 </p>

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
  #### DHTemp
    An original driver for the DHT11/22 (AM2302) temperature and humidity sensor from one of my other repositories.  
    This driver also provides the benifit of STM32 portability through the use of their HAL definitions.  
    This library makes use of a function defined in "Delay.h" which uses a timer to attain delay in micro seconds.  
  #### ESP8266
    Internet connectivity for this project is attained through the use of an ESP8266 WiFi module.  
    The NodeMCU ESP-12E Development Board allows for programming the module with Arduino IDE and C++ rather than using AT commands to control the chip.  
    The ESP8266WiFi library provides support for the module in the Arduino IDE, while the ThingSpeak library simplifies communication with the IoT platform.
    
### Description
    
    Using the DHTemp driver, the STM32 chip is configured to request an update on weather conditions from the DHT sensor about once a second (may be modified). The chip then recieves the data, and updates the information printed on the LCD accordingly.
    A seperate timer on the chip is configured to trigger an interrupt about once every 5 minutes (may be modified) to transmit the weather conditions at that instance to the NodeMCU ESP-12E via UART. The NodeMCU then sends the data to a ThingSpeak channel via WiFi.
    
