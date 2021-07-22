# AutoCalibrationRB
Proof of concept (PoC) of the auto-calibration process for RB guitar controllers

Before compiling, make sure to replace the VENDOR_ID & PRODUCT_ID in guitar_control.h to reflect your hardware.  
This code was created for the Mad Catz Fender Mustang Pro-Guitar for Nintendo Wii.

**Steps to compile :**
1) sudo apt install qt5-qmake qtmultimedia5-dev qt5-default libusb-1.0-0-dev
3) qmake ./Auto_calibration.pro
4) make
5) ./Auto_calibration

**About this Project :**
The main objective of this project is to understand the auto-calibration process for Rock Band guitar controllers with the goal of 
implementing the feature into other applications and enabling custom-guitar makers to create hardware that supports auto-calibration. A fully detailed 
report of the project can be found in the report folder of this repository.