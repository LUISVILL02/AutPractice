#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> 
#include <pthread.h>

int **matriz, *vect, *resultante, filas, columnas;

typedef struct{
    int indice_inicio;
    int indice_fin;
} Indice;

int error(char  *mensaje){
    printf("%s\n", mensaje);
    exit(EXIT_FAILURE);
}

void llenarDatos(char *fileName, int **vect, int ***matriz, int *filas, int *columnas){
    int rows, cols, tam;
    FILE *file;
    file = fopen(fileName, "r");

    if (file == NULL) error("no se pudo abrir el archivo");
    
    fscanf(file, "%d %d", &rows, &cols);

    *filas = rows;
    *columnas = cols;

    printf("filas: %d columnas: %d\n", rows, cols);

    *matriz = (int **)malloc(rows*sizeof(int*));
    if (matriz == NULL) error("no se pudo asignar memoria dinamica");
    
    for (int l = 0; l < rows; l++){
        (*matriz)[l] = (int *)malloc(cols*sizeof(int));
        if ((*matriz)[l] == NULL) error("no se pudo asignar memoria dinamica");
        for (int j = 0; j < cols; j++){
            fscanf(file, "%d", &(*matriz)[l][j]);
        }
    }
    fscanf(file, "%d", &tam);
    *vect = (int *)malloc(tam*sizeof(int));
    if (vect == NULL) error("no se pudo asignar memoria dinamica");
    
    for (int i = 0; i < tam; i++){
        fscanf(file, "%d", &(*vect)[i]);
    }
}

void *multiplicar(void *arg){
    printf("\nsoy el hilo %ld\n", pthread_self());

    Indice *index = (Indice *) arg;
    int inicio = index->indice_inicio;
    int fin = index->indice_fin;

    printf("Mi parte. matriz:\n");
    for (int i = inicio; i < fin; i++){
        resultante[i] = 0;
        for (int j = 0; j < columnas; j++){
            resultante[i] += matriz[i][j] * vect[j];
            printf("%d ", matriz[i][j]);
        }
        printf("\n");
    }
    pthread_exit(0);
}

int main(int argc, char **argv){
    int numHilos = atoi(argv[2]);
    
    pthread_t *hilos = (pthread_t *)malloc(numHilos*sizeof(pthread_t));

    resultante = (int *)malloc(filas*sizeof(int));
    if (resultante == NULL) error("no se pudo asignar memoria dinamica");

    llenarDatos(argv[1], &vect, &matriz, &filas, &columnas);

    if (numHilos > filas || filas % numHilos != 0) error("muchos hilos para poca matriz o deben ser multiplo de el tamaño de la matriz");
    
    Indice *index = (Indice *)malloc(numHilos*sizeof(Indice));
    int delta = filas / numHilos;

    for (int i = 0; i < numHilos; i++){
        index[i].indice_inicio = i * delta;
        index[i].indice_fin = index[i].indice_inicio + delta;
        pthread_create(&hilos[i], NULL, multiplicar, (void *)&index[i]);
    }
    
    for (int j = 0; j < numHilos; j++){
        pthread_join(hilos[j], NULL);
    }

    printf("\nmatriz \n");
    for (int i = 0; i < filas; i++){
        for (int j = 0; j < columnas; j++){
            printf("%d ", matriz[i][j]);
        }
        printf("\n");
    }

    printf("\nvector \n");
    for (int h = 0; h < filas; h++){
        printf("%d ", vect[h]);
    }
    printf("\n\nvector resultante\n");
    for (int h = 0; h < filas; h++){
        printf("%d ", resultante[h]);
    }

    for (int g = 0; g < filas; g++){
        free(matriz[g]);
    }
    free(matriz);
    free(vect);
    free(resultante);
    free(hilos);
    free(index);
    
    return EXIT_SUCCESS;
}