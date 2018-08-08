# GY80_RaspberryPi
This repository contains a code to interface GY80 chip with raspberry pi over I2C bus. The GY80 chip has 5 sensors:
  3 axis Gyroscope
  3 axis Accelerometer
  3 axis Magnetometer
  Barometer + Thermometer
  
The code reads the data from gyroscope and accelerometer from I2C bus using wiringPi library and sends the data over the socket
The client code connectes to the listening socket and reads the data continuously 
  
