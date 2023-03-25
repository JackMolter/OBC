# SATFAB BPP Launch Code
---
## Introduction

### Background
The Satellite Fabrication (SATFAB) project is one of the teams that make up the Students for the Exploration and Development of Space (SEDS) at the University of Maryland. The team was founded to design build and test custom satellite components to eventually send to space.
### Goals
This project aims to validate the flight capability of our hardware and software. We have also documented progress in what will become a YouTube video to help promote the club.
### Contents
* Hardware schemtaics 
* Flight code and Ground station code
* Python code for graphical representation of the collected data
* Useful resources
* List of major and minor components with BOM and other helpful documentation

## Boards & Components 

* On Board Computer (OBC)
  * RP2040 Microcontroller 
  * DS2485 1-wire Master
  * MAX31820 Temperature Sensors 
  * BMI088 IMU
  * MS5607 Barometer (Broken)
  * W25Q128JV Flash Memory 

* OpenLST by Planet
  * CC1110
  * RFFM6404

* MS5607 Barometer module

* SAM-M8Q GPS antenna module
## FSM Model 

The Finite State Machine model controls what our satellite does during different phases of the mission. The satellite can only be in a single state at a time.
### Standby
### Flight View
### Flight
### Tree 
## Software Breakdown

### barometer.c 
barometer.c file initilizes the barometer module and has functions to collect temperature and pressure data from it. 
##### Quick info
* I2C0 protocol for MS5607 chip
* The module is set for 
  * CS pin set low
  * PS pin set high
* Pressure output is in millibar
* The barometer uses factory set PROM bytes to calibrate its temperature and pressure readings these do not change 
* The MS5607 is a sensitive chip and should be treated extra carefully 

### commands.c
Commands.c holds the current and any possible future command handleing for the satellite. It is currently equipped to process groundstation commands for state change or status requests. 
### flash.c
flash.c handles all the data storage for the OBC. This files main use is to store the data that is collected during the flight 
##### Quick info
* SPI protocol for W25Q128JV chip
* Chip is capable of duel or quad SPI, however current code is only capable of normal SPI communication speeds  
* The OBC has 2 flash chips, this code is only for data flash
*  ~12Mb of total storage
* flash.c should be optimized with the RP2040s DMA functionality in the future  
### GPS.c
GPS.c stores the data from the GPS module and uses the lwgps library to process it.
##### Quick info
* UART1 protocol
* 9600 Baud rate (Only component that doesnt use 115200)
 
##### Quick info
* GPS can require up to 2 minutes to get a fix
* GPS often can not get a fix indoors  
* GPS trasmits new info every second
* No setting have been changed on this module as of now 
### IMU.c
IMU.c is the code that collects the accelerometer, gyroscope, and temperature sensor data from the IMU. 
##### Quick info
* I2C0 protocol for BMI088 chip
* Output is in 1/1000 gs
* Possible issue found, nominal total acceleration when the board is at rest is only .66 of a g when it should be ~1g. A fudge factor has been added for now to deal with this
* THe first reading of the accelerometer is alway 0,0,0
* The accelerometer and gyro have deep sleep states that could be implemented in the future for power savings 
* Limited settings have been set in the code for simplicity and to save timecommunication speeds  

### LED.c
LED.c does sick fades and blinks for debugging and visual validation of state changes and transmission success.
### lwgps.c
lwgps.c is a library used to handle the GPS data and turn it into useful info. This file gets the latitude, longitude, altitude, # of satellites in view, speed, time, date, and GPS status for data collection. A few other metrics are also determined by it. 
### main.c
Controls FSM
### OpenLST.c
OpenLST.c is the file that contains the functions that handle all communication from the satellite to the groundstation. This file contains the send_packet function that is responsible for sending data to and from the satellite. 
##### Quick info
* UART protocol
* Some complexities of the data sending process are still being tested
* Possible issue with loss of power in OpenLST breaking FSM  
### .vscode
Contains a c_cpp file that is useful for helping your computer find the PICO_SDK file
### pico_sdk_import.cmake
Helps find the PICO_SDK

### CMakeLists.txt
Required to create the .uf2 file that is used to put the code onto the OBC




