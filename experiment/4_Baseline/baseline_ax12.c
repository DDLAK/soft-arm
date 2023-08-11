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
	ax_turn2angle(MOTOR_RF, 511);
	ax_turn2angle(MOTOR_LF, 511);

	ax_move_speed(MOTOR_LB, 200);
	ax_move_speed(MOTOR_RB, 200);

	for (i = 0; i < 5; ++i)
	{
		ax_turn2angle(MOTOR_LB, 255);
		ax_turn2angle(MOTOR_RB, 768);
		bcm2835_delay(800);
		ax_turn2angle(MOTOR_LB, 0);
		ax_turn2angle(MOTOR_RB, 1023);
		bcm2835_delay(800);
	}
	ax_turn2angle(MOTOR_LB, 511);
	ax_turn2angle(MOTOR_RB, 511);
	//ax_move_speed(MOTOR_LB, 255);
	//ax_turn2angle(MOTOR_LB, 688);
	//bcm2835_delay(800);
	//ax_turn2angle(MOTOR_LB, 1023);

	// Close the file.
	ax12_deinit();

	return 0;
}
