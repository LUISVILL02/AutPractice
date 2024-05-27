#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

int aVeces = 0;
int tVeces = 0;
int cVeces = 0;
int gVeces = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

char secuencia[] = "gcgtga";
char *arreglo;

struct Respuesta {
    pthread_t tid;
    int positions[100];
    int count;
};

struct Nodo {
    int inicio;
    int fin;
    struct Respuesta *respuesta;
};

void *buscarSecuencia(void *arg) {
    struct Nodo *nodo = (struct Nodo *) arg;
    nodo->respuesta->count = 0;

    for (int i = nodo->inicio; i < nodo->fin; i++) {
        if (arreglo[i] == 'g') {
            int j = i;
            int aux = 0;
            while (arreglo[j] == secuencia[aux] && aux < 6) {
                j++;
                aux++;
            }
            if (aux == 6) { 
                pthread_mutex_lock(&mutex);
                if (nodo->respuesta->count < 100) {
                    nodo->respuesta->positions[nodo->respuesta->count++] = i;
                }
                pthread_mutex_unlock(&mutex);
            }
        }
    }
    return NULL;
}

void *buscarBases(void *arg) {
    struct Nodo *nodo = (struct Nodo *) arg;
    for (int i = nodo->inicio; i < nodo->fin; i++) {
        pthread_mutex_lock(&mutex);
        if (arreglo[i] == 'a')
            aVeces++;
        else if (arreglo[i] == 't')
            tVeces++;
        else if (arreglo[i] == 'c')
            cVeces++;
        else if (arreglo[i] == 'g')
            gVeces++;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(int argc, char *argv[]) {

    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Error");
        return 1;
    }

    int nHilos = 5;
    struct Nodo nodos[nHilos];
    struct Respuesta respuestas[nHilos];
    pthread_t tids[nHilos];

    int size;
    fscanf(file, "%d\n", &size);
    arreglo = (char *) malloc(size * sizeof(char));

    for (int i = 0; i < size; i++) {
        fscanf(file, " %c", &arreglo[i]);
    }
    fclose(file);

    int rangoXHilo = size / (nHilos - 1);

    nodos[0].inicio = 0;
    nodos[0].fin = size;
    pthread_create(&tids[0], NULL, buscarBases, &nodos[0]);

    for (int i = 1; i < nHilos; i++) {
        nodos[i].inicio = (i - 1) * rangoXHilo;
        nodos[i].fin = (i == nHilos - 1) ? size : i * rangoXHilo;
        nodos[i].respuesta = &respuestas[i];
        respuestas[i].tid = tids[i];
        pthread_create(&tids[i], NULL, buscarSecuencia, &nodos[i]);
    }

    for (int i = 0; i < nHilos; i++) {
        pthread_join(tids[i], NULL);
    }

    free(arreglo);

    printf("\nLa base A esta presente: %d veces", aVeces);
    printf("\nLa base T esta presente: %d veces", tVeces);
    printf("\nLa base C esta presente: %d veces", cVeces);
    printf("\nLa base G esta presente: %d veces\n\n", gVeces);

    for (int i = 1; i < nHilos; i++) {
        printf("Thread ID: %lu posiciones encontradas: ", respuestas[i].tid);
        for (int j = 0; j < respuestas[i].count; j++) {
            printf("%d ", respuestas[i].positions[j]);
        }
        printf("\n");
    }



    pthread_mutex_destroy(&mutex);

    return 0;
}
