/**
 * Copyright (c) 2015 - present LibDriver All rights reserved
 * 
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. 
 *
 * @file      driver_mpu9250_interface_template.c
 * @brief     driver mpu9250 interface template source file
 * @version   1.0.0
 * @author    Shifeng Li
 * @date      2022-08-30
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author      <th>Description
 * <tr><td>2022/08/30  <td>1.0      <td>Shifeng Li  <td>first upload
 * </table>
 */

#include "driver_mpu9250_interface.h"
#include <bcm2835.h>

#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define IIC_ADAPTOR_NAME "/dev/i2c-0"

static int i2c_fd;

/**
 * @brief  interface iic bus init
 * @return status code
 *         - 0 success
 *         - -1 iic init failed
 * @note   none
 */
uint8_t mpu9250_interface_iic_init(void)
{
	if (!bcm2835_init())
	{
		perror("Fail to init bcm2835");
		return -1;
	}

	i2c_fd = open(IIC_ADAPTOR_NAME, O_RDWR);

	if (-1 == i2c_fd)
	{
		perror("Fail to open i2c_fd");
		return -1;
	}
	else
	{
		return 0;
	}
}

/**
 * @brief  interface iic bus deinit
 * @return status code
 *         - 0 success
 *         - -1 iic deinit failed
 * @note   none
 */
uint8_t mpu9250_interface_iic_deinit(void)
{
	if (-1 == close(i2c_fd))
	{
		perror("Fail to close i2c_fd");
		return -1;
	}
	else
	{
		return 0;
	}
}

/**
 * @brief      interface iic bus read
 * @param[in]  addr is the iic device write address
 * @param[in]  reg is the iic register address
 * @param[out] *buf points to a data buffer
 * @param[in]  len is the length of the data buffer
 * @return     status code
 *             - 0 success
 *             - -1 read failed
 * @note       none
 */
uint8_t mpu9250_interface_iic_read(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len)
{
	struct i2c_rdwr_ioctl_data i2c_rdwr_data;
	struct i2c_msg msgs[2];

	memset(&i2c_rdwr_data, 0, sizeof(struct i2c_rdwr_ioctl_data));

	memset(msgs, 0, sizeof(struct i2c_msg) *2);

	// Set the parameters.
	msgs[0].addr = addr >> 1;
	msgs[0].flags = 0;
	msgs[0].buf = &reg;
	msgs[0].len = 1;

	msgs[1].addr = addr >> 1;
	msgs[1].flags = I2C_M_RD;
	msgs[1].buf = buf;
	msgs[1].len = len;

	i2c_rdwr_data.msgs = msgs;
	i2c_rdwr_data.nmsgs = 2;

	// Transmit.
	if (-1 == ioctl(i2c_fd, I2C_RDWR, &i2c_rdwr_data))
	{
		perror("Fail to read i2c (mpu9250_interface_iic_read)");
		return -1;
	}

	return 0;
}

/**
 * @brief     interface iic bus write
 * @param[in] addr is the iic device write address
 * @param[in] reg is the iic register address
 * @param[in] *buf points to a data buffer
 * @param[in] len is the length of the data buffer
 * @return    status code
 *            - 0 success
 *            - -1 write failed
 * @note      none
 */
uint8_t mpu9250_interface_iic_write(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len)
{
	struct i2c_rdwr_ioctl_data i2c_rdwr_data;
	struct i2c_msg msgs[1];
	uint8_t buf_send[len + 1];

	memset(&i2c_rdwr_data, 0, sizeof(struct i2c_rdwr_ioctl_data));
	memset(msgs, 0, sizeof(struct i2c_msg) * 1);
	memset(buf_send, 0, sizeof(uint8_t) * (len + 1));

	// Set the parameters.
	msgs[0].addr = addr >> 1;
	msgs[0].flags = 0;
	buf_send[0] = reg;
	memcpy(&buf_send[1], buf, len);
	msgs[0].buf = buf_send;
	msgs[0].len = len + 1;

	i2c_rdwr_data.msgs = msgs;
	i2c_rdwr_data.nmsgs = 1;

	// Transmit.
	if (-1 == ioctl(i2c_fd, I2C_RDWR, &i2c_rdwr_data))
	{	
		perror("Fail to write i2c (mpu9250_interface_iic_write)");
		return -1;
	}

	return 0;
}

/**
 * @brief  interface spi bus init
 * @return status code
 * 	   - 0 success
 * 	   - -1 spi init failed
 * @note   none
 */
uint8_t mpu9250_interface_spi_init(void)
{
	return 0;
}

/**
 * @brief  interface spi bus deinit
 * @return status code
 * 	   - 0 success
 * 	   - -1 spi deinit failed
 * @note   none
 */
uint8_t mpu9250_interface_spi_deinit(void)
{
	return 0;
}

/**
 * @brief      interface spi bus read
 * @param[in]  reg is the register address
 * @param[out] *buf points to a data buffer
 * @param[in] len is the length of data buffer 
 * @return status code
 * 	   - 0 success
 * 	   - -1 read failed
 * @note   none
 */
uint8_t mpu9250_interface_spi_read(uint8_t reg, uint8_t *buf, uint16_t len)
{
	return 0;
}

/**
 * @brief      interface spi bus write
 * @param[in]  reg is the register address
 * @param[out] *buf points to a data buffer
 * @param[in] len is the length of data buffer 
 * @return status code
 * 	   - 0 success
 * 	   - -1 write failed
 * @note   none
 */
uint8_t mpu9250_interface_spi_write(uint8_t reg, uint8_t *buf, uint16_t len)
{
	return 0;
}

/**
 * @brief     interface delay ms
 * @param[in] ms
 * @note      none
 */
void mpu9250_interface_delay_ms(uint32_t ms)
{
	bcm2835_delay(ms);
}

/**
 * @brief     interface print format data
 * @param[in] fmt is the format data
 * @note      none
 */
void mpu9250_interface_debug_print(const char *const fmt, ...)
{
	char str[256];
	uint16_t len;
	va_list args;

	memset((char *)str, '\0', sizeof(char)*256);
	va_start(args, fmt);
	vsnprintf((char*)str, 255, (char const *)fmt, args);
	va_end(args);

	len = strlen((char *)str);
	(void)printf((uint8_t *)str, len);
}

/**
 * @brief     interface receive callback
 * @param[in] type is the irq type
 * @note      none
 */
void mpu9250_interface_receive_callback(uint8_t type)
{
    switch (type)
    {
        case MPU9250_INTERRUPT_MOTION :
        {
            mpu9250_interface_debug_print("mpu9250: irq motion.\n");
            
            break;
        }
        case MPU9250_INTERRUPT_FIFO_OVERFLOW :
        {
            mpu9250_interface_debug_print("mpu9250: irq fifo overflow.\n");
            
            break;
        }
        case MPU9250_INTERRUPT_FSYNC_INT :
        {
            mpu9250_interface_debug_print("mpu9250: irq fsync int.\n");
            
            break;
        }
        case MPU9250_INTERRUPT_DMP :
        {
            mpu9250_interface_debug_print("mpu9250: irq dmp\n");
            
            break;
        }
        case MPU9250_INTERRUPT_DATA_READY :
        {
            mpu9250_interface_debug_print("mpu9250: irq data ready\n");
            
            break;
        }
        default :
        {
            mpu9250_interface_debug_print("mpu9250: irq unknown code.\n");
            
            break;
        }
    }
}

/**
 * @brief     interface dmp tap callback
 * @param[in] count is the tap count
 * @param[in] direction is the tap direction
 * @note      none
 */
void mpu9250_interface_dmp_tap_callback(uint8_t count, uint8_t direction)
{
    switch (direction)
    {
        case MPU9250_DMP_TAP_X_UP :
        {
            mpu9250_interface_debug_print("mpu9250: tap irq x up with %d.\n", count);
            
            break;
        }
        case MPU9250_DMP_TAP_X_DOWN :
        {
            mpu9250_interface_debug_print("mpu9250: tap irq x down with %d.\n", count);
            
            break;
        }
        case MPU9250_DMP_TAP_Y_UP :
        {
            mpu9250_interface_debug_print("mpu9250: tap irq y up with %d.\n", count);
            
            break;
        }
        case MPU9250_DMP_TAP_Y_DOWN :
        {
            mpu9250_interface_debug_print("mpu9250: tap irq y down with %d.\n", count);
            
            break;
        }
        case MPU9250_DMP_TAP_Z_UP :
        {
            mpu9250_interface_debug_print("mpu9250: tap irq z up with %d.\n", count);
            
            break;
        }
        case MPU9250_DMP_TAP_Z_DOWN :
        {
            mpu9250_interface_debug_print("mpu9250: tap irq z down with %d.\n", count);
            
            break;
        }
        default :
        {
            mpu9250_interface_debug_print("mpu9250: tap irq unknown code.\n");
            
            break;
        }
    }
}

/**
 * @brief     interface dmp orient callback
 * @param[in] orientation is the dmp orientation
 * @note      none
 */
void mpu9250_interface_dmp_orient_callback(uint8_t orientation)
{
    switch (orientation)
    {
        case MPU9250_DMP_ORIENT_PORTRAIT :
        {
            mpu9250_interface_debug_print("mpu9250: orient irq portrait.\n");
            
            break;
        }
        case MPU9250_DMP_ORIENT_LANDSCAPE :
        {
            mpu9250_interface_debug_print("mpu9250: orient irq landscape.\n");
            
            break;
        }
        case MPU9250_DMP_ORIENT_REVERSE_PORTRAIT :
        {
            mpu9250_interface_debug_print("mpu9250: orient irq reverse portrait.\n");
            
            break;
        }
        case MPU9250_DMP_ORIENT_REVERSE_LANDSCAPE :
        {
            mpu9250_interface_debug_print("mpu9250: orient irq reverse landscape.\n");
            
            break;
        }
        default :
        {
            mpu9250_interface_debug_print("mpu9250: orient irq unknown code.\n");
            
            break;
        }
    }
}
