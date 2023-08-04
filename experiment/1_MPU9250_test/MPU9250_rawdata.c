#include "driver_mpu9250_basic.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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
		if (0 != mpu9250_basic_read(g, dps, ut)) 
		{
			(void)mpu9250_basic_deinit();
			return -1;
		}

		if (0 != mpu9250_basic_read_temperature(&degrees))
		{
			(void)mpu9250_basic_deinit();
			return -1;
		}
	
		// Output the value.
//		printf("mpu9250: acce x is %0.2fg.\n", g[0]);
//		printf("mpu9250: acce y is %0.2fg.\n", g[1]);
//		printf("mpu9250: acce z is %0.2fg.\n", g[2]);
//		printf("mpu9250: gyro x is %0.2fdps.\n", dps[0]);
//		printf("mpu9250: gyro y is %0.2fdps.\n", dps[1]);
//		printf("mpu9250: gyro z is %0.2fdps.\n", dps[2]);
//		printf("mpu9250: temperature is %0.2fC.\n", degrees);
		
		printf("%.4f  %.4f  %.4f\n", g[0], g[1], g[2]);
		
		mpu9250_interface_delay_ms(10);
	}

	// Deinit and finish.
	(void)mpu9250_basic_deinit();

	return 0;

} // End of main.



