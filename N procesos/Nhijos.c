#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void handleSignal(int signal){
    printf("señal\n");
}

int main(int argc, char *argv[]){

    int numProcces = atoi(argv[1]), i = 0;

    pid_t padre = getpid();

    pid_t pids[numProcces];

    signal(SIGUSR1, handleSignal);

    for (i = 0; i <= numProcces; i++){
        pids[i] = fork();
        if (pids[i] != 0){
            pause();
            printf("soy padre %d con pid: %d hijo de %d\n", i+1, getpid(), getppid());
            (i > 0) ? kill(getppid(), SIGUSR1) : printf("finish\n"); 
            break;
        }else if(pids[i] == -1){
            printf("error\n");
            exit(EXIT_FAILURE);
        }
        
    }

    if (i-1 == numProcces){
        printf("soy el ultimo hijo creado con pid: %d hijo de %d\n", getpid(), getppid());
        sleep(2);
        kill(getppid(), SIGUSR1);   
    }  
        
    return 0;
}