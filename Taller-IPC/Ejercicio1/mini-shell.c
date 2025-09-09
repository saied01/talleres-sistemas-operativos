#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include "constants.h"
#include "mini-shell-parser.c"

void ejecutar_hijo(int i, int count, int pipes[][2], char ***progs)
{
	for (int j = 0; j < count; j++)
	{
		if (j != i && j != i - 1)
		{
			close(pipes[j][PIPE_READ]);
			close(pipes[j][PIPE_WRITE]);
		}
	}

	if (i == 0)
	{
		close(pipes[i][PIPE_READ]);
		dup2(pipes[i][PIPE_WRITE], STDOUT_FILENO);
	}
	else if (i == count)
	{
		close(pipes[i - 1][PIPE_WRITE]);
		dup2(pipes[i - 1][PIPE_READ], STDIN_FILENO);
	}
	else
	{
		close(pipes[i - 1][PIPE_WRITE]);
		dup2(pipes[i - 1][PIPE_READ], STDIN_FILENO);
		close(pipes[i][PIPE_READ]);
		dup2(pipes[i][PIPE_WRITE], STDOUT_FILENO);
	}

	execvp(progs[i][0], progs[i]);
	perror("fallo execvp");
	return;
}

static int run(char ***progs, size_t count)
{
	int r, status;

	// Reservo memoria para el arreglo de pids
	// TODO: Guardar el PID de cada proceso hijo creado en children[i]
	pid_t *children = malloc(sizeof(*children) * count);

	// TODO: Pensar cuantos procesos necesito count
	// TODO: Pensar cuantos pipes necesito. count - 1
	int pipes[count - 1][2];
	for (int i = 0; i < count-1; i++)
	{
		pipe(pipes[i]);
	}

	for (int i = 0; i < count; i++)
	{
		children[i] = fork();
		if (children[i] == 0)
		{
			ejecutar_hijo(i, count-1, pipes, progs);
		}

	}

	for (int i = 0; i < count-1; i++)
	{
		close(pipes[i][PIPE_READ]);
		close(pipes[i][PIPE_WRITE]);
	}
	
	// TODO: Para cada proceso hijo:
	// 1. Redireccionar los file descriptors adecuados al proceso
	// 2. Ejecutar el programa correspondiente

	// Espero a los hijos y verifico el estado que terminaron
	for (int i = 0; i < count; i++)
	{
		waitpid(children[i], &status, 0);

		if (!WIFEXITED(status))
		{
			fprintf(stderr, "proceso %d no terminó correctamente [%d]: ",
					(int)children[i], WIFSIGNALED(status));
			perror("");
			return -1;
		}
	}
	r = 0;
	free(children);

	return r;
}

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("El programa recibe como parametro de entrada un string con la linea de comandos a ejecutar. \n");
		printf("Por ejemplo ./mini-shell 'ls -a | grep anillo'\n");
		return 0;
	}
	int programs_count;
	char ***programs_with_parameters = parse_input(argv, &programs_count);

	printf("status: %d\n", run(programs_with_parameters, programs_count));

	fflush(stdout);
	fflush(stderr);

	return 0;
}
