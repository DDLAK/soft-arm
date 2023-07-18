# soft-arm
The project is for UOB MSc bio-robotics dissertation, 'Bio-inspired telepresence avatars for embodied communication'.

## Directory Structure

<font color=#9400D3>\mechanism</font> (includes the mechanical structure design of the robot)

​			<font color=#9370DB>*.stl files</font>: For 3D printing purpose.

​			<font color=#9370DB>*.dxf files</font>: For laser cutting purpose.

​			<font color=#9370DB>*.f3d / *.f3z files</font>: For viewing and editing purpose. Can be opened by 'Fusion 360'.

<font color=#9400D3>\hardware</font> (includes the basic description and driver of all used hardware)

​			<font color=#9370DB>\raspberrry pi zero W</font>: The quick start guides are in it.

​			<font color=#9370DB>\IMU_Selection</font>: Includes the specifications and the comparing results of the IMUs. Both CJMCU-BNO055 and MPU9250 meet the needs with decent performance. However, raspberry pi zero w doesn't have the <font color=#996B1F>I2C clock stretching feature</font> which is essential to BNO055. In addition, BNO055 is more expensive than MPU9250. Thus, I chose MPU9250 to build this project.

​			<font color=#9370DB> \MPU9250</font>: Contains the source code (Python & C) to drive the MPU9250. 

​			<font color=#9370DB>\AX12_servo</font>: Contains the datasheet of AX12_servo, helpful websites of AX12 Dynamixel hat of raspberry pi zero and documents of AX12 python library.



<font color=#9400D3>\experiment</font> (includes the explanations, source codes and results of each experiment)



## Mechanism Description





## Hardware Description



## Software Set-up Steps

### Python Programming Environment

This project uses CircuitPython, which mainly developed by Adafruit and is a fork from MicroPython, to control the GPIOs and other hardwares on the raspberry Pi. To install CircuitPython, [this website](https://learn.adafruit.com/circuitpython-on-raspberrypi-linux/installing-circuitpython-on-raspberry-pi) can be referred. For convenience, the streamlined steps are shown here.

```
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install python3-pip
sudo pip3 install --upgrade setuptools	
```

```
cd
sudo pip3 install --upgrade adafruit-python-shell
wget https://raw.githubusercontent.com/adafruit/Raspberry-Pi-Installer-Scripts/master/raspi-blinka.py
sudo python3 raspi-binka.py
```

### C Programming Streamline

Ongoing...





## Status

v0.0 Ongoings
