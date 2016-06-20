# TempTrack
Integrated System for Commercial Foodservice Temperature Analysis and Logging

This is version 1 of Arduino-API based temperature monitoring and analysis system, developed with the intent to ultimately be used in the Food Service industry to assist with USDA regulatory compliance, and as a means to improve food safety through proactive monitoring of potentially hazardous foods while they cool. 

Initially written in C++ via Arduino API, the hardware consists of DallasTemperature's DS18B20 temperature sensors, ATmega328p MCU for satellite probes (still under construction), and most likely ATmega2560 to drive central control module.  

This version of the code was designed with a wired probe configuration, but is being replaced by wireless nodes using nRFL01+ ghz radio transceivers, and TMRH20's mesh network library (http://tmrh20.github.io/RF24Mesh/)

**THIS IS AS MUCH A LEARNING PROJECT FOR ME AS IT IS A DESIRE TO CREATE A SYSTEM THAT EFFECTIVELY IMPROVES FOOD SAFETY**
So, if you'd like to talk, share, write, offer feedback, suggestions, or improvements, please feel free to do so 
alan.bickel@gmail.com


I've uploaded all libraries included in this project, a number of which are authored by others...documentation of all original authorship is maintained in each of the libraries... 

a quick run-down of libraries I've written:
-----------------------------------------
DataLog
DataWrapper
Draw
Error
GraphicConstants
InputProcessor_v3  
Message
Prove_v3
SystemConstants
TempCalc_v2
Util
