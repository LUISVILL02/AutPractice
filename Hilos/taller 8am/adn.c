#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

pthread_mutex_t mutex;

char **matriz;
int secuencias, bases, estudios;

typedef struct{
    int indiceI;
    int indiceJ;
    int tub[100][2];
    int hilo;
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
    printf("secuencias de hilo %d: (%d, %d) \n", comp->hilo + 1, comp->indiceI, comp->indiceJ);
    sleep(1);
    int i = comp->indiceI;
    int j = comp->indiceJ;
    int hilo = comp->hilo;
    int tubRead = comp->tub[hilo][0];
    int tubWrite = comp->tub[hilo][1];
    Info info[1000];
    int contador = 0;
    for (int a = 0; a < bases; a++){
        if (matriz[i][a] != matriz[j][a]){
            pthread_mutex_lock(&mutex);
            info[contador].caracter1 = matriz[i][a];
            info[contador].caracter2 = matriz[j][a];
            info[contador].pos = a;
            contador++;
            pthread_mutex_unlock(&mutex);
        }
    }
    if(write(comp->tub[hilo][1], &contador, sizeof(int)) == -1) perror("no se pudo escribir\n");
    if (write(comp->tub[hilo][1], &info, contador * sizeof(Info)) == -1) error("no se pudo escribir\n");
    pthread_exit(0);
}

int main(int argc, char **argv){
    pthread_mutex_init(&mutex, NULL);

    leer_archivo(argv[1], &matriz, &secuencias, &bases);

    estudios = secuencias*(secuencias - 1) / 2;
    printf("estudios: %d secuencia: %d\n", estudios, secuencias);
    pthread_t *hilos = (pthread_t*)malloc(estudios * sizeof(pthread_t));

    Comparacion *comp = (Comparacion*)malloc(estudios * sizeof(Comparacion));

    int index = 0;
    for (int i = 0; i < secuencias; i++){
        for (int j = i + 1; j < secuencias; j++){
            if (pipe(comp[index].tub[index]) == -1) error("no se pudieron crear las tuberias");
            comp[index].indiceI = i;
            comp[index].indiceJ = j;
            comp[index].hilo = index;
            pthread_create(&hilos[index], NULL, comparar, (void *) &comp[index]);
            index++;
        }
    }
    for (int i = 0; i < index; i++){
        int c_hijos = 0;
        read(comp[i].tub[i][0], &c_hijos, sizeof(int));
        printf("----------contador hilo %d: %d----------\n", i + 1, c_hijos);
        Info inf[c_hijos];
        read(comp[i].tub[i][0], &inf, c_hijos * sizeof(Info));

        for (int j = 0; j < c_hijos; j++){
            printf("Pos %d: %c %c\n", inf[j].pos, inf[j].caracter1, inf[j].caracter2);
        }
        close(comp[i].tub[i][0]);
        close(comp[i].tub[i][1]);
    }

    for (int i = 0; i < estudios; i++){
        pthread_join(hilos[i], NULL);
    }
    

    for (int i = 0; i < secuencias; i++){
        free(matriz[i]);
    }
    free(hilos);
    free(matriz);
    free(comp);
    pthread_mutex_destroy(&mutex);
    return EXIT_SUCCESS;
}