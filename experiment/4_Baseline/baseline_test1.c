#include "driver_mpu9250_basic.h"
#include "driver_ax12.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define I2C_FLAG_RIGHT 0
#define I2C_FLAG_LEFT 1

// MOTOR ID MACROS, 
// L/R means left/right, F/B means forward/backward.
#define MOTOR_LB 0x01
#define MOTOR_LF 0x02
#define MOTOR_RF 0x03
#define MOTOR_RB 0x04

int main()
{
	int i;

	ax12_init();
	
	// (134 / 1023) * 114 ~= 15 RPM 
	ax_move_speed(MOTOR_RF, 134);
	ax_move_speed(MOTOR_RB, 134);
	
	ax_turn2angle(MOTOR_RF, 512);
	ax_turn2angle(MOTOR_RB, 512);

	getchar();
	printf("test begin\n");
	
	for (i = 0; i < 8; ++i)
	{
		// RF 90 degree, RB 60 degree	
		// ax_turn2angle(MOTOR_RF, 200);
		// ax_turn2angle(MOTOR_RB, 511-205);
		
		// RF 120 degree, RB 150 degree
		ax_turn2angle(MOTOR_RF, 101);
		ax_turn2angle(MOTOR_RB, 511+512);
		getchar();

		ax_turn2angle(MOTOR_RF, 512);
		ax_turn2angle(MOTOR_RB, 512);
		getchar();
		
		printf("This is the %d turn\n", i+1);
	}

	return 0;

} // End of main.

