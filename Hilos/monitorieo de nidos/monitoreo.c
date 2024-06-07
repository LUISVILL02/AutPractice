#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> 
#include <pthread.h>

pthread_mutex_t mutex;

int **matriz, filas, columnas, numNidos = 0;

typedef struct{
    int startF;
    int endF;
    int startC;
    int endC;
} Indices;

typedef struct{
    int i;
    int j;
    pthread_t hilo;
} Coordenadas;

Coordenadas coordenadas[100];

int error(char  *mensaje){
    printf("%s\n", mensaje);
    exit(EXIT_FAILURE);
}

void leer_imagen(char *fileName, int ***mat, int *filas, int *columnas){
    FILE *file;
    file = fopen(fileName, "r");
    if (file == NULL) error("no se pudo abir el archivo");

    fscanf(file, "%d %d", &(*filas), &(*columnas));

    *mat = (int **)malloc(*filas*sizeof(int*));
    if(*mat == NULL) error("no se puede asignar memoria para las filas");

    for (int i = 0; i < *filas; i++){
        (*mat)[i] = (int *)malloc(*columnas*sizeof(int));
        if ((*mat)[i] == NULL) error("no se puede asignar memoria para las columnas");

        for (int j = 0; j < *columnas; j++){
            fscanf(file, "%d", &(*mat)[i][j]);
        }
    } 
    fclose(file);
}

int vefificar_nido(int ***m, int i, int j, int p){
    int x, y;
    x = (i == 0) ? i + 1 : i;
    y = (j == 0) ? j + 1 : j;

    if (i + 1 == filas){
        x = i;
        i -= 1;
    }
    if (j + 1 == columnas){
        y = j;
        j -= 1;
    }

    for (int a = x - 1; a < i + 2; a++){
        for (int b = y - 1; b < j + 2; b++){
            if((*m)[a][b] == p) return 1;
        }
    }
    return 0;
}

void *buscar_nidos(void *arg){
    Indices *indice = (Indices *) arg;
    for (int i = indice->startF; i < indice->endF; i++){
        for (int j = indice->startC; j < indice->endC; j++){
            if (matriz[i][j] == 1){
                if(vefificar_nido(&matriz, i, j, 2)) {
                    coordenadas[numNidos].i = i;
                    coordenadas[numNidos].j = j;
                    coordenadas[numNidos].hilo = pthread_self();
                    pthread_mutex_lock(&mutex);
                    numNidos++;
                    pthread_mutex_unlock(&mutex);
                }
            }
        }
    }
    pthread_exit(0);
}

int main(int argc, char **argv){
    pthread_mutex_init(&mutex, NULL);
    int numHilos = 4;
    pthread_t *hilos = (pthread_t*)malloc(numHilos*sizeof(pthread_t));
    if (hilos == NULL)error("no se pudo asignar memoria a los hilos");

    leer_imagen(argv[1], &matriz, &filas, &columnas);

    Indices *indice = (Indices*)malloc(numHilos*sizeof(Indices));
    if (indice == NULL)error("no se pudo asignar memoria a la estructura indices");
    
    for (int i = 0; i < numHilos; i++){
        indice[i].startC = (i % 2 == 0) ? 0 : columnas / 2;
        indice[i].endC = (i % 2 == 0) ? columnas / 2 : columnas;
        indice[i].startF = (i < 2) ? 0 : filas / 2;
        indice[i].endF = (i < 2) ? filas / 2 : filas;
        pthread_create(&hilos[i], NULL, buscar_nidos, (void *) &indice[i]);
    }

    for (int i = 0; i < numHilos; i++){
        pthread_join(hilos[i], NULL);
    }

    for (int i = 0; i < numNidos; i++){
        printf("hay un nido aqui en (%d, %d). Lo encontro %ld\n", coordenadas[i].i, coordenadas[i].j, coordenadas[i].hilo);
    }

    for (int i = 0; i < filas; i++){
        free(matriz[i]);
    }
    free(matriz);
    free(hilos);
    free(indice);

    return EXIT_SUCCESS;
}