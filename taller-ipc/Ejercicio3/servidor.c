#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>   // para signal() y SIGINT



int calcular(const char *expresion) {
    int num1, num2, resultado;
    char operador;

    // Usamos sscanf para extraer los dos números y el operador de la expresión
    if (sscanf(expresion, "%d%c%d", &num1, &operador, &num2) != 3) {
        printf("Formato incorrecto\n");
        return 0;  // En caso de error, retornamos 0.
    }

    // Realizamos la operación según el operador
    switch (operador) {
        case '+':
            resultado = num1 + num2;
            break;
        case '-':
            resultado = num1 - num2;
            break;
        case '*':
            resultado = num1 * num2;
            break;
        case '/':
            if (num2 != 0) {
                resultado = num1 / num2;
            } else {
                printf("Error: División por cero\n");
                return 0;  // Si hay división por cero, retornamos 0.
            }
            break;
        default:
            printf("Operador no reconocido\n");
            return 0;  // Si el operador no es válido, retornamos 0.
    }

    return resultado;
}


int correr_cliente(int client_socket)
{

    while (1)
    {
        char buffer[256] = {0}; // tamaño arbitrario
        size_t len_buf = sizeof(buffer)-1;
        ssize_t len_msg = recv(client_socket, &buffer, len_buf, 0);

        if (len_msg < 0)
        {
            perror("recv failed");
            continue;
        }

        // If no  messages  are available to be received and the peer 
        // has performed an orderly shut‐down, recv() shall return 0.
        if (len_msg == 0)
            break;

        int resultado = calcular(buffer);

        send(client_socket, &resultado, sizeof(resultado), 0);
    }

    close(client_socket);

    exit(0);
}





int main() {

    // signal(SIGINT, handle_sigint);

    // burocracia de sockets

    // SOCKET DE CLIENTE/SERVIDOR, ADDR DE CLIENTE/SERVIDOR ....
    int server_socket;
    int client_socket;
    struct sockaddr_un server_addr;
    struct sockaddr_un client_addr;
    socklen_t slen = sizeof(server_addr);
    socklen_t clen = sizeof(client_addr);


    // The  AF_UNIX  (also  known  as  AF_LOCAL) socket family is used to communicate between
    // processes on the same machine efficiently.
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, "unix_socket");

    // unlink() deletes a name from the filesystem.  If that name was the last link to a file
    // and no processes have the file open, the file is deleted and the space it was using is
    // made available for reuse.
    unlink(server_addr.sun_path);

    // socket()  creates  an  endpoint  for  communication and returns a file descriptor that
    // refers to that endpoint.
    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    bind(server_socket, (struct sockaddr *) &server_addr, slen);
    listen(server_socket, 5);
     
    while (1) {
    client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &clen);
    if (client_socket < 0) {
        perror("accept failed");
        continue;
    }

    if (fork() == 0) {
        // hijo
        close(server_socket);  
        correr_cliente(client_socket);
    }

    close(client_socket);  // padre no usa el socket
}


    close(server_socket);

    exit(0);
}

