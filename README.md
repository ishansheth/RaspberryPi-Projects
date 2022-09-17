# RaspberryPi-Projects

### To login to my raspberry pi 

- Use ssh to login

- command: ssh pi@ishanpi (hostname -> ishanpi. IP->169.254.133.250 (try))
- password: qweasd

This repository contains a sample program in C++ of interfacing raspberry pi with GY80 chip, switches, LEDs and USB Webcam

To connect to new wifi station:

Add below lines to sudo nano /etc/wpa_supplicant/wpa_supplicant.conf

network={
    ssid="You SSID Name"
    psk="Your WiFI Password"
    key_mgmt=WPA-PSK
}

