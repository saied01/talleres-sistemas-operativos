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

	server_addr.sun_family = AF_UNIX;
	strcpy(server_addr.sun_path, "unix_socket");

	server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
	exit(0);
}
