#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_PORT 8888
#define LISTEN_BACKLOG 10

int main (int argc, char* argv[])
{
	int server_socket, client_socket;
	struct sockaddr_in server_addr, client_addr;
	int addr_len;

	// Create a socket for tcp server.
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == -1)
	{
		perror("Socket");
		exit(EXIT_FAILURE);
	}

	// Bind address to this socket.
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(server_addr.sin_zero, '\0', 8);
	
	if (bind(server_socket, (const struct sockaddr *)&server_addr, 
			sizeof(server_addr)) == -1)
	{
		perror("Bind");
		exit(EXIT_FAILURE);
	}

	// Start to listen.
	if (listen(server_socket, LISTEN_BACKLOG) == -1)
	{
		perror("Listen");
		exit(EXIT_FAILURE);
	}
	
	// Wait for accepting a connection.
	addr_len = sizeof(struct sockaddr);
	int accept_return;
	while (1)
	{
		accept_return = accept(server_socket, (struct sockaddr *)&client_addr,
				&addr_len);
		printf("%d\n", accept_return);
		if (accept_return != -1)
		{
			printf("Successful\n");
			printf("Successful connect to: %s\n",
					inet_ntoa(client_addr.sin_addr));

		}
		else
		{
			perror("Accept");
		}
	}


	// Send / Receive data.
	
	return 0;
}
