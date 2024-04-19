#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void handlesignal(int signal){

}

int main(int argc, char *argv[]){

    int numProcess = atoi(argv[1]), i = 0;

    pid_t *pids;
    pid_t padre = getpid();
    pid_t hijoLevel2;
    pid_t hijoLevel3;
    pid_t hijoLevel4;

    pids = (pid_t *)malloc((numProcess * 2 + 2)*sizeof(pid_t));

    signal(SIGUSR1, handlesignal);

    for ( i = 0; i < numProcess; i++){
        pids[i] = fork();
        if (pids[i] == 0){
            hijoLevel2 = getpid();
            pids[numProcess + i] = fork();
            if (i != 1){
                if(i > 0 && hijoLevel2 == getpid()){
                    pause();
                    printf("Proceso 3 %d padre [%d]\n", getpid(), getppid());
                    kill(pids[numProcess+2], SIGUSR1);
                    pause();
                    printf("Proceso 3 %d padre [%d]\n", getpid(), getppid());
                    kill(pids[i-1], SIGUSR1);
                }
                break;
            }else if(pids[numProcess + i] == 0){
                hijoLevel3 = getpid();
                pids[numProcess*2 + 1] = fork();
                if (pids[numProcess*2 + 1] == 0){
                    hijoLevel4 = getpid();
                    pids[numProcess*2 + 2] = fork();
                    break;
                }
            }
            break;
        }
        if (pids[i] == -1){
            printf("no se pudo crear el proceso\n");
            exit(EXIT_FAILURE);
        }   
    }


    if (pids[numProcess+2] == 0 && i == 2){
        pause();
        printf("Proceso 4 %d padre [%d]\n", getpid(), getppid());
        kill(getppid(), SIGUSR1);
        printf("Proceso 4 %d padre [%d]\n", getpid(), getppid());
    }
    if (pids[numProcess*2 + 1] == 0 && i == 1){
        if(hijoLevel3 == getppid()){
            pause();
            printf("Proceso 7 %d padre [%d]\n", getpid(), getppid());
            kill(pids[numProcess*2 + 2], SIGUSR1);
            pause();
            kill(getppid(), SIGUSR1);
        }else
            if(hijoLevel4 == getppid() && i == 1){
                pause();
                printf("Proceso 8 %d padre [%d]\n", getpid(), getppid());
                kill(getppid(), SIGUSR1);
            } 
    } 
    
    if(i == 1 && pids[numProcess + 1] == 0){
        pause();
        printf("Proceso 5 %d padre [%d]\n", getpid(), getppid());
        kill(pids[numProcess*2 + 1], SIGUSR1);
        pause();
        kill(getppid(), SIGUSR1);
    }

    if(i == 1 && padre == getppid()){
        pause();
        printf("Proceso 2 %d padre [%d]\n", getpid(), getppid());
        kill(pids[numProcess + 1], SIGUSR1);
        pause();
        printf("Proceso 2 %d padre [%d]\n", getpid(), getppid());
        kill(pids[i-1], SIGUSR1);
    }
    
    if(i == 0 && padre == getppid()){
        pause();
        printf("Proceso 1 %d padre [%d]\n", getpid(), getppid());
        kill(pids[numProcess], SIGUSR1);
        pause();
        printf("Proceso 1 %d padre [%d]\n", getpid(), getppid());
        kill(padre, SIGUSR1);
    }

    if(i == 0 && pids[numProcess] == 0){
        pause();
        printf("Proceso 6 %d padre [%d]\n", getpid(), getppid());
        kill(getppid(), SIGUSR1);
    }


    if(padre == getpid()){
        sleep(2);
        printf("raiz %d\n", getpid());
        kill(pids[numProcess - 1], SIGUSR1);
        pause();
        for (int j = 0; j < numProcess; j++){
            wait(NULL);
        }
        printf("raiz %d\n", getpid());
    }

    free(pids);

    return EXIT_SUCCESS;
}