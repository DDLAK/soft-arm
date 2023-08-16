#include "driver_ax12.h"

// MOTOR ID MACROS, 
// L/R means left/right, F/B means forward/backward.
#define MOTOR_LB 0x01
#define MOTOR_LF 0x02
#define MOTOR_RF 0x03
#define MOTOR_RB 0x04

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
	ax_move_speed(MOTOR_RF, 134);	
	ax_move_speed(MOTOR_RB, 134);

	// Initial Position
	ax_turn2angle(MOTOR_RF, 511);
	ax_turn2angle(MOTOR_RB, 511);
	getchar();

	// RF 90 degree, RB -60 degree
	ax_turn2angle(MOTOR_RF, 511-311);
	ax_turn2angle(MOTOR_RB, 511-205);
	getchar();

	// RF 90 degree, RB 60 degree
	ax_turn2angle(MOTOR_RF, 511-311);
	ax_turn2angle(MOTOR_RB, 511+205);
	getchar();

	// RF 30 degree, RB 150 degree
	ax_turn2angle(MOTOR_RF, 511-103);
	ax_turn2angle(MOTOR_RB, 511+512);
	getchar();

	// RF 120 degree, RB 0 degree
	ax_turn2angle(MOTOR_RF, 511-410);
	ax_turn2angle(MOTOR_RB, 511+0);
	getchar();

	// RF 0 degree, RB 0 degree
	ax_turn2angle(MOTOR_RF, 511);
	ax_turn2angle(MOTOR_RB, 511);
	//getchar();

	// Close the file.
	ax12_deinit();

	return 0;
}
