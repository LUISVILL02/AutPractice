#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> 
#include <pthread.h>


int *vect, tam, repeticiones, *indices;
pthread_t *hilos_se;

typedef struct Nodo {
    int indice_inicio;
    int indice_fin;
    int target;
} Valores;


int error(char  *mensaje){
    printf("%s\n", mensaje);
    exit(EXIT_FAILURE);
}

void leerVector(char *fileName, int **vect, int *tamaño){
    int tam;
    FILE *file;
    file = fopen(fileName, "r");

    if (file == NULL) error("no se pudo abrir el archivo");
    
    fscanf(file, "%d", &tam);

    *tamaño = tam;

    *vect = (int *)malloc(tam*sizeof(int));

    if (*vect == NULL) error("no se pudo asignar memoria dinamica");
    
    for (int i = 0; i < tam; i++){
        fscanf(file, "%d", &(*vect)[i]);
    }
}

void* contar_repeticiones(void *arg){
    Valores *val = (Valores *) arg;
    int inicio = val->indice_inicio;
    int fin = val->indice_fin;
    int i = 0;
    pthread_t hilo_id = pthread_self();
    printf("\nHilo: %ld buscando valores repetidos para: %d\n", hilo_id, val->target);
    for (i = inicio; i < fin; i++){
        if (vect[i] == val->target){ 
            indices[repeticiones] = i;
            hilos_se[repeticiones] = hilo_id;
            repeticiones++;
        }
        printf("%d ", vect[i]);
    }
    pthread_exit(0);
}

int main(int argc, char **argv){
    int i = 0, numHilos = atoi(argv[2]), target = atoi(argv[3]);
    pthread_t *hilos;
    hilos = (pthread_t *)malloc(numHilos*sizeof(pthread_t));

    if (hilos == NULL)error("no se pudo asignar memoria para los hilos");
    
    leerVector(argv[1], &vect, &tam);

    int delta = tam / numHilos;

    Valores *values = (Valores *)malloc(numHilos*sizeof(Valores));

    indices = (int *)malloc(tam*sizeof(int));
    hilos_se = (pthread_t *)malloc(tam*sizeof(pthread_t));

    if (indices == NULL)error("no se pudo asignar memoria para los indices");
    if (hilos_se == NULL)error("no se pudo asignar memoria para los hilos");

    for (i = 0; i < numHilos; i++){ 
        values[i].target = target;
        values[i].indice_inicio = i * delta;
        values[i].indice_fin = values[i].indice_inicio + delta;
        pthread_create(&hilos[i], NULL, contar_repeticiones, (void *) &values[i]);
    }
    
    for (int j = 0; j < numHilos; j++){
        pthread_join(hilos[j], NULL);
    }
    printf("\nTodos los hijos terminaron, yo soy el hilo principal %ld\n", pthread_self());
    printf("El valor %d tuvo %d repeticiones estando en los indices: ", target, repeticiones);
    for (int k = 0; k < repeticiones; k++){
        printf("[%d, hilo: %ld] ", indices[k], hilos_se[k]);
    }

    free(vect);
    free(hilos);
    free(indices);
    free(hilos_se);
    return EXIT_SUCCESS;
}