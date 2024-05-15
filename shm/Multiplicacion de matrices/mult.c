#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

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

void imprimir_matriz(int  **matriz, int tamaño, char *nameMatriz){
    printf("============Esta es la matriz %s ================\n", nameMatriz);
    for (int c = 0; c < tamaño; c++){
        for (int d = 0; d < tamaño; d++){
            printf("%d ", matriz[c][d]);
        }
        printf("\n");
    } 
}

int main(int argc, char **argv){

    int numHijos = atoi(argv[1]), i = 0;
    int *turno;
    pid_t hijos[numHijos];
    int tamaño_matriz = 2*numHijos;

    size_t tam_seg = sizeof_dm(tamaño_matriz, tamaño_matriz, sizeof(int));

    int id_memory1 = shmget(IPC_PRIVATE, tam_seg, IPC_CREAT | 0600);
    int id_memory2 = shmget(IPC_PRIVATE, tam_seg, IPC_CREAT | 0600);
    int id_memory3 = shmget(IPC_PRIVATE, tam_seg, IPC_CREAT | 0600);
    int  id_turno  = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0600);
    if(id_memory1 == -1 || id_memory2 == -1 || id_memory3 == -1 || id_turno == -1) error("no se pudo asignar memoria compartida");

    int **segmento1 = shmat(id_memory1, NULL, 0);
    int **segmento2 = shmat(id_memory2, NULL, 0);
    int **segmento3 = shmat(id_memory3, NULL, 0);
    turno = shmat(id_turno, NULL, 0);

    *turno = numHijos;

    create_index((void*)segmento1, tamaño_matriz, tamaño_matriz, sizeof(int));
    create_index((void*)segmento2, tamaño_matriz, tamaño_matriz, sizeof(int));
    create_index((void*)segmento3, tamaño_matriz, tamaño_matriz, sizeof(int));

    for ( i = 0; i < numHijos; i++){
        hijos[i] = fork();
        if (hijos[i] == 0) break;
    }

    if (i == numHijos){
        srand(time(NULL));
        printf("padre llenando...\n");
        for (int a = 0; a < tamaño_matriz; a++){
            for (int b = 0; b < tamaño_matriz; b++){
                segmento1[a][b] = rand() % 1 + 3;
                segmento2[a][b] = rand() % 1 + 3;
            }
        }
        imprimir_matriz(segmento1, tamaño_matriz, "A");
        imprimir_matriz(segmento2, tamaño_matriz, "B");
        imprimir_matriz(segmento3, tamaño_matriz, "C");

        *turno = 0;
        
        for (int f = 0; f < numHijos; f++){
            wait(NULL);
        }
        
        printf("Matriz resultantante desde el padre\n");  
        imprimir_matriz(segmento3, tamaño_matriz, "Matriz resultante");

        shmctl(id_memory1, IPC_RMID, 0);
        shmctl(id_memory2, IPC_RMID, 0);
        shmctl(id_memory3, IPC_RMID, 0);
        shmctl(id_turno, IPC_RMID, 0);
    }

    if (hijos[i] == 0){
        while (*turno != i) sleep(2);
        printf("hijo %d operando...\n", getpid());

        int inicio = i;
        int fin = tamaño_matriz - i - 1;
        for (int x = inicio; x <= fin; x++){
            for (int y = inicio; y <= fin; y++){
                if (x == inicio || x == fin || y == inicio || y == fin) {
                    for (int g = inicio; g <= fin; g++){
                        segmento3[x][y] += segmento1[x][g] * segmento2[g][y]; 
                    }
                }
            }
        }
        imprimir_matriz(segmento3, tamaño_matriz, "C");
        *turno += 1; 
    }

    shmdt(segmento1);
    shmdt(segmento2);
    shmdt(segmento3);
    shmdt(turno);

    return EXIT_SUCCESS;
}