#include "driver_mpu9250_basic.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <sys/time.h>

#define I2C_FLAG_LEFT 0
#define I2C_FLAG_RIGHT 1

#define OFFSET_R_ACCEX -0.0081 
#define OFFSET_R_ACCEY -0.0287
#define OFFSET_R_ACCEZ 0.0335
#define OFFSET_R_GYROX 1.2870
#define OFFSET_R_GYROY 0.4252
#define OFFSET_R_GYROZ -0.5238

#define LOCAL_GRAVITY 9.8118

int main()
{
	int i = 0;
	
	float q_past[4] = {0}, q_new[4] = {0};
	float norm_q = 0;
	float R_nb[3][3] = {0};	

	uint8_t res;
	float accel[3], gyro[3], magn[3];
	
	// To calculate the zero drift.
	float temp;
	float aver[6] = {0};

	// To calculate the orientation and the position
	float a_nn[3] = {0};
	float w_n[3] = {0};
	float v_past[3] = {0}, v_new[3] = {0};
	float position[3] = {0};
	float angle[3] = {0};

	// To calculate the error.
	float norm_a = 0;
	float gravity_accel[3] = {0};
	float gravity_gyro[3] = {0};
	float error[3] = {0};
	float integ_error[3] = {0};
	float rect_gyro[3] = {3};
	float q_rect_new[4] = {0};

	struct timeval tv_s, tv_e;
	float interval = 0;
	float output_time = 0;

	int t = 0;
	int times = 2000;

	// Open a csv file to store the data.
	FILE *csv_fp = fopen("./data/filter.csv","w+");
	if (NULL == csv_fp)
	{
		perror("Fail to fopen csv_fp");
		exit(EXIT_FAILURE);
	}

	// Determinate the format of this csv file.
	fprintf(csv_fp, "time,esti_x,esti_y,esti_z,esti_a,esti_b,esti_c\n");

	mpu9250_address_t addr;

	// Initialize MPU9250.
	addr = MPU9250_ADDRESS_AD0_LOW;
	res = mpu9250_basic_init(MPU9250_INTERFACE_IIC, addr);
	if (res != 0)
	{
		perror("Fail to init MPU9250 (MPU9250_basic_init)");
		return -1;
	}

	for (i = 0; i < 500; ++i)
	{
		mpu9250_basic_read(I2C_FLAG_LEFT, accel, gyro, magn);
		aver[0] += accel[0];
		aver[1] += accel[1];
		aver[2] += accel[2];
		aver[3] += gyro[0];
		aver[4] += gyro[1];
		aver[5] += gyro[2];
		mpu9250_interface_delay_ms(10);		
	}

	for(i = 0; i < 6; ++i)
	{
		aver[i] = aver[i] / 500;
	}

	// Initialize unit quarternion 
	q_past[0] = 1;
	// Initialize the start time
	gettimeofday(&tv_s, NULL);	
	output_time = 0;
	for (t = 0; t < times; ++t)
	{
		// Read accelerometer and gyroscope value from the right mpu9250..
		if (0 != mpu9250_basic_read(I2C_FLAG_LEFT, accel, gyro, magn)) 
		{
			(void)mpu9250_basic_deinit();
			return -1;
		}

		gettimeofday(&tv_e, NULL);
		interval = ((tv_e.tv_sec - tv_s.tv_sec) * 1000) + 
			((tv_e.tv_usec - tv_s.tv_usec) / 1000);
		interval /= 1000;
		output_time += interval;
		gettimeofday(&tv_s, NULL);	
		
		// Rectify the value read from mpu9250
		accel[0] = (accel[0] + (1 + aver[0])) * LOCAL_GRAVITY;
		accel[1] = (accel[1] - aver[1]) * LOCAL_GRAVITY;
		accel[2] = (accel[2] - aver[2]) * LOCAL_GRAVITY;
		gyro[0] = (gyro[0] - aver[3]) * 0.0174;
		gyro[1] = (gyro[1] - aver[4]) * 0.0174;
		gyro[2] = (gyro[2] - aver[5]) * 0.0174;

		// Mitigate the offset.
//		accel[0] = (accel[0] + OFFSET_R_ACCEX) * LOCAL_GRAVITY;
//		accel[1] = (accel[1] + OFFSET_R_ACCEY) * LOCAL_GRAVITY;
//		accel[2] = (accel[2] + OFFSET_R_ACCEZ) * LOCAL_GRAVITY;
//		gyro[0] = (gyro[0] + OFFSET_R_GYROX) * 0.0174;
//		gyro[1] = (gyro[1] + OFFSET_R_GYROY) * 0.0174;
//		gyro[2] = (gyro[2] + OFFSET_R_GYROZ) * 0.0174;

		// Convert IMU frame to body frame.
		temp = accel[0];
		accel[0] = accel[2];
		accel[2] = -1 * temp;
		temp = gyro[0];
		gyro[0] = gyro[2];
		gyro[2] = -1 * temp;
	
		// Update q_new and q_past
		q_new[0] = q_past[0] - 0.5 * interval * (gyro[0] * q_past[1] + gyro[1] * q_past[2] + gyro[2] * q_past[3]);
		q_new[1] = q_past[1] + 0.5 * interval * (gyro[0] * q_past[0] - gyro[1] * q_past[3] + gyro[2] * q_past[2]);
		q_new[2] = q_past[2] + 0.5 * interval * (gyro[0] * q_past[3] + gyro[1] * q_past[0] - gyro[2] * q_past[1]);
		q_new[3] = q_past[3] + 0.5 * interval * (-1*gyro[2] * q_past[2] + gyro[1] * q_past[1] + gyro[2] * q_past[0]);

		// Normalize the q_new
		norm_q = 1 / sqrt(pow(q_new[0],2) + pow(q_new[1],2) + pow(q_new[2],2)+pow(q_new[3],2));
		for (i = 0; i < 4; ++i)
		{
			q_new[i] = q_new[i] * norm_q;
		}

		// Calculate the gravity vector from accel. (under body frame)
		norm_a = 1 / sqrt(pow(accel[0],2) + pow(accel[1],2) + pow(accel[2],2));
		gravity_accel[0] = accel[0] * norm_a;
		gravity_accel[1] = accel[1] * norm_a;
		gravity_accel[2] = accel[2] * norm_a;

		// Calculate the gravity vector from gyro. (under body frame)
		gravity_gyro[0] = 2 * (q_new[1] * q_new[3] - q_new[0] * q_new[2]);
		gravity_gyro[1] = 2 * (q_new[3] * q_new[2] + q_new[0] * q_new[1]);
		gravity_gyro[2] = pow(q_new[0], 2) - pow(q_new[1], 2) - pow(q_new[2], 2) + pow(q_new[3], 2);	

		// Calculate the error between gravity_accel and gravity_gyro
		error[0] = gravity_accel[1] * gravity_gyro[2] - gravity_accel[2] * gravity_gyro[1];
		error[1] = gravity_accel[2] * gravity_gyro[0] - gravity_accel[0] * gravity_gyro[2];
		error[2] = gravity_accel[0] * gravity_gyro[1] - gravity_accel[1] * gravity_gyro[0];
		printf("error: %f, %f, %f\n", error[0], error[1], error[2]);

		// // Integrated the error.
		// integ_error[0] += error[0] * 0.001 * interval;
		// integ_error[1] += error[1] * 0.001 * interval;
		// integ_error[2] += error[2] * 0.001 * interval;

		// Rectify the gyro's value.
		rect_gyro[0] = gyro[0] + 0.8 * error[0] + integ_error[0]; 
		rect_gyro[1] = gyro[1] + 0.8 * error[1] + integ_error[1]; 
		rect_gyro[2] = gyro[2] + 0.8 * error[2] + integ_error[2]; 

		
		// Update q_new and q_past
		q_rect_new[0] = q_past[0] - 0.5 * interval * (rect_gyro[0] * q_past[1] + rect_gyro[1] * q_past[2] + rect_gyro[2] * q_past[3]);
		q_rect_new[1] = q_past[1] + 0.5 * interval * (rect_gyro[0] * q_past[0] - rect_gyro[1] * q_past[3] + rect_gyro[2] * q_past[2]);
		q_rect_new[2] = q_past[2] + 0.5 * interval * (rect_gyro[0] * q_past[3] + rect_gyro[1] * q_past[0] - rect_gyro[2] * q_past[1]);
		q_rect_new[3] = q_past[3] + 0.5 * interval * (-1*rect_gyro[2] * q_past[2] + rect_gyro[1] * q_past[1] + rect_gyro[2] * q_past[0]);

		// Normalize the q_new
		norm_q = 1 / sqrt(pow(q_rect_new[0],2) + pow(q_rect_new[1],2) + pow(q_rect_new[2],2) + pow(q_rect_new[3],2));
		for (i = 0; i < 4; ++i)
		{
			q_rect_new[i] = q_rect_new[i] * norm_q;
		}

		// Update the rotation matrix
		R_nb[0][0] = pow(q_rect_new[0], 2) + pow(q_rect_new[1], 2) - pow(q_rect_new[2], 2) - pow(q_rect_new[3], 2);
		R_nb[0][1] = 2 * (q_rect_new[1] * q_rect_new[2] - q_rect_new[0] * q_rect_new[3]);
		R_nb[0][2] = 2 * (q_rect_new[1] * q_rect_new[3] + q_rect_new[0] * q_rect_new[2]);
		R_nb[1][0] = 2 * (q_rect_new[1] * q_rect_new[2] + q_rect_new[0] * q_rect_new[3]);
		R_nb[1][1] = pow(q_rect_new[0], 2) - pow(q_rect_new[1], 2) + pow(q_rect_new[2], 2) - pow(q_rect_new[3], 2);
		R_nb[1][2] = 2 * (q_rect_new[3] * q_rect_new[2] - q_rect_new[0] * q_rect_new[1]);
		R_nb[2][0] = 2 * (q_rect_new[1] * q_rect_new[3] - q_rect_new[0] * q_rect_new[2]);
		R_nb[2][1] = 2 * (q_rect_new[3] * q_rect_new[2] + q_rect_new[0] * q_rect_new[1]);
		R_nb[2][2] = pow(q_rect_new[0], 2) - pow(q_rect_new[1], 2) - pow(q_rect_new[2], 2) + pow(q_rect_new[3], 2);
		
		// int j = 0;
		// printf("R_nb = \n");
		// for(i = 0; i < 3; ++i)
		// {
		//	for(j = 0; j < 3; ++j)
		//	{
		//		printf("%f ", R_nb[i][j]);
	 	//	}
		// 	printf("\n");
		// }

		// Update the acceleration with respect to navigation frame
		a_nn[0] = R_nb[0][0] * accel[0] + R_nb[0][1] * accel[1] + R_nb[0][2] * accel[2];
		a_nn[1] = R_nb[1][0] * accel[0] + R_nb[1][1] * accel[1] + R_nb[1][2] * accel[2];
		a_nn[2] = R_nb[2][0] * accel[0] + R_nb[2][1] * accel[1] + R_nb[2][2] * accel[2] - LOCAL_GRAVITY;

		// Update the speed and the position
		for (i = 0; i < 3; ++i)
		{
			v_new[i] = v_past[i] + a_nn[i] * interval;
			// printf("accel[%d] = %f\t", i, accel[i]);
			// printf("a_nn[%d] = %f\n", i, a_nn[i]);
			position[i] += 0.5 * interval * (v_past[i] + v_new[i]);
			v_past[i] = v_new[i];
		}

		// Update the angle		
		w_n[0] = R_nb[0][0] * rect_gyro[0] + R_nb[0][1] * rect_gyro[1] + R_nb[0][2] * rect_gyro[2];
		w_n[1] = R_nb[1][0] * rect_gyro[0] + R_nb[1][1] * rect_gyro[1] + R_nb[1][2] * rect_gyro[2];
		w_n[2] = R_nb[2][0] * rect_gyro[0] + R_nb[2][1] * rect_gyro[1] + R_nb[2][2] * rect_gyro[2];
		
		angle[0] = angle[0] + w_n[0] * interval;
		angle[1] = angle[1] + w_n[1] * interval;
		angle[2] = angle[2] + w_n[2] * interval;

		// printf("w_n[0] = %f\tgyro[0] = %f\n", w_n[0], gyro[0]);

		// Update the q_past		
		for (i = 0; i < 4; ++i)
		{
			q_past[i] = q_rect_new[i];
		}

		// Ouput the postition
		printf("x = %fdegree\ty = %fdegree\tz=%fdegree\n", 
				angle[0]/3.1415*180, angle[1]/3.1415*180, angle[2]/3.1415*180);
		printf("x = %fm\ty = %fm\tz = %fm\n", position[0], position[1], position[2]);
		
		fprintf(csv_fp, "%f,%f,%f,%f,%f,%f,%f\n", output_time,
				position[0], position[1], position[2],
				angle[0]/3.1415*180, angle[1]/3.1415*180, angle[2]/3.1415*180);	

		mpu9250_interface_delay_ms(10);

	}	
	
	// Deinit and finish.
	(void)mpu9250_basic_deinit();

	return 0;

} // End of main.

