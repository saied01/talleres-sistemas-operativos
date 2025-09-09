#include <signal.h> /* constantes como SIGINT*/
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char* argv[]) {

	if (fork() == 0) {
		for (int i = 0; i<5 ; i++) {
			sleep(1);
			printf("sup!");
			pid_t padre = getppid();
			kill(padre, SIGURG);
		}
	} else {
		signal(SIGURG, urgHandlr());
		wait(NULL);
		execve("ls", argv);
	}
	exit(EXIT_SUCCESS);
}

int urgHandlr() {
	printf("ya va!");
}