# TempTrack
Integrated System for Commercial Foodservice Temperature Analysis and Logging

This is version 1 of Arduino-API based temperature monitoring and analysis system, developed with the intent to ultimately be used in the Food Service industry to assist with USDA regulatory compliance, and as a means to improve food safety through proactive monitoring of potentially hazardous foods while they cool. 

Initially written in C++ via Arduino API, the hardware consists of DallasTemperature's DS18B20 temperature sensors, ATmega328p MCU for satellite probes (still under construction), and most likely ATmega2560 to drive central control module.  Datalogging to microSD card for v1, but I anticipate either serial port upload to system database (java derby ) or direct wifi upload to cloud storage in the coming versions :)

This version of the code was designed with a wired probe configuration, but is being replaced by wireless nodes using nRFL01+ ghz radio transceivers, and TMRH20's mesh network library (http://tmrh20.github.io/RF24Mesh/)

**THIS IS AS MUCH A LEARNING PROJECT FOR ME AS IT IS A DESIRE TO CREATE A SYSTEM THAT EFFECTIVELY IMPROVES FOOD SAFETY**
So, if you'd like to talk, share, write, offer feedback, suggestions, or improvements, please feel free to do so 
alan.bickel@gmail.com


For libraries required other than my own, see the includes in the .ino file.

Also, be sure to check out the IT3 Java repo! right now it is shell for uploading system data to SD card(loading item names, employee names) which transfers information to control module, but possibly will be fleshed out to store local copies of data logs...
