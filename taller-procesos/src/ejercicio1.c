#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

typedef struct {
    pid_t pid;
    int status;
} tHijo;

int n;
int numero_maldito;
tHijo* hijos;



int generate_random_number(){
    //Funcion para cargar nueva semilla para el numero aleatorio
    srand(time(NULL) ^ getpid());
    return (rand() % n);
}

void pierde(int sig) {
    pid_t pid = wait(NULL);
    for (int i = 0; i<n ; i++) {
        if (hijos[i].pid == pid) {
            hijos[i].status = 0;
        }
    }
}


void handlerMaldito(int sig) {
    int n = generate_random_number();
    if (n == numero_maldito) {
        printf("Patroclo!!!!!!!!!!!!!\n");
        exit(0);
    }
}


int funcionHijo() {
    signal(SIGTERM, handlerMaldito);
    while (1) {
        pause();
    };
}


int main(int argc, char const *argv[]){
    
    n = atoi(argv[1]);
	int rondas = atoi(argv[2]);
	numero_maldito = atoi(argv[3]);

    hijos = malloc(n*sizeof(tHijo));
    for (int i = 0; i<n ; i++) {
        hijos[i].status = 1;
    }

    signal(SIGCHLD, pierde);

    for (int i=0; i < n; i++) {
        printf("se crea un hijo\n");
        hijos[i].pid= fork();
        if (hijos[i].pid == 0) {
            funcionHijo();
        }
        
    }
    printf("padre sigue\n");
    
    // k veces

    for (int j = 0; j<rondas ; j++) {
        printf("ronda %d \n", j);
        for (int i = 0; i<n ; i++) {
            if (hijos[i].status == 1) {
                kill(hijos[i].pid, SIGTERM);
                sleep(1);
            }
        }
    }

    for (int i = 0; i<n ; i++) {
        pid_t pid = hijos[i].pid;
        if (hijos[i].status == 1) {
            printf("hijo %d gano\n", pid);
            kill(pid, SIGKILL);
            sleep(1);
        } else {
            printf("hijo %d perdio\n", pid);
        }
    }

    free(hijos);


    exit(0);
}