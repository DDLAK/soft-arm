#include "driver_mpu9250_basic.h"
#include "driver_ax12.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_PORT 8888
#define LISTEN_BACKLOG 1	// Only permit one terminal to control the robot.
#define BUFFER_SIZE 5+1		// Receive 3 same numbers, to ensure the instruction is correct.


#define I2C_FLAG_RIGHT 0
#define I2C_FLAG_LEFT 1

// MOTOR ID MACROS, 
// L/R means left/right, F/B means forward/backward.
#define MOTOR_RB 0x01
#define MOTOR_RF 0x02
#define MOTOR_LF 0x03
#define MOTOR_LB 0x04

void action_paradigm(int paradigm)
{
	assert(paradigm <= 4 && paradigm >=0);
	
	switch paradigm:
		case 0:
			// paradigm 0: LF 0 degree, LB 0 degree
			ax_turn2angle(MOTOR_LB, 512);
			ax_turn2angle(MOTOR_LF, 512);
			break;
		case 1:
			// paradigm 1: LF 75 degree, LB 0 degree
			ax_turn2angle(MOTOR_LB, 511+0);
			ax_turn2angle(MOTOR_LF, 511-256);
			break;
		case 2:
			// paradigm 2: LF 60 degree, LB -60 degree
			ax_turn2angle(MOTOR_LB, 511+205);
			ax_turn2angle(MOTOR_LF, 511-205);
			break;
		case 3:
		// paradigm 3: LF 60 degree, LB 60 degree
			ax_turn2angle(MOTOR_LB, 511-205);
			ax_turn2angle(MOTOR_LF, 511-205);
			break;
		case 4:
		// paradigm 4: LF 30 degree, LB -150 degree
			ax_turn2angle(MOTOR_LB, 511+512);
			ax_turn2angle(MOTOR_LF, 511-103);
			break;
			
}

int main()
{	
	int server_socket, client_socket;
	struct sockaddr_in server_addr, client_addr;
	int addr_len;
	unsigned char buffer[BUFFER_SIZE];
	int recv_len;

	int i;
	char prev_command = 0, pprev_command = 0;
	char exec_command = 0;
	
	ax12_init();
	
	// (134 / 1023) * 114 ~= 15 RPM 
	ax_move_speed(MOTOR_LF, 134);
	ax_move_speed(MOTOR_LB, 134);
	
	ax_turn2angle(MOTOR_LF, 512);
	ax_turn2angle(MOTOR_LB, 512);
	
	exec_command = 0;

	// Create a socket for tcp server.
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == server_socket)
	{
		perror("Socket");
		exit(EXIT_FAILURE);
	}

	// Bind address to this socket.
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(server_addr.sin_zero, '\0', 8);
	
	if (-1 == bind(server_socket, (const struct sockaddr *)&server_addr, 
			sizeof(server_addr)))
	{
		perror("Bind");
		exit(EXIT_FAILURE);
	}

	// Start to listen.
	if (-1 == listen(server_socket, LISTEN_BACKLOG))
	{
		perror("Listen");
		exit(EXIT_FAILURE);
	}
	
	// Wait for accepting a connection.
	addr_len = sizeof(struct sockaddr);
	client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
	if (client_socket != -1)
	{
		printf("Successful connect to: %d %s\n",
				client_socket, inet_ntoa(client_addr.sin_addr));
	}
	else
	{
		perror("Accept");
	}


	// Send / Receive data.
	while(0 != strcmp(buffer, "end"))
	{
		recv_len = recv(client_socket, &buffer, BUFFER_SIZE-1, 0);
		if (-1 == recv_len)
		{	
			perror("Receive");
			close(client_socket);
			exit(EXIT_FAILURE);
		}
		else
		{
			buffer[recv_len] = '\0';
			printf("Received message: %s\n", buffer);
		}
		
		// Is buffer a command?
		if (buffer[0] != '*' || buffer[recv_len-1] != '.' || recv_len != 6)
		{
			printf("This message is not a command!\n");
			continue;
		}
		
		// Does the command contain 3 same number? 
		if (buffer[1] == buffer[2] && buffer[3] == buffer[2] 
		&& buffer[1] <= '4' && buffer[1] >= '0')
		{
			if (buffer[1] == prev_command && buffer[1] == pprev_command)
			{
				action_paradigm(buffer[1] - '0');
				exec_command = buffer[1];
			}
			
			pprev_command = prev_command;
			prev_command = buffer[1];
		}
		else
		{
			printf("This command is not correct!\n");
			continue;
		}
	}
	
	return 0;
}
	
	
	
	

	



