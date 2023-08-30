#include "driver_ax12.h"

// MOTOR ID MACROS, 
// L/R means left/right, F/B means forward/backward.
#define MOTOR_RB 0x01
#define MOTOR_RF 0x02
#define MOTOR_LF 0x03
#define MOTOR_LB 0x04

int main(int argc, char *argv[])
{
	int i;

	ax12_init();

//	// Open a test fd.
//	int test_fd = open("./output", O_RDWR | O_CREAT);
//	if (-1 == test_fd)
//	{
//		perror("Fail to open test_fd");
//		exit(EXIT_FAILURE);
//	}
//	write_packet(test_fd, 0x01, 0x05, I_WRITE, 0x1E, 0x96, 0x00);


	// Write instruction to ax12.
	ax_move_speed(MOTOR_LF, 134);	
	ax_move_speed(MOTOR_LB, 134);
	ax_move_speed(MOTOR_RF, 134);	
	ax_move_speed(MOTOR_RB, 134);	

	// Initial Position
	ax_turn2angle(MOTOR_LF, 511);
	ax_turn2angle(MOTOR_LB, 511);
	ax_turn2angle(MOTOR_RF, 511);
	ax_turn2angle(MOTOR_RB, 511);
	getchar();

	// LF 60 degree, LB 60 degree
	ax_turn2angle(MOTOR_LB, 511-205);
	ax_turn2angle(MOTOR_LF, 511-205);
	getchar();

	// LF 60 degree, LB -60 degree
	ax_turn2angle(MOTOR_LB, 511+205);
	ax_turn2angle(MOTOR_LF, 511-205);
	getchar();

	// LF 30 degree, LB -150 degree
	ax_turn2angle(MOTOR_LB, 511+512);
	ax_turn2angle(MOTOR_LF, 511-103);
	getchar();

	// LF 75 degree, LB 0 degree
	ax_turn2angle(MOTOR_LB, 511+0);
	ax_turn2angle(MOTOR_LF, 511-256);
	getchar();

	// LF 0 degree, LB 0 degree
	ax_turn2angle(MOTOR_LB, 511);
	ax_turn2angle(MOTOR_LF, 511);
	getchar();

	// **************************
	// RF 60 degree, RB 60 degree
	ax_turn2angle(MOTOR_RB, 511+205);
	ax_turn2angle(MOTOR_RF, 511+205);
	getchar();

	// RF 60 degree, RB -60 degree
	ax_turn2angle(MOTOR_RB, 511-205);
	ax_turn2angle(MOTOR_RF, 511+205);
	getchar();

	// RF 30 degree, RB -150 degree
	ax_turn2angle(MOTOR_RB, 511-511);
	ax_turn2angle(MOTOR_RF, 511+103);
	getchar();

	// RF 75 degree, RB 0 degree
	ax_turn2angle(MOTOR_RB, 511+0);
	ax_turn2angle(MOTOR_RF, 511+256);
	getchar();

	// RF 0 degree, RB 0 degree
	ax_turn2angle(MOTOR_RB, 511);
	ax_turn2angle(MOTOR_RF, 511);
	getchar();


	// Close the file.
	ax12_deinit();

	return 0;
}
