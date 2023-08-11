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
	int times = 500;

	uint8_t res;
	float g[3], dps[3], ut[3];
	float degrees;

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
		if (0 != mpu9250_basic_read(I2C_FLAG_RIGHT, g, dps, ut)) 
		{
			(void)mpu9250_basic_deinit();
			return -1;
		}

		if (0 != mpu9250_basic_read_temperature(I2C_FLAG_RIGHT, &degrees))
		{
			(void)mpu9250_basic_deinit();
			return -1;
		}
	
		// Output the value
		printf("right: %.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\n", g[0], g[1], g[2], dps[0], dps[1], dps[2], degrees);
		
		if (0 != mpu9250_basic_read(I2C_FLAG_LEFT, g, dps, ut)) 
		{
			(void)mpu9250_basic_deinit();
			return -1;
		}

		if (0 != mpu9250_basic_read_temperature(I2C_FLAG_LEFT, &degrees))
		{
			(void)mpu9250_basic_deinit();
			return -1;
		}
	
		// Output the value
		printf("left: %.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\n", g[0], g[1], g[2], dps[0], dps[1], dps[2], degrees);
		
		mpu9250_interface_delay_ms(10);
		
	}

	// Deinit and finish.
	(void)mpu9250_basic_deinit();

	return 0;

} // End of main.

