#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <stdbool.h>

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

void tamaño_datos(char *fileName, int *Nmuestras, int *Cmuestras, int *days){
    int numMuestras, cantidadMuestras, dias;
    FILE *file;
    file = fopen(fileName, "r");
    if (file == NULL) error("no se pudo abrir el archivo");
    fscanf(file, "%d %d %d", &cantidadMuestras, &numMuestras, &dias);
    *Nmuestras = numMuestras;
    *Cmuestras = cantidadMuestras;   
    *days = dias;
    fclose(file);
}

void leerDatos(char *fileName, int ***matriz, int filas, int colum){
    FILE *file;
    file = fopen(fileName, "r");
    int rows, cols, dias; 

    if (file == NULL){
        printf("error al abrir el archivo con nombre: %s\n", fileName);
        exit(EXIT_FAILURE);
    }

    fscanf(file, "%d %d %d", &rows, &cols, &dias);

    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            fscanf(file, "%d", &(*matriz)[i][j]);
        }
        
    }
    
    fclose(file);
}

int celdasAdyacentes(int **matriz, int status1, int status2, int filas, int columnas, int row, int col){
    int cont = 0;
    for(int x = -1; x <= 1; x++){
        for(int y = -1; y <= 1; y++){
            int r = row + x;
            int c = col + y;
            if(r >= 0 && r < filas && c >= 0 && c < columnas){
                if(matriz[r][c] == status1 || matriz[r][c] == status2 ){
                    cont++;
                }          
            }
        }
    }
    return cont;
}

int main(int argc, char **argv){

    signal(SIGUSR1, handlesignal);

    int numFilas, numCols, dias, i = 0;
    pid_t hijos[4];

    tamaño_datos(argv[1], &numFilas, &numCols, &dias);

    size_t tam_seg = sizeof_dm(numFilas, numCols, sizeof(int));

    int id_memory = shmget(IPC_PRIVATE, tam_seg, IPC_CREAT | 0600);
    if (id_memory == -1) error("no se pudo crear el segmento");

    int id_me_turn = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0600);

    int **matrizSegmen = shmat(id_memory, NULL, 0);
    int *turno = shmat(id_me_turn, NULL, 0);

    create_index((void *)matrizSegmen, numFilas, numCols, sizeof(int));
    
    leerDatos(argv[1], &matrizSegmen, numFilas, numCols);

    for(i = 0; i < 4; i++){
        hijos[i] = fork();
        if (hijos[i] == 0) break;
    }
    *turno = 2;

    for (int r = 0; r < 2; r++){   
        if (i == 4){
            printf("padre leyendo la matriz\n");
            for (int a = 0; a < numFilas; a++){
                for (int b = 0; b < numCols; b++){
                    printf("%d ", matrizSegmen[a][b]);
                }
                printf("\n");
            }
            sleep(3);
            kill(hijos[1], SIGUSR1);
            pause();
        }
        if (i == 0){
            pause();
            for (int t = 0; t < numFilas; t++){
                for (int y = 0; y < numCols; y++){
                    if (matrizSegmen[t][y] == 0){
                        int vecinos = celdasAdyacentes(matrizSegmen, 1, 2, numFilas, numCols, t, y);
                        if (vecinos >= 4){
                            matrizSegmen[t][y] =  34;
                        }
                        
                    }
                }
            }
            sleep(1);
            kill(getppid(), SIGUSR1);
        }
        if (i == 1){
            srand(time(NULL));
            pause();
            printf(" sena recibida\n");
            int probabilidad = 0.1, vecinos;
            for (int t = 0; t < numFilas; t++){
                for (int y = 0; y < numCols; y++){
                    vecinos = celdasAdyacentes(matrizSegmen, 0,0, numFilas, numCols, t, y);
                    int P_media_a_baja = probabilidad + vecinos * 0.3;
                    bool pasa_a_baja =  rand() % 1 + 0 < P_media_a_baja;
                    if (pasa_a_baja){
                        matrizSegmen[t][y] = 51;
                    }
                }
            }
            kill(hijos[0], SIGUSR1);
        }
        
    }
    return EXIT_SUCCESS;
}