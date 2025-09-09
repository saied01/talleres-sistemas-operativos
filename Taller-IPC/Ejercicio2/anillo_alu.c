#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include "constants.h"

int generate_random_number(){
	return (rand() % 50);
}


int run_child(int i, int n, int start, int pipes[][2], int pipe_padre[2])
{
	printf("soy el hijo %d \n", i);

	// CIERRO LOS PIPES QUE NO USA EL HIJO i
	for (int j=0;j<n;j++)
	{
		if (j!=i%n && j!=(i-1+n)%n)
		{
			close(pipes[j][PIPE_WRITE]);
			close(pipes[j][PIPE_READ]);
		}
	} 
	
	close(pipes[i%n][PIPE_READ]);
	close(pipes[(i-1+n)%n][PIPE_WRITE]);


	int buffer_secreto;
	int buffer;
	if (i == start)
	{
		buffer_secreto = generate_random_number();
		printf("el buffer secreto es %d\n", buffer_secreto);
		read(pipes[start][PIPE_READ], &buffer, sizeof(buffer));
		printf("start envia el primer mensaje al anillo");
		buffer++;
		write(pipes[i%n][PIPE_WRITE], &buffer, sizeof(buffer));
	} else
	{
		close(pipe_padre[PIPE_WRITE]);
		close(pipe_padre[PIPE_READ]);
	}

	while ((read(pipes[(i-1+n)%n][PIPE_READ], &buffer, sizeof(buffer))))
	{
		printf("hijo %d recibe el valor %d \n", i, buffer);
		buffer++;

		if (i == start && buffer > buffer_secreto)
		{
			// INFORMO AL PADRE EL VALOR FINAL
			write(pipe_padre[PIPE_WRITE], &buffer, sizeof(buffer));
			// CIERRO PIPES RESTANTES PARA EL EOF
			close(pipes[i%n][PIPE_WRITE]);
			close(pipes[(i-1+n)%n][PIPE_READ]);
			close(pipe_padre[PIPE_READ]);
			close(pipe_padre[PIPE_WRITE]);

			exit(EXIT_SUCCESS);
		}

		write(pipes[i%n][PIPE_WRITE], &buffer, sizeof(buffer));
		printf("hijo %d manda valor %d \n", i, buffer);
	}

	close(pipes[i%n][PIPE_WRITE]);
	close(pipes[(i-1+n)%n][PIPE_READ]);

	exit(EXIT_SUCCESS);
}


int main(int argc, char **argv)
{	
	//Funcion para cargar nueva semilla para el numero aleatorio
	srand(time(NULL));

	int status, n, start, buffer;
	n = atoi(argv[1]);
	buffer = atoi(argv[2]);
	start = atoi(argv[3]);

	if (argc != 4){ printf("Uso: anillo <n> <c> <s> \n"); exit(0);}
    
    printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", n, buffer, start);

	// CREO LAS PIPES POR LA CUAL EL PADRE SE CONECTA AL HIJO 'START'

	int pipe_padre[2];
	pipe(pipe_padre);

	// CREO LAS N PIPES PARA EL ANILLO

	int pipes[n][2];

	for (int i=0;i<n;i++)
	{
		pipe(pipes[i]);
	}

	pid_t *children;
	children = malloc(n * sizeof(pid_t));

	for (int i=0;i<n;i++)
	{
		children[i] = fork();
		if (children[i] == 0)
			run_child(i,n,start,pipes, pipe_padre);
	}

	write(pipes[start][PIPE_WRITE], &buffer, sizeof(buffer));

	int valor_final;
	read(pipe_padre[PIPE_READ], &valor_final, sizeof(valor_final));

	close(pipe_padre[PIPE_READ]);
	close(pipe_padre[PIPE_WRITE]);

	printf("Padre: El valor final es: %d\n", valor_final);

	for (int i=0;i<n;i++)
	{
		close(pipes[i][PIPE_READ]);
		close(pipes[i][PIPE_WRITE]);
	}
	for (int i=0;i<n;i++)
	{
		wait(NULL);
	}

	free(children);
    
	exit(EXIT_SUCCESS);
}
