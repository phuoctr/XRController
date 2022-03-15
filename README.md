Tutorial: Thumb-mounted controller

This thumb-mounted controller is an input device for using in extended reality (XR) environments, it operates by sensing user’s thumb gestures in which thumb contact and movements are processed using the output data from 2 or more force sensitive resistors and a gyroscope, recognised gestures are send to a XR device via Bluetooth Low Energy.

Materials

Adafruit Feather nRF52840 sense: this is a bluetooth low energy (BLE) development board which comes with built-in accelerator, gyroscope and magnetometer. Another equivalent board is Arduino Nano 33 BLE. The Adafruit feather sense can be purchase here.
2*Force sensitive resistors: resistors with minimum sensitve of 30gr is recommend for better gesture sensing. The force sensistive resistors can be purchase here.
2* 10,000Ohms resistors. The resistors can be bought easily from the internet.
3.3V Li-Po battery. For this controller I used this battery.
A housing for the board: this housing is based on the board holder from Adafruit and a thumb cover to mount the controller on. The stl file for this housing can be found here.
A housing for the battery: a wrist band for mounting the battery. The stl file for this band can be found here.

Force Sensitive Resistors

Force sensitive resistors play an important role on this controller for touch recognition including swiping and pressing. To use the force sensors, we need to make a voltage divider circuit as the figure below, a 3V power is connect to one pin on each force sensor, and on the other pin the analog input and ground is connected.
<img width="235" alt="image" src="https://user-images.githubusercontent.com/46408299/158438894-bc3cbff2-3adb-4711-93ce-b42906d7a4ae.png">


Arduino program

To program the Adafruit Feather Sense board, you need to use Arduino with Adafruit nRF52 library imported, the process can be done following this tutorial from Adafruit. Along with the nRF52 library, the Adafruit_LSM6DS33.h library need to be imported to Arduino to get output data from the gyroscope. After the Arduino IDE is set up, connect the board to your computer via microUSB cable, download this Arduino sketch  and run it. 
Housing

The housing comprises of two parts, one for holding the Feather Sense board and the other for mounting user’s thumb, under the thumb mount, the force sensitive parts of two force sensitive resistors are stiched as illustrated in the figure below:
![image](https://user-images.githubusercontent.com/46408299/158438943-571378e9-1d8e-448f-9e74-16d3c323688a.png)


The controller can be powered either by connecting to a computer or using an external battery, a battery wristband is designed for mounting the battery.



Gestures

Swiping left/right on a touch zone
Swiping up/down
Quick press
Long press
