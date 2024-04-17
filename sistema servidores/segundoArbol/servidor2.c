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
    pid_t nivel2;
    pids = (pid_t *) malloc((numProcess * 3)*sizeof(pid_t));

    signal(SIGUSR1, handlesignal);

    if (!pids){
        printf("error\n");
        exit(EXIT_FAILURE);
    }
    

    for (i = 0; i < numProcess; ++i){
        pids[i] = fork();
        if (pids[i] == 0){
            if ((i + 1) % 2 == 0){
                pids[numProcess + i] = fork();
                if (pids[numProcess + i] == 0){
                    nivel2 = getpid();
                    pids[2*numProcess + i] = fork();
                    if (pids[2*numProcess + i] != 0){
                        pause();
                        printf("soy el hijo con pid: %d\n", getpid());
                        kill(pids[i -1], SIGUSR1);
                        pause();
                        printf("soy el hijo con pid: %d\n", getpid());
                        kill(getppid(), SIGUSR1);
                        break;
                    }else{
                        pause();
                        printf("proceso %d padre %d\n", getpid(), getppid());
                        kill(getppid(), SIGUSR1);
                    }
                }
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
                    printf("soy el hijo %d", i+1);
                    pause();
                    printf("hijo %d\n", i +1);
                    kill(pids[i -1], SIGUSR1);
                }
                
            }else{
                pause();
                printf("hijo %d\n", i +1);
                kill(pids[i -1], SIGUSR1);
            }

            
        }else if(pids[numProcess + i] == 0){
            //printf("%d\n", i);
            pause();
            printf("nieto\n");
            kill(getppid(), SIGUSR1);
        }
    }

    if(getpid() == padre){
        printf("soy la raiz a %d %d\n", getpid(), i);
        sleep(3);
        kill(pids[i], SIGUSR1);
        pause();
        for (int j = 0; j < numProcess; j++){
            wait(NULL);
        }
        printf("soy la raiz a\n");

    }
    return 0;
}