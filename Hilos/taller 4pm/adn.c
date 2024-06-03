#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

char *cadena, *comple;
int tamaño;

typedef struct{
    int indice_inicio;
    int indice_fin;
} Indice;

int error(char  *mensaje){
    printf("%s\n", mensaje);
    exit(EXIT_FAILURE);
}

void leer_archivo(char *fileName){
    FILE *file;
    file = fopen(fileName, "r");
    if (file == NULL) error("no se pudo abrir el archivo");

    fscanf(file, "%d\n", &tamaño);

    printf("tamaño: %d\n", tamaño);

    cadena = (char *)malloc((tamaño) * sizeof(char ));
    if (cadena == NULL) error("no se puede asignar memoria para la cadena");

    for (int i = 0; i < tamaño; i++){
        if((i % 60) == 0){
            fscanf(file, "\n");
        }
        fscanf(file, "%c", &cadena[i]);
    }
    fclose(file);
}

void *complemento(void *arg){
    Indice *ind = (Indice *) arg;

    printf("\nComplemento calculado por el hilo:  %ld en el intervalo: (%d, %d)\n", pthread_self(), ind->indice_inicio, ind->indice_fin);

    for (int i = ind->indice_inicio; i < ind->indice_fin; i++){
        if (cadena[i] == 'a'){
            comple[i] = 't';
        }else if (cadena[i] == 'c'){
            comple[i] = 'g';
        }else if (cadena[i] == 't'){
            comple[i] = 'a';
        }else if (cadena[i] == 'g'){
            comple[i] = 'c';
        }   
    }
    pthread_exit(0);
}

void escribir_complemento(char *fileName){
    FILE *file;
    file = fopen(fileName, "w");
    if (file == NULL) error("no se pudo abrir el archivo");

    fprintf(file, "%d\n", tamaño);

    int c = 0;
    for (int i = 0; i < tamaño; i++){
        if (c != 59){           
            fprintf(file, "%c", cadena[i]);
            c++;
        }else{
            c = 0;
            fprintf(file, "\n");
        }
    }

    fprintf(file, "\n\n");

    int f = 0;
    for (int i = 0; i < tamaño; i++){
        if (f != 59){           
            fprintf(file, "%c", comple[i]);
            f++;
        }else{
            f = 0;
            fprintf(file, "\n");
        }
    }

    fclose(file);
}

int main(int argc, char **argv){
    int numHilos = atoi(argv[2]);

    pthread_t *hilos = (pthread_t*)malloc(numHilos * sizeof(pthread_t));

    leer_archivo(argv[1]);

    Indice *index = (Indice *)malloc(numHilos*sizeof(Indice));
    int delta = tamaño / numHilos;

    comple = (char *)malloc((tamaño) * sizeof(char ));
    if (comple == NULL) error("no se puede asignar memoria para el arreglo complemnto");

    for (int i = 0; i < numHilos; i++){
        index[i].indice_inicio = i * delta;
        index[i].indice_fin = index[i].indice_inicio + delta;

        if (i == numHilos - 1) index[i].indice_fin = tamaño;
        pthread_create(&hilos[i], NULL, complemento, (void *)&index[i]);
    }

    for (int j = 0; j < numHilos; j++){
        pthread_join(hilos[j], NULL);
    }

    escribir_complemento(argv[3]);

    printf("Cadena complemento\n");
    for (int i = 0; i < tamaño; i++){
        printf("%c", comple[i]);
    }

    free(comple);
    free(cadena);
    free(hilos);
    free(index);

    return EXIT_SUCCESS;
}