#include "driver_mpu9250_basic.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define I2C_FLAG_RIGHT 0
#define I2C_FLAG_LEFT 1

int main()
{
	int i = 0;
	int times = 6000;

	uint8_t res;
	float accel[3], gyro[3], magn[3];

	mpu9250_address_t addr;

	// Initialize MPU9250.
	addr = MPU9250_ADDRESS_AD0_LOW;
	res = mpu9250_basic_init(MPU9250_INTERFACE_IIC, addr);
	if (res != 0)
	{
		perror("Fail to init MPU9250 (MPU9250_basic_init)");
		return -1;
	}
	
	// Read accelerometer and gyroscope.
	for (i = 0; i < times; ++i)
	{
		if (0 != mpu9250_basic_read(I2C_FLAG_RIGHT, accel, gyro, magn)) 
		{
			(void)mpu9250_basic_deinit();
			return -1;
		}
	
		if (0 != mpu9250_basic_read(I2C_FLAG_LEFT, accel, gyro, magn)) 
		{
			(void)mpu9250_basic_deinit();
			return -1;
		}
		
		mpu9250_interface_delay_ms(10);		
	}

	// Deinit and finish.
	fclose(csv_fp);
	(void)mpu9250_basic_deinit();

	return 0;

} // End of main.

