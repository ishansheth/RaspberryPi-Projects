# RaspberryPi-Projects

### To login to my raspberry pi 

- Use ssh to login

- command: ssh pi@ishanpi (hostname -> ishanpi. IP->169.254.133.250 (try))
- password: qweasd

This repository contains a sample program in C++ of interfacing raspberry pi with GY80 chip, switches, LEDs and USB Webcam

To connect to new wifi station:

Add below lines to _sudo nano /etc/wpa_supplicant/wpa_supplicant.conf_

network={

    ssid="You SSID Name"
    
    psk="Your WiFI Password"
    
    key_mgmt=WPA-PSK
    
}

