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
#define MOTOR_RB 0x01
#define MOTOR_RF 0x02
#define MOTOR_LF 0x03
#define MOTOR_LB 0x04

int main()
{
	int i;

	ax12_init();
	
	// (134 / 1023) * 114 ~= 15 RPM 
	ax_move_speed(MOTOR_LF, 134);
	ax_move_speed(MOTOR_LB, 134);
	
	ax_turn2angle(MOTOR_LF, 512);
	ax_turn2angle(MOTOR_LB, 512);

	getchar();
	printf("test begin\n");
	
	for (i = 0; i < 8; ++i)
	{
//		// paradigm 1: LF 75 degree, LB 0 degree
//		ax_turn2angle(MOTOR_LB, 511+0);
//		ax_turn2angle(MOTOR_LF, 511-256);
//		getchar();	
		
//		// paradigm 2: LF 60 degree, LB -60 degree
//		ax_turn2angle(MOTOR_LB, 511+205);
//		ax_turn2angle(MOTOR_LF, 511-205);
//		getchar();	
		
//		// paradigm 3: LF 60 degree, LB 60 degree
//		ax_turn2angle(MOTOR_LB, 511-205);
//		ax_turn2angle(MOTOR_LF, 511-205);
//		getchar();	

		// paradigm 4: LF 30 degree, LB -150 degree
		ax_turn2angle(MOTOR_LB, 511+512);
		ax_turn2angle(MOTOR_LF, 511-103);
		getchar();

		// paradigm 0: LF 0 degree, LB 0 degree
		ax_turn2angle(MOTOR_LB, 512);
		ax_turn2angle(MOTOR_LF, 512);
		getchar();
		
		printf("This is the %d turn\n", i+1);
	}

	return 0;

} // End of main.

