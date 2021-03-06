# Tutorial: Thumb-mounted controller

This thumb-mounted controller is an input device for using in extended reality (XR) environments, it operates by sensing user’s thumb gestures in which thumb contact and movements are processed using the output data from 2 or more force sensitive resistors and a gyroscope, recognised gestures are send to a XR device via Bluetooth Low Energy.

## Materials

- Adafruit Feather nRF52840 sense: this is a bluetooth low energy (BLE) development board which comes with built-in accelerator, gyroscope and magnetometer. Another equivalent board is Arduino Nano 33 BLE. The Adafruit Feather Sense can be found at [Adafruit website](https://www.adafruit.com/product/4516)

- 2*Force sensitive resistors: resistors with minimum sensitve of 30gr is recommend for better gesture sensing. The force sensistive resistors can be purchased for example on [Mouser](https://eu.mouser.com/ProductDetail/Ohmite/FSR05CE?qs=0lSvoLzn4L9hUdjZ8RLKtw==).

- 2* 10,000Ohms resistors. The resistors can be bought easily from the internet.
- 3.3V Li-Po battery. For this controller, a 500mAH battery is used but it can also be powered by the microUSB port but keep it in mind that because the power consumption is not very significant so some battery banks will only work for few second before it stop powering the controller.
- A housing for the board: this housing is based on the board holder from Adafruit and a thumb cover to mount the controller on. The stl file for this housing can be found in this repo. This housing is modified from the [Adafruit Feather case](https://www.thingiverse.com/thing:1367270/files).
- A housing for the battery: a wrist band for mounting the battery. The stl file for this band can be found in this repo.

## Force Sensitive Resistors

Force sensitive resistors play an important role on this controller for touch recognition including swiping and pressing. To use the force sensors, we need to make a voltage divider circuit as the figure below, a 3V power is connect to one pin on each force sensor, and on the other pin the analog input and ground is connected.

<img width="694" alt="image" src="https://user-images.githubusercontent.com/46408299/158440514-24e7ef20-33b0-4343-98be-111f3e64b7fb.png">

## Arduino program

To program the Adafruit Feather Sense board, you need to use Arduino with Adafruit nRF52 library imported, the process can be done following this tutorial from Adafruit. Along with the nRF52 library, the Adafruit_LSM6DS33.h library need to be imported to Arduino to get output data from the gyroscope. After the Arduino IDE is set up, connect the board to your computer via microUSB cable, download this Arduino sketch  and run it. 
Housing

The housing comprises of two parts, one for holding the Feather Sense board and the other for mounting user’s thumb, under the thumb mount, the force sensitive parts of two force sensitive resistors are stiched as illustrated in the figure below:
<img width="681" alt="image" src="https://user-images.githubusercontent.com/46408299/158440403-1672186f-5fa4-4843-a0e4-049963879c19.png">


The controller can be powered either by connecting to a computer or using an external battery, a battery wristband is designed for mounting the battery.


## Gestures
At the moment, this controller supports following gestures, in future version of the hardware and software I hope to include more gestures
- Swiping left/right on a touch zone
- Swiping up/down
- Quick press
- Long press

A working demo for this controller can be found [here](https://phuoctrinh.com/ar-input-prototype), in this demo an AR Unity application for iPad is developed which communicate with the XR controller over BLE.
