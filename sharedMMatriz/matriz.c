#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

unsigned int sizeof_dm(int rows, int cols, size_t sizeElement){
    size_t size;
    size = rows * sizeof(void *); 
    size += (cols * rows * sizeElement); 
    return size;
}
void create_index(void **matriz, int rows, int cols, size_t sizeElement){
    int i;
    size_t sizeRow = cols * sizeElement;
    matriz[0] = matriz + rows;
    for(i = 1; i < rows; i++){
        matriz[i] = (matriz[i-1] + sizeRow);
    }
}

int error(char *mesagge){
    printf("%s\n", mesagge);
    exit(EXIT_FAILURE);
}

void handlesignal(int signal){}

int main(int argc, char **argv) {

    int cols = atoi(argv[1]), rows = atoi(argv[2]);
    int **matriz;
    int **ptr;
    matriz = (int**) malloc(rows * sizeof(int *));
    if (!matriz){
        error("no se pudo asignar memoria");
    }
    
    for (int i = 0; i < rows; i++){
        matriz[i] = (int*)malloc(cols*sizeof(int));
        if (!matriz[i]){
            error("no se pudo asignar memoria");
        }
    }

    size_t sizeMatriz = sizeof_dm(rows, cols, sizeof(int));
    int shmId = shmget(IPC_PRIVATE, sizeMatriz, IPC_CREAT | 0600);
    if (!shmId){
        error("no se pudo crear segmento");
    }
    
    ptr = shmat(shmId, NULL, 0);
    create_index((void*)ptr, rows, cols, sizeof(int));

    pid_t hijo = fork();

    if (hijo == -1) {
        printf("error\n");
        exit(EXIT_FAILURE);
    }

    if (hijo == 0) {
        signal(SIGUSR1, handlesignal);
        pause();
        printf("hijo [%d]\n", getpid());
        for (int j = 0; j < rows; j++) {
            printf("[");
            for (int k = 0; k < cols; k++) {
                printf("%d ", ptr[j][k]);    
            }
            printf("]\n");
        }
    }else if (hijo != 0){
        printf("padre [%d] esperando a [%d]\n", getpid(), hijo);
        sleep(2);
        for (int j = 0; j < rows; j++) {
            for (int k = 0; k < cols; k++){
                matriz[j][k] = (cols * j) + k; 
                ptr[j][k] = matriz[j][k];
            }
        }
        kill(hijo, SIGUSR1);
        wait(NULL);
        printf("terminado\n");
        shmctl(shmId, IPC_RMID, NULL);
    }

    for (int i = 0; i < cols; i++){
        free(matriz[i]);
    }
    free(matriz);

    shmdt(ptr);
    
    return EXIT_SUCCESS;
}