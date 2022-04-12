# IoT Water Level Monitor and Control
by Daniel Promise Olayimika  

A water level monitoring and control system can be used to monitor the level of water in an overhead tank, automate the pump switching so water doesn't go below a pre-determined level and doesn't get overfilled, and also can be used to manually put on/off the pump when necessary.  
This results in the efficiency of power, detecting leakage since water level drastically reduces, smarter homes,...  
This project is easy to implement, light in weight, compact in size, and can easily be integrated with the WiFi network of a home or organization, also it does the automation of the pump and water level even **without connectivity to a network.**

## Components used:
* ESP 32
* 5V relay
* 5V water pump
* _wires/conductor of different length_ (7)
* Resistors (220 Ω x5)
* 5V power supply
* rubber pipe
* 2 water storage (buckets)

ESP 32 is the main driver of this project, the sensor used to detect the water level are simple wires/conductors (Aluminium foils glued to the walls of the tank were used in the development of the project) of different lengths (It is advised that copper should not be used as it is easily oxidized), A relay is used to activate the pump since the microcontroller can't supply enough current to drive the pump, the resistors are used to pull down the ESP 32 Analog pins connected to the sensing wires.  

Accessing the controls and viewing the level of the water tank is done using a browser, once the system has connected to an accessible network, it IP address is entered into the browser and a [web page](data/index.html) is loaded on the device browser, the water level is shown by a responsive [GIF image](data/) which is easy to interpret, also there is a checkbox above the image which acts as the switch.

## Circuit diagram
![schematics](resources/Fritzing%20schematics_bb.jpg)

## Libraries used:
* [WiFi](https://github.com/arduino-libraries/WiFi)
* [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)
* [ESP 32 SPIFFS](https://github.com/espressif/arduino-esp32/tree/master/libraries/SPIFFS)
