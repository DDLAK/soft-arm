#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_PORT 8888
#define LISTEN_BACKLOG 1	// Only permit one terminal to control the robot.
#define BUFFER_SIZE 3+1		// Receive 3 same numbers, to ensure the instruction is correct.

int main (int argc, char* argv[])
{
	int server_socket, client_socket;
	struct sockaddr_in server_addr, client_addr;
	int addr_len;
	unsigned char buffer[BUFFER_SIZE];
	int recv_len;

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
	}
		
	return 0;
}
