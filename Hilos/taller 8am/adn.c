#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

pthread_mutex_t mutex;

char **matriz;
int secuencias, bases, estudios, diferencias = 0;

typedef struct{
    int indiceI;
    int indiceJ;
    int tub[2];
} Comparacion;

typedef struct{
    char caracter1;
    char caracter2;
    int pos;
}Info;


int error(char  *mensaje){
    printf("%s\n", mensaje);
    exit(EXIT_FAILURE);
}

void leer_archivo(char *fileName, char ***mat, int *secuencias, int *bases){
    FILE *file;
    file = fopen(fileName, "r");
    if (file == NULL) error("no se pudo abrir el archivo");

    fscanf(file, "%d %d", secuencias, bases);

    *mat = (char **)malloc(*secuencias * sizeof(char *));
    if (*mat == NULL) error("no se puede asignar memoria para las filas");

    for (int i = 0; i < *secuencias; i++) {
        (*mat)[i] = (char *)malloc((*bases) * sizeof(char));
        if ((*mat)[i] == NULL) error("no se puede asignar memoria para las columnas");
        fscanf(file, "%s", (*mat)[i]);
    }
    fclose(file);
}

void imprimir_secuencias(char ***m, int secuencias, int bases){
    for (int i = 0; i < secuencias; i++){
        for (int j = 0; j < bases; j++){
            printf("%c", (*m)[i][j]);
        }
        printf("\n");
    }
}
void *comparar(void *arg){

    Comparacion *comp = (Comparacion*) arg;
    printf("secuencias: (%d, %d) ", comp->indiceI, comp->indiceJ);
    int i = comp->indiceI;
    int j = comp->indiceJ;
    int tubRead = comp->tub[0];
    int tubWrite = comp->tub[1];

    Info info[1000];

    int contador = 0;

    for (int a = 0; a < bases; a++){
        if (matriz[i][a] != matriz[j][a]){
            //printf("%c y %c indice: %d\n", matriz[i][a], matriz[j][a], a);
            info[contador].caracter1 = matriz[i][a];
            info[contador].caracter2 = matriz[j][a];
            info[contador].pos = a;
            contador++;
        }
    }

     //write(tubWrite, info, contador * sizeof(Info));

    for (int a = 0; a < contador; a++){
        printf("\n desde hilo %d Pos %d: %c %c\n", i, info[i].pos, info[i].caracter1, info[i].caracter2);
    }

    pthread_mutex_lock(&mutex);
    diferencias += contador;
    pthread_mutex_unlock(&mutex);

    pthread_exit(0);
}

int main(int argc, char **argv){
    pthread_mutex_init(&mutex, NULL);

    leer_archivo(argv[1], &matriz, &secuencias, &bases);

    estudios = secuencias*(secuencias - 1) / 2;
    printf("estudios: %d secuencia: %d\n", estudios, secuencias);
    pthread_t *hilos = (pthread_t*)malloc(estudios * sizeof(pthread_t));

    Comparacion *comp = (Comparacion*)malloc(estudios * sizeof(Comparacion));

    int pipes[estudios][2];
    int index = 0;
    for (int i = 0; i < secuencias; i++){
        for (int j = i + 1; j < secuencias; j++){
            pipe(pipes[i]);
            comp[index].indiceI = i;
            comp[index].indiceJ = j;
            pthread_create(&hilos[index], NULL, comparar, (void *) &comp[index]);
            index++;
        }
    }
    Info inf[1000];

    for (int i = 0; i < estudios; i++)
        pthread_join(hilos[i], NULL);

    for (int i = 0; i < index; i++){
        // close(pipes[i][1]);
        // Info info[1000];
        // int nbytes = read(pipes[i][0], info, sizeof(info));
        // int num_elements = nbytes / sizeof(Info);

        // for (int j = 0; j < num_elements; j++) {
        //     printf("Desde hilo %d, Pos %d: %c %c\n", comp[i].indiceI, info[j].pos, info[j].caracter1, info[j].caracter2);
        // }

        close(pipes[i][0]);
    }

    close(comp->tub[1]);

    for (int i = 0; i < secuencias; i++){
        free(matriz[i]);
    }
    free(hilos);
    free(matriz);
    free(comp);

    pthread_mutex_destroy(&mutex);
    return EXIT_SUCCESS;
}