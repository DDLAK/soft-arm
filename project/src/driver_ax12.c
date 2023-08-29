#include "driver_ax12.h"

#define PIN_CTRL RPI_GPIO_P1_12

#define SERIAL_PATH "/dev/serial0"
#define BAUDRATE B1000000

// Two START_FLAGs indicate an incoming packet
#define START_FLAG 0xFF

// The instructions value.
#define I_PING 0x01
#define I_READ 0x02
#define I_WRITE 0x03
#define I_REGW 0x04
#define I_ACTION 0x05
#define I_RESET 0x06
#define I_SYNCW 0x83

// The error bit.
#define E_INSTRUCTION 6
#define E_OVERLOAD 5
#define E_CHECKSUM 4
#define E_RANGE 3
#define E_OVER_HEATING 2
#define E_ANGLE_LIMIT 1
#define E_INPUT_VOLTAGE 0

// The meaning of each position shown in the packet.
#define PCK_START1 0
#define PCK_START2 1
#define PCK_ID 2
#define PCK_LENGTH 3
#define PCK_INSTR_ERROR 4

// Control Table of the ax12.
#define CT_MODEL_NUMBER_L  	0x00
#define CT_MODEL_NUMBER_H  	0x01
#define CT_FIRMWARE_VERSION 	0x02
#define CT_ID 		 	0x03
#define CT_BAUD_RATE	 	0x04
#define CT_RETURN_DELAY		0x05
#define CT_STATUS_RETURN_LEVEL 	0x10
#define CT_GOAL_POSI_L		0x1E
#define CT_GOAL_POSI_H   	0x1F
#define CT_MOVE_SPEED_L		0x20
#define CT_MOVE_SPEED_H		0x21

// Typical value for baud rate.
#define BPS_1M 		0x01
#define BPS_500K 	0x03
#define BPS_400K	0x04
#define BPS_250K 	0x07
#define BPS_200K	0x09
#define BPS_115200	0x10
#define BPS_57600	0x22
#define BPS_19200	0x67
#define BPS_9600	0xCF

static int serial_fd;

int ax12_init(void)
{
	struct termios serial_options;

	// Initialize bcm2835 (raspberry pi zero w) chip.
	if (!bcm2835_init())
	{
		printf("Fail to init bcm2835");
		return -1;
	}
	bcm2835_gpio_fsel(PIN_CTRL, BCM2835_GPIO_FSEL_OUTP);

	// Open device serial0.
	serial_fd = open(SERIAL_PATH, O_RDWR | O_NOCTTY);
	if (-1 == serial_fd)
	{
		perror("Open");
		return -1;
	}

	if (0 == isatty(serial_fd))
	{
		printf("Wrong file descriptor: serial_fd is not an open termial device!");
		return -1;
	}

	// Set up the UART.
	if (-1 == tcgetattr(serial_fd, &serial_options))
	{
		perror("Get termial attributes (tcgetattr)");
		return -1;
	}
	
	// Only modified the attribute(s) we are interested in.
	// Default protocol of AX12: speed 1000000 bps, 8 bit, 1 stop, No parity.
	serial_options.c_iflag &= ~(INPCK | ISTRIP);
	serial_options.c_cflag |= CLOCAL | CREAD | CS8;
	serial_options.c_cflag &= ~(PARENB | CSTOPB);
	serial_options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	serial_options.c_cc[VMIN] = 1;
	serial_options.c_cc[VTIME] = 0;
	if (-1 == cfsetispeed(&serial_options, BAUDRATE))
	{
		perror("Fail to set input speed (cfsetispeed)");
		return -1;
	}
	
	if (-1 == cfsetospeed(&serial_options, BAUDRATE))
	{
		perror("Fail to set input speed (cfsetispeed)");
		return -1;
	}


	// Reflush and set the new structure.
	if (-1 == tcflush(serial_fd, TCIFLUSH))
	{
		perror("Fail to flush the serial_fd (tcflush)");
		return -1;
	}
	if (-1 == tcsetattr(serial_fd, TCSANOW, &serial_options))
	{
		perror("Set attributes in structure termios (tcsetattr)");
		return -1;
	}

	return 0;
}

int ax12_deinit(void)
{
	if (-1 == close(serial_fd))
	{
		perror("Close");
		return -1;
	}

	bcm2835_close();

	return 0;
}

int write_packet(uint8_t id, uint8_t length, uint8_t instruction, uint8_t *params) 
{
	// Set the PIN_CTRL in order to write.
	bcm2835_gpio_write(PIN_CTRL, HIGH);
	
	int i, sum;
	uint8_t *buf; 
	buf = (uint8_t *)malloc((length + 4) * sizeof(uint8_t));
	if (NULL == buf)
	{
		perror("Fail to malloc in 'write_packet'");
		return -1;
	}

	memset(buf, 0, (length + 4));
	buf[0] = START_FLAG;
	buf[1] = START_FLAG;
	buf[2] = id;
	buf[3] = length;
	buf[4] = instruction;
	for (i = 5, sum = 0; i < length + 3; ++i)
	{
		buf[i] = params[i-5];
		sum += params[i-5];
	}
	buf[length+3] = ~(id + length + instruction + sum);

	if (-1 == write(serial_fd,buf, (length + 4)))
	{
		perror("Fail to write in 'write_packet'");
		return -1;
	}

	free(buf);
	
	return 0;
}

int read_packet(uint8_t length)
{
	// Reset the PIN_CTRL in order to read.
	bcm2835_gpio_write(PIN_CTRL, LOW);
	
	uint8_t *buf = (uint8_t *)malloc(length * sizeof(uint8_t));
	if (NULL == buf)
	{
		perror("Fail to malloc in 'read_packet'");
		return -1;
	}

	if (-1 == read(serial_fd,buf, length))
	{
		perror("Fail to read in 'read_packet'");
		return -1;
	}

//	printf("read status packet: %x %x %x %x\n", buf[0], buf[1], buf[2], buf[3]);
//	printf("%x %x\n", buf[4], buf[5]);

	free(buf);

	return 0;
}

int set_id(uint8_t id, uint8_t new_id)
{
	uint8_t length = 0x02 + 2;
	uint8_t *params = (uint8_t *)malloc((length - 2) * sizeof(uint8_t));
	if (NULL == params)
	{
		perror("Fail to malloc in 'set_id'");
		return -1;
	}

	params[0] = CT_ID;
	params[1] = new_id;
	
	if (-1 == write_packet(id, length, I_WRITE, params))
	{
		printf("Fail to write packet in 'set_id'!\n");
		return -1;
	}

	free(params);
	
	return 0;
}

int set_status_return_level(uint8_t id, uint8_t level)
{
	if ((level != 0) && (level != 1) && (level != 2))
	{
		printf("Wrong level value in 'set_status_return_level'!\n");
		return -1;
	}

	uint8_t length = 0x02 + 2;
	uint8_t *params = (uint8_t *)malloc((length - 2) * sizeof(uint8_t));
	if (NULL == params)
	{
		perror("Fail to malloc in 'set_status_return_level'");
		return -1;
	}

	params[0] = CT_STATUS_RETURN_LEVEL;
	params[1] = level;
	
	if (-1 == write_packet(id, length, I_WRITE, params))
	{
		printf("Fail to write packet in 'set_status_return_level'!\n");
		return -1;
	}

	free(params);
	
	return 0;
}

int set_baud_rate(uint8_t id, uint8_t baud_rate)
{
	uint8_t length = 0x02 + 2;
	uint8_t *params = (uint8_t *)malloc((length - 2) * sizeof(uint8_t));
	if (NULL == params)
	{
		perror("Fail to malloc in 'set_baud_rate'");
		return -1;
	}

	params[0] = CT_BAUD_RATE;
	params[1] = baud_rate;
	
	if (-1 == write_packet(id, length, I_WRITE, params))
	{
		printf("Fail to write packet in 'set_baud_rate'!\n");
		return -1;
	}

	free(params);
	
	return 0;
}

// @brief: Write PING instruction with certain id.
int ax_ping(uint8_t id)
{
	uint8_t length = 0x00 + 2;
	uint8_t *params = NULL;

	if (-1 == write_packet(id, length, I_PING, params))
	{
		printf("Fail to write packet in 'ping'!\n");
		return -1;
	}

	return 0;
}

// @brief: Reset the Control Table.
int ax_reset(uint8_t id)
{
	uint8_t length = 0x00 + 2;
	uint8_t *params = NULL;

	if (-1 == write_packet(id, length, I_RESET, params))
	{
		printf("Fail to write packet in 'ping'!\n");
		return -1;
	}

	return 0;
}


// @brief: Turn to a specific angle with certain id.
int ax_turn2angle(uint8_t id, int angle)
{
	if (angle < 0 || angle > 1023)
	{
		printf("Angle out of range in 'turn2angle'!\n");
		return -1;
	}

	uint8_t goal_posi_low, goal_posi_high;
	uint8_t length = 0x03 + 2;
	uint8_t *params = (uint8_t *)malloc((length - 2) * sizeof(uint8_t));
	if (NULL == params)
	{
		perror("Fail to malloc in 'ax_turn2angle'");
		return -1;
	}

	goal_posi_low = angle & 0xFF;
	goal_posi_high = angle >> 8;
	params[0] = CT_GOAL_POSI_L;
	params[1] = goal_posi_low;
	params[2] = goal_posi_high;

	if (-1 == write_packet(id, length, I_WRITE, params))
	{
		printf("Fail to write packet in 'ax_turn2angle'!\n");
		return -1;
	}

	free(params);

	return 0;
}

int ax_move_speed(uint8_t id, int speed)
{
	if (speed < 1 || speed > 1023)
	{
		printf("Speed out of range in 'ax_move_speed'!\n");
		return -1;
	}

	uint8_t move_speed_low, move_speed_high;
	uint8_t length = 0x03 + 2;
	uint8_t *params = (uint8_t *)malloc((length - 2) * sizeof(uint8_t));
	if (NULL == params)
	{
		perror("Fail to malloc in 'ax_move_speed'");
		return -1;
	}

	move_speed_low = speed & 0xFF;
	move_speed_high = speed >> 8;
	params[0] = CT_MOVE_SPEED_L;
	params[1] = move_speed_low;
	params[2] = move_speed_high;

	if (-1 == write_packet(id, length, I_WRITE, params))
	{
		printf("Fail to write packet in 'ax_move_speed'!\n");
		return -1;
	}

	free(params);

	return 0;

}

