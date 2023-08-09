#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <sys/ioctl.h>

int main()
{
	struct i2c_rdwr_ioctl_data i2c_rdwr_data;
	struct i2c_msg msgs[2];
	
	int i2c_fd = open("/dev/i2c-1", O_RDWR);

	memset(&i2c_rdwr_data, 0, sizeof(struct i2c_rdwr_ioctl_data));

	memset(msgs, 0, sizeof(struct i2c_msg) *2);

	char reg = 0x75;

	// Set the parameters.
	msgs[0].addr = 0xD0 >> 1;
	msgs[0].flags = 0;
	msgs[0].buf = &reg;
	msgs[0].len = 1;

	msgs[1].addr = 0xD0 >> 1;
	msgs[1].flags = I2C_M_RD;
	msgs[1].buf = &reg;
	msgs[1].len = 1;

	i2c_rdwr_data.msgs = msgs;
	i2c_rdwr_data.nmsgs = 2;

	// Transmit.
	if (-1 == ioctl(i2c_fd, I2C_RDWR, &i2c_rdwr_data))
	{
		perror("Fail to read i2c (mpu9250_interface_iic_read)");
		return -1;
	}

	printf("the receive data is %x\n", reg);

	return 0;
}

