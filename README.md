**This repository contains code used to test and control a 2000W nominal Mitsuba M2096 Brushless DC Motor**

The set up of the test is as follows:
- push buttons -> Arduino Uno -> CPC1002N solid state relays -> motor ECU switch input -> motor
- mechanical potentiometer -> Arduino Uno -> MCP4146 digital potentiometer -> motor ECU accel/regen input -> motor

The test consists of using the mechanical inputs of push buttons and mechanical pots to control the Mitsuba motor through a PCB designed to interface with the motor's electronic control unit (ECU).
An Arduino Uno was used to capture the signal from the push buttons through interrupts and map the voltage output of the mechanical pot to the SPI MOSI values necessary to communicate to the digital pot which then outputs acceleration and regeneration signals for the motor's ECU.

Some files in the repository directly communicate with the digital pot via SPI. Others utilize the arduino-mcp4xxx library https://github.com/jmalloc/arduino-mcp4xxx which offers higher levels of abstraction.


- CPC1002N SSR datasheet: https://www.ixysic.com/home/pdfs.nsf/www/CPC1002N.pdf/$file/CPC1002N.pdf

- MCP4146 Digital Pot datasheet: http://ww1.microchip.com/downloads/en/DeviceDoc/22060b.pdf
