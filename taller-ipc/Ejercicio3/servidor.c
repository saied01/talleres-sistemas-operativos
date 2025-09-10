#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

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

int main() {
    // burocracia de sockets

    // SOCKET DE CLIENTE/SERVIDOR, ADDR DE CLIENTE/SERVIDOR ....
    int server_socket;
    int client_socket;
    struct sockaddr_un server_addr;
    struct sockaddr_un client_addr;
    unsigned int slen = sizeof(server_addr);
    unsigned int clen = sizeof(client_addr);


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
    listen(server_socket, 1);
     
    // COMPLETAR. Este es un ejemplo de funcionamiento básico.
    // La expresión debe ser recibida como un mensaje del cliente hacia el servidor.
    const char *expresion = "10+5";  
    int resultado = calcular(expresion);
    printf("El resultado de la operación es: %d\n", resultado);
    exit(0);
}

