#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include "constants.h"
#include "mini-shell-parser.c"

// count --> cantidad de programas diferentes que corro, 
// 			es decir cantidad de valores separados por pipes "|"
// progs --> ['ls -al', 'wc -l', ....]


void exec_shell(int pipes[][2], int i, int count, char ***args)
{
	// 3 casos --> es primer comando
	// 		   --> es comando del medio
	//		   --> es ultimo comando

	// primero comando: lee de la shell y escribe en stdout
	// comando del diome: lee del valor devuelto por el comando anterior y escribe en stdout
	// comando final: lee del anterior y escribe en el predeterminado (shell o salida)

	// cierro los pipes que no va a usar
	for (int j=0;j<count-1;j++)
	{
		if (j != i && j != i-1)
		{
			close(pipes[j][0]);
			close(pipes[j][1]);
		}
	}


	// es primer comando?
	if (i == 0)
	{
		dup2(pipes[i][1], STDOUT_FILENO);
	}
	if (i != 0)
	{
		// es ultimo comando?
		if (i == count-1)
		{
			dup2(pipes[i-1][0], STDIN_FILENO);
		} else
		{
			dup2(pipes[i][1], STDOUT_FILENO);
			dup2(pipes[i-1][0], STDIN_FILENO);
		}
	}
	
	execvp(args[i][0], args[i]);

	perror("exec fail");
	exit(EXIT_FAILURE);
}


static int run(char ***progs, size_t count)
{	
	int r, status;

	//Reservo memoria para el arreglo de pids
	pid_t *children = malloc(sizeof(*children) * count);


	// crear n-1 pipes 
// (comando 1 se conecta con el 2 que se conecta con el 3 ... por eso n-1)
	int pipes[count-1][2];
	for (int i=0;i<count-1;i++)
	{
		pipe(pipes[i]);
	}


	// crear n hijos y los agregue al arreglo
	for (int i=0; i<count; ++i)
	{
		pid_t pid = fork();
		if (pid == 0)
			exec_shell(pipes, i, count, progs);
		else
		{
			children[i] = pid;
			if (i < count)
			{
				close(pipes[i][0]);
				close(pipes[i][1]);
			}
		}
	}


	//Espero a los hijos y verifico el estado que terminaron
	for (int i = 0; i < count; i++) {
		waitpid(children[i], &status, 0);

		if (!WIFEXITED(status)) {
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
	if (argc < 2) {
		printf("El programa recibe como parametro de entrada un string con la linea de comandos a ejecutar. \n"); 
		printf("Por ejemplo ./mini-shell 'ls -a | grep anillo'\n");
		return 0;
	}
	int programs_count;
	char*** programs_with_parameters = parse_input(argv, &programs_count);

	printf("status: %d\n", run(programs_with_parameters, programs_count));

	fflush(stdout);
	fflush(stderr);

	return 0;
}
