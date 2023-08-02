#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <bcm2835.h>

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

int write_packet(int fd, char id, char length, char instruction, char p1, char p2, char p3)
{

	char ch;
	int ret = 0;

	// Set the PIN_CTRL in order to write.
	bcm2835_gpio_write(PIN_CTRL, HIGH);
	
	ch = START_FLAG;
	ret += write(fd, &ch, 1);
	ch = START_FLAG;
	ret += write(fd, &ch, 1);
	ch = id;
	ret += write(fd, &ch, 1);
	ch = length;
	ret += write(fd, &ch, 1);
	ch = instruction;
	ret += write(fd, &ch, 1);
	ch = p1;
	ret += write(fd, &ch, 1);
	ch = p2;
	ret += write(fd, &ch, 1);
	ch = p3;
	ret += write(fd, &ch, 1);
	ch = ~(id+length+instruction+p1+p2+p3);
	ret += write(fd, &ch, 1);
	
	// TODO: 不定长参数
		
	if (ret != length+4)
	{
		return -1;
	}
	
	return 0;
}	

int main(int argc, char *argv[])
{
	int serial_fd;
	struct termios serial_options;
	char *instr_packet, *status_packet;   

	if (!bcm2835_init())
	{
		printf("Fail to init bcm2835");
		exit(EXIT_FAILURE);
	}
	bcm2835_gpio_fsel(PIN_CTRL, BCM2835_GPIO_FSEL_OUTP);

	// Open file /dev/ttyAMA0.
	serial_fd = open(SERIAL_PATH, O_RDWR | O_NOCTTY);
	if (-1 == serial_fd)
	{
		perror("Open");
		exit(EXIT_FAILURE);
	}

	printf("serial_fd %d\n", serial_fd);

	if (0 == isatty(serial_fd))
	{
		printf("Wrong file descriptor: serial_fd is not an open termial device!");
		exit(EXIT_FAILURE);
	}

	printf("serial_fd name: %s\n", ttyname(serial_fd));

	// Set up the UART.
	if (-1 == tcgetattr(serial_fd, &serial_options))
	{
		perror("Get termial attributes (tcgetattr)");
		exit(EXIT_FAILURE);
	}
	
	// Only modified the attribute(s) we are interested in.
	// Default protocol of AX12: speed 1000000 bps, 8 bit, 1 stop, No parity.
	serial_options.c_iflag &= ~(INPCK | ISTRIP);
	serial_options.c_cflag |= CLOCAL | CREAD | CS8;
	serial_options.c_cflag &= ~PARENB;
	serial_options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	serial_options.c_cc[VMIN] = 1;
	serial_options.c_cc[VTIME] = 0;
	if (-1 == cfsetispeed(&serial_options, BAUDRATE))
	{
		perror("Fail to set input speed (cfsetispeed)");
		exit(EXIT_FAILURE);
	}
	
	if (-1 == cfsetospeed(&serial_options, BAUDRATE))
	{
		perror("Fail to set input speed (cfsetispeed)");
		exit(EXIT_FAILURE);
	}


	// Reflush and set the new structure.
	if (-1 == tcflush(serial_fd, TCIFLUSH))
	{
		perror("Fail to flush the serial_fd (tcflush)");
		exit(EXIT_FAILURE);
	}
	if (-1 == tcsetattr(serial_fd, TCSANOW, &serial_options))
	{
		perror("Set attributes in structure termios (tcsetattr)");
		exit(EXIT_FAILURE);
	}

	
	// Write instruction to ax12.
	write_packet(serial_fd, 0x01, 0x05, I_WRITE, 0x1E, 0x00, 0x00);
	write_packet(serial_fd, 0x02, 0x05, I_WRITE, 0x1E, 0x00, 0x00);
	write_packet(serial_fd, 0x03, 0x05, I_WRITE, 0x1E, 0x00, 0x00);
	write_packet(serial_fd, 0x04, 0x05, I_WRITE, 0x1E, 0x00, 0x00);

	// Close the file.
	if (-1 == close(serial_fd))
	{
		perror("Close");
		exit(EXIT_FAILURE);
	}

	return 0;
}
