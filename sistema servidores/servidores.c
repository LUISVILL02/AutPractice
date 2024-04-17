#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void handlesignal(int signal){
    //printf("señal\n");
}

int main(int argc, char *argv[]){

    int numProcess = atoi(argv[1]), i = 0;
    pid_t padre = getpid();

    pid_t *pids;
    pids = (pid_t *) malloc((numProcess * 2)*sizeof(pid_t));

    signal(SIGUSR1, handlesignal);

    if (!pids){
        printf("error\n");
        exit(EXIT_FAILURE);
    }
    

    for (i = 0; i < numProcess; ++i){
        pids[i] = fork();
        if (pids[i] == 0){
            if ((i + 1) % 2 != 0){
                pids[numProcess + i] = fork();
                if (pids[numProcess + i] == 0)break;
            }
            break;
        }
    }

    if (pids[i] == 0){
        if(padre == getppid()){
            if (i > 0){   
                if (pids[numProcess + i] != 0){  
                    pause();
                    printf("hijo %d\n", i +1);
                    kill(pids[numProcess + i], SIGUSR1);
                    pause();
                    printf("hijo %d\n", i +1);
                    kill(pids[i -1], SIGUSR1);
                }else{
                    pause();
                    printf("hijo %d\n", i +1);
                    kill(pids[i -1], SIGUSR1);
                }
            }else{
                pause();
                printf("hijo %d\n", i +1);
                kill(pids[numProcess + i], SIGUSR1);
                pause();
                printf("hijo %d\n", i +1);
                kill(getppid(), SIGUSR1);
            }
            
        }else if(pids[numProcess + i] == 0){
            pause();
            printf("nieto\n");
            kill(getppid(), SIGUSR1);
        }
        
    }
    if(getpid() == padre){
        printf("soy la raiz a\n");
        sleep(2);
        kill(pids[i-1], SIGUSR1);
        pause();
        for (int j = 0; j < numProcess; j++){
            wait(NULL);
        }
        printf("soy la raiz a\n");

    }
    

    return 0;
}