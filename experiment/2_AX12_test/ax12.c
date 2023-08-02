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

struct instruction_packet
{
	unsigned char start[2];
	unsigned char id;
	unsigned char length;
	unsigned char instruction;
	unsigned char *parameters;
	unsigned char checksum;
};

struct status_packet
{
	unsigned char start[2];
	unsigned char id;
	unsigned char length;
	unsigned char error;
	unsigned char *parameters;
	unsigned char checksum;
};

int write_packet(int fd, struct instruction_packet *packet)
{
	
}

int main(int argc, char *argv[])
{
	int serial_fd;
	struct termios serial_options;
	struct instruction_packet i_packet;
	struct status_packet s_packet;

	int ret;
	char ch;

	if (!bcm2835_init())
	{
		return -1;
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

	
	// Test.
//	while(1)
//	{
//		scanf("%c", &ch);
//
	//	bcm2835_gpio_write(PIN_CTRL, HIGH);
	//	ret = write(serial_fd, &ch, 1);
	//	printf("write [%c], ret is %d \n", ch, ret);
	//
	//	bcm2835_delay(200);
	//	bcm2835_gpio_write(PIN_CTRL, LOW);
	//	ret = read(serial_fd, &ch, 1);
	//	if (ret < 0)
	//	{
	//		printf("read fail, ret is %d\n", ret);
	//	}
	//	else
	///	{
	//		printf("recv a char: [0x%02x][%c]\n", ch, ch);	
	//	}	
	//}

	// Write 'PING' instruction to ax12.
	bcm2835_gpio_write(PIN_CTRL, HIGH);
	i_packet.start[0] = START_FLAG;
	i_packet.start[1] = START_FLAG;
	i_packet.id = 0x04;
	i_packet.length = 0x05;
	i_packet.instruction = I_WRITE;
	i_packet.parameters = (char *)malloc(3*sizeof(char));
	if (NULL == i_packet.parameters)
	{
		perror("Malloc");
		exit(EXIT_FAILURE);
	}
	i_packet.parameters[0] = 0x1E;
	i_packet.parameters[1] = 0x00;
	i_packet.parameters[2] = 0x00;
	i_packet.checksum = ~(i_packet.id + i_packet.length + i_packet.instruction +
			i_packet.parameters[0] + i_packet.parameters[1] + i_packet.parameters[2]);

	printf("%x %x %x ", i_packet.parameters[0], i_packet.parameters[1], i_packet.parameters[2]);
	printf("%x", i_packet.checksum);
	
	int fd = open("./output", O_CREAT | O_RDWR);
	write(fd, &i_packet, sizeof(i_packet));

	if (-1 == write(serial_fd, &i_packet, sizeof(i_packet)))
	{
		perror("Fail to write i_packet");
		exit(EXIT_FAILURE);
	}

	bcm2835_delay(500);

	i_packet.parameters[1] = 0x96;
	i_packet.checksum = ~(i_packet.id + i_packet.length + i_packet.instruction +
			i_packet.parameters[0] + i_packet.parameters[1] + i_packet.parameters[2]);

	if (-1 == write(serial_fd, &i_packet, sizeof(i_packet)))
	{
		perror("Fail to write i_packet");
		exit(EXIT_FAILURE);
	}

//	bcm2835_gpio_write(PIN_CTRL, HIGH);
//	ch = 0xFF;
//	write(serial_fd, &ch, 1);
//	ch = 0xFF;
//	write(serial_fd, &ch, 1);
//	ch = 0x04;
//	write(serial_fd, &ch, 1);
//	ch = 0x05;
//	write(serial_fd, &ch, 1);
//	ch = 0x03;
//	write(serial_fd, &ch, 1);
//	ch = 0x1E;
//	write(serial_fd, &ch, 1);
//	ch = 0x00;
//	write(serial_fd, &ch, 1);
//	ch = 0x00;
//	write(serial_fd, &ch, 1);
//	ch = 0xD5;
//	write(serial_fd, &ch, 1);
//	bcm2835_delay(500);
		

//	ch = 0xFF;
//	write(serial_fd, &ch, 1);
//	ch = 0xFF;
//	write(serial_fd, &ch, 1);
//	ch = 0x04;
//	write(serial_fd, &ch, 1);
//	ch = 0x05;
//	write(serial_fd, &ch, 1);
//	ch = 0x03;
//	write(serial_fd, &ch, 1);
//	ch = 0x1E;
//	write(serial_fd, &ch, 1);
//	ch = 0x96;
//	write(serial_fd, &ch, 1);
//	ch = 0x00;
//	write(serial_fd, &ch, 1);
//	ch = 0x3F;
//	write(serial_fd, &ch, 1);
//	bcm2835_delay(500);

//	printf("done write\n");

//	char buffer[9];
///
//	bcm2835_gpio_write(PIN_CTRL, HIGH);
//	buffer[0] = 0xFF;
//	buffer[1] = 0xFF;
//	buffer[2] = 0x04;
//	buffer[3] = 0x05;
//	buffer[4] = 0x03;
//	buffer[5] = 0x1E;
//	buffer[6] = 0x96;
//	buffer[7] = 0x00;
//	buffer[8] = 0x3F;
//	printf("return write %d\n", write(serial_fd, &buffer, sizeof(buffer)));
//	memset(&buffer, 0, 11);
//	
//	bcm2835_delay(500);
//
//	bcm2835_gpio_write(PIN_CTRL, LOW);
//	printf("return read %d\n", read(serial_fd, &buffer, 3));
//	printf("read data%x\n", buffer[0]);
//	printf("read data %x\n", buffer[1]);
//	printf("read data %x\n", buffer[2]);

	// Read from ax12.
//	s_packet.parameters = (char *)malloc(3*sizeof(char));
//	if (NULL == s_packet.parameters)
//	{
//		perror("Malloc");
//		exit(EXIT_FAILURE);
//	}
//	
//	if (-1 == read(serial_fd, &s_packet, sizeof(s_packet)))
//	{
//		perror("Fail to read s_packet");
//		exit(EXIT_FAILURE);
//	}
//
//	printf("id: %x, length: %x, error: %x\n", s_packet.id, s_packet.length, s_packet.error);
//	printf("parameters: 1. %x, 2. %x, 3. %x\n", s_packet.parameters[0], s_packet.parameters[1],
//		       	s_packet.parameters[2]);
	

	// Close the file.
	if (-1 == close(serial_fd))
	{
		perror("Close");
		exit(EXIT_FAILURE);
	}

	return 0;
}
