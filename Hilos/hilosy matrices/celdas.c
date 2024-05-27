#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> 
#include <pthread.h>
#include <string.h>

pthread_mutex_t mutex;

int **matrizA, **matrizB, **matrizC, filas, columnas;
int iter = -1;

typedef struct{
    int value;
} Indice;

int error(char  *mensaje){
    printf("%s\n", mensaje);
    exit(EXIT_FAILURE);
}

void asigna_memoria(int ***matriz, int filas, int columnas){
    *matriz = (int **)malloc(filas*sizeof(int*));
    if(matriz == NULL) error("no se pudo asignar memoria a filas");

    for (int i = 0; i < filas; i++){
        (*matriz)[i] = (int *)malloc(columnas*sizeof(int));
        if ((*matriz)[i] == NULL) error("no se pudo asignar memoria a columnas");
    }
    
}

void llenar_matriz(int ***matriz, int filas, int columnas, char *nombre){
    for (int i = 0; i < filas; i++){
        for (int j = 0; j < columnas; j++){
            if(strcmp(nombre, "C") == 0){
                (*matriz)[i][j] = 0;
            }else 
                (*matriz)[i][j] = 1 + rand() % 9;
        }
    }
}

void imprimir_matriz(int ***matriz, int filas, int columnas, char *nombre){
    printf("--------%s---------\n", nombre);
    for (int i = 0; i < filas; i++){
        for (int j = 0; j < columnas; j++){
            printf("%d ", (*matriz)[i][j]);
        }
        printf("\n");
    }
}

void *multiplicar(void *arg){
    Indice *indiceF = (Indice*) arg;

    pthread_mutex_lock(&mutex);
    if (iter == filas) iter = 0;

    int inicioF = (indiceF->value < filas) ? 0: indiceF->value / filas;
    int finalF = (indiceF->value < filas) ? indiceF->value : iter;
    iter++;

    pthread_mutex_unlock(&mutex);
    
    printf("inicio: %d fin: %d indice: %d\n", inicioF, finalF, indiceF->value);
    
    printf("hilo %d procesando el indice: (%d, %d)\n", indiceF->value +1, inicioF, finalF);
    for (int k = 0; k < columnas; k++){
        matrizC[inicioF][finalF] += matrizA[inicioF][k] * matrizB[k][finalF];
    }
    pthread_exit(0);
}

int main(int argc, char **argv){
    filas = atoi(argv[1]);
    columnas = filas;
    int numHilos = filas*columnas;

    pthread_mutex_init(&mutex, NULL);

    pthread_t *hilos = (pthread_t*)malloc((numHilos)*sizeof(pthread_t));

    asigna_memoria(&matrizA, filas, columnas);
    asigna_memoria(&matrizB, filas, columnas);
    asigna_memoria(&matrizC, filas, columnas);

    llenar_matriz(&matrizA, filas, columnas, "A");
    llenar_matriz(&matrizB, filas, columnas, "B");
    llenar_matriz(&matrizC, filas, columnas, "C");

    imprimir_matriz(&matrizA, filas, columnas, "Matriz A");
    imprimir_matriz(&matrizB, filas, columnas, "Matriz B");
    imprimir_matriz(&matrizC, filas, columnas, "Matriz C");

    Indice *index = (Indice *)malloc(numHilos*sizeof(Indice));

    for (int i = 0; i < numHilos; i++){
        index[i].value = i;
        pthread_create(&hilos[i], NULL, multiplicar, (void *) &index[i])    ;
    }

    for (int j = 0; j < numHilos; j++){
        pthread_join(hilos[j], NULL);
    }

    printf("resultado de la matriz: \n");
    imprimir_matriz(&matrizC, filas, columnas, "Matriz C");

    return EXIT_SUCCESS;
}