#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <signal.h>

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

// Los comentarios representan una sincronizacion utilizando señales, pero se puede hace; 
// en este caso, solo durmiendo a cada hijo un poco para darle tiempo al padre de escribir en la matriz, 
// ya que cada proceso hijo modifia una fila en concreto. 

int main(int argc, char **argv){
    int **matriz, i = 0;
    int numFilas = atoi(argv[1]), numColums = atoi(argv[2]);
    int hijos[numFilas];
    int **segmento;

    if(numFilas < 3 || numColums < 3){
        error("La matriz debe ser minimo de 3x3");
    }

    //signal(SIGUSR1, handlesignal);

    matriz = (int **)malloc(numFilas * sizeof(int*));

    if (!matriz) error("no se pudo asignar memoria");
    
    for (int a = 0; a < numFilas; a++){
        matriz[a] = (int*)malloc(numColums*sizeof(int));
        if (!matriz[a]) error("no se pudo asignar memoria");
    }

    size_t matriz_tamaño = sizeof_dm(numFilas, numColums, sizeof(int)); 

    int id_memoria_compartida = shmget(IPC_PRIVATE, matriz_tamaño, IPC_CREAT | 0600);
    if (id_memoria_compartida == -1) error("no se pudo reservar una memoria");

    segmento = shmat(id_memoria_compartida, NULL, 0);
    create_index((void*)segmento, numFilas, numColums, sizeof(int));

    for (i = 0; i < numFilas; i++){
        hijos[i] = fork();
        if (hijos[i] == 0) break;
    }
    
    if (i == numFilas){
        printf("matriz sin modificar\n");
        for (int b = 0; b < numFilas; b++){
            for (int c = 0; c < numColums; c++){
                matriz[b][c] = 1;
                segmento[b][c] = matriz[b][c];
                printf("%d ", matriz[b][c]);
            }  
            printf("\n");
        }
        //sleep(1);
        //kill(hijos[i-1], SIGUSR1);
        //pause();
        for (i = 0; i < numFilas; i++){
            wait(NULL);
        }
        printf("esta es la matriz resultante\n");
        for (int b = 0; b < numFilas; b++){
            for (int c = 0; c < numColums; c++){
                printf("%d ", segmento[b][c]);
            }
            printf("\n");
        }
        shmctl(id_memoria_compartida, IPC_RMID, NULL);
    }else if(hijos[i] == 0){
        //pause();
        sleep(1);
        for (int c = 0; c < numColums; c++){
            segmento[i][c]++;
        }
        //(i > 0) ? kill(hijos[i-1], SIGUSR1) : kill(getppid(), SIGUSR1);
    }
    
    for (int d = 0; d < numColums; d++){
        free(matriz[d]);
    }
    free(matriz);
    shmdt(segmento);

    return EXIT_SUCCESS;
}