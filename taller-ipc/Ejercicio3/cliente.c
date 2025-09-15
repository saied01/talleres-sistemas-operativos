#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

int main() {
	// burocracia de sockets
	int server_socket;
	struct sockaddr_un server_addr;
	socklen_t slen = sizeof(server_addr); 

	server_addr.sun_family = AF_UNIX;
	strcpy(server_addr.sun_path, "unix_socket");

	server_socket = socket(AF_UNIX, SOCK_STREAM, 0);

	connect(server_socket, (struct sockaddr *) &server_addr, slen);

	while (1)
	{
		printf("Escribir operacion:");
		char *line = NULL;
		size_t line_cap = 0;

		ssize_t line_len = getline(&line, &line_cap, stdin);

		if (line_len <= 0)
		{
			free(line);
			break;
		}

		line[line_len-1] = '\0';

		if (strcmp(line, "exit") == 0)
		{
			free(line);
			break;
		}

		send(server_socket, line, strlen(line), 0);

		int res;
		recv(server_socket, &res, sizeof(res), 0);

		printf("El valor recibido por el cliente es: %d\n", res);

		free(line);
	}

	close(server_socket);

	exit(0);
}
