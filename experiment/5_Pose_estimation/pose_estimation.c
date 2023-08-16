#include "driver_mpu9250_basic.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define I2C_FLAG_RIGHT 0
#define I2C_FLAG_LEFT 1

// a = (The value from accel) * AFS_SEL (mg)
#define AFS_SEL_0 0.061
#define AFS_SEL_1 0.122
#define AFS_SEL_2 0.244
#define AFS_SEL_3 0.488

// w = (The value from gyroscope) * FS_SEL (mdps)
#define FS_SEL_0  7.633
#define FS_SEL_1  15.267
#define FS_SEL_2  30.488
#define FS_SEL_3  60.975

int main()
{
	int i = 0;

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
	
	// Read accelerometer and gyroscope value from the right mpu9250..
	if (0 != mpu9250_basic_read(I2C_FLAG_RIGHT, accel, gyro, magn)) 
	{
		(void)mpu9250_basic_deinit();
		return -1;
	}

	// Deinit and finish.
	(void)mpu9250_basic_deinit();

	return 0;

} // End of main.

