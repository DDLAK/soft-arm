#include "driver_mpu9250_basic.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define I2C_FLAG_LEFT 0
#define I2C_FLAG_RIGHT 1

int main()
{
	int i = 0;
	int times = 6000;

	uint8_t res;
	float accel[3], gyro[3], magn[3];

	mpu9250_address_t addr;

	// Open a csv file to store the data
	FILE *csv_fp = fopen("./data/test1_10.csv", "w+");
	if (NULL == csv_fp)
	{
		perror("Fail to fopen csv_fd");
		exit(EXIT_FAILURE);
	}	

	// Determinate the format of this csv file.
	fprintf(csv_fp, "l_accel_x,l_accel_y,l_accel_z,l_gyro_x,l_gyro_y,l_gyro_z,"
			"r_accel_x,r_accel_y,r_accel_z,r_gyro_x,r_gyro_y,r_gyro_z\n");

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
		if (0 != mpu9250_basic_read(I2C_FLAG_LEFT, accel, gyro, magn)) 
		{
			(void)mpu9250_basic_deinit();
			return -1;
		}
	
		// Output the value
		fprintf(csv_fp, "%f,%f,%f,%f,%f,%f,", 
				accel[0], accel[1], accel[2], gyro[0], gyro[1], gyro[2]);
		
		if (0 != mpu9250_basic_read(I2C_FLAG_RIGHT, accel, gyro, magn)) 
		{
			(void)mpu9250_basic_deinit();
			return -1;
		}

		// Output the value
		fprintf(csv_fp, "%f,%f,%f,%f,%f,%f\n", 
				accel[0], accel[1], accel[2], gyro[0], gyro[1], gyro[2]);
		
		mpu9250_interface_delay_ms(10);		
	}

	// Deinit and finish.
	fclose(csv_fp);
	(void)mpu9250_basic_deinit();

	return 0;

} // End of main.

