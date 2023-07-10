# soft-arm
The project is for UOB MSc bio-robotics dissertation, 'Bio-inspired telepresence avatars for embodied communication'.

## Structure

<font color=#9400D3>\hardware</font> (includes the basic description and driver of all used hardware)

​			<font color=#9370DB>\raspberrry pi zero W</font>: The quick start guides are in it.

​			<font color=#9370DB>\IMU_Selection</font>: Includes the specifications and the comparing results of the IMUs. Both CJMCU-BNO055 and MPU9250 meet the needs with decent performance. However, raspberry pi zero w doesn't have the <font color=#996B1F>I2C clock stretching feature</font> which is essential to BNO055. In addition, BNO055 is more expensive than MPU9250. Thus, I chose MPU9250 to build this project.

​			<font color=#9370DB>\AX12_servo</font>: Contains the datasheet of AX12_servo, helpful websites of AX12 Dynamixel hat of raspberry pi zero and documents of AX12 python library.



<font color=#9400D3>\experiment</font> (includes the explanations, source codes and results of each experiment)

## Status

v0.0 Ongoing
