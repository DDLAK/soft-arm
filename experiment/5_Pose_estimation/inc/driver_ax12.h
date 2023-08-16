#ifndef DRIVER_AX12_H
#define DRIVER_AX12_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <bcm2835.h>
#include <stdint.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C"{
#endif

int ax12_init(void);

int ax12_deinit(void);

int write_packet(uint8_t id, uint8_t length, uint8_t instruction, uint8_t *params);

int read_packet(uint8_t length);

int set_id(uint8_t id, uint8_t new_id);
	
int set_status_return_level(uint8_t id, uint8_t level);

int set_baud_rate(uint8_t id, uint8_t baud_rate);

// @brief: Write PING instruction with certain id.
int ax_ping(uint8_t id);

// @brief: Reset the Control Table.
int ax_reset(uint8_t id);

// @brief: Turn to a specific angle with certain id.
int ax_turn2angle(uint8_t id, int angle);

int ax_move_speed(uint8_t id, int speed);

#ifdef __cplusplus
}
#endif

#endif

