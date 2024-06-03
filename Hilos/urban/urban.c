#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

int **matriz, **aux, numDias, filas, columnas, turno = 0;

int error(char *mensaje){
    printf("%s\n", mensaje);
    exit(EXIT_FAILURE);
}

void leer_imagen(char *fileName){
    FILE *file;
    file = fopen(fileName, "r");
    if (file == NULL) error("no se pudo abir el archivo");

    fscanf(file, "%d %d %d", &numDias, &filas, &columnas);

    matriz = (int **)malloc(filas * sizeof(int *));
    if (matriz == NULL) error("no se puede asignar memoria para las filas");

    aux = (int **)malloc(filas * sizeof(int *));
    if (aux == NULL) error("no se puede asignar memoria para las filas");

    for (int i = 0; i < filas; i++){
        matriz[i] = (int *)malloc(columnas * sizeof(int));
        aux[i] = (int *)malloc(columnas * sizeof(int));
        if (matriz[i] == NULL) error("no se puede asignar memoria para las columnas");
        for (int j = 0; j < columnas; j++){
            fscanf(file, "%d", &matriz[i][j]);
            aux[i][j] = 5;
        }
    }
    fclose(file);
}

int vefificar_vecinos(int i, int j, int v1, int v2){
    int x, y;
    x = (i == 0) ? i + 1: i;
    y = (j == 0) ? j + 1: j;

    if(i + 1 == filas){
        x = i;
        i -= 1;
    }
    if(j + 1 == columnas){
        y = j;
        j -= 1;
    }

    int vecinos = 0;
    for (int a = x - 1; a < i + 2; a++){
        for (int b = y - 1; b < j + 2; b++){
            if ((matriz[a][b] == v1 || matriz[a][b] == v2)) vecinos++;
        }
    }
    return vecinos;
}


void *baja_a_media(void *arg){
    for (int i = 0; i < numDias; i++){
        while (turno != 0);
        printf("soy el hilo %ld que verificara si pasa de baja a media\n", pthread_self());
        int vecinos = 0;
        for (int i = 0; i < filas; i++){
            for (int j = 0; j < columnas; j++){
                if (matriz[i][j] == 0) {
                    vecinos = vefificar_vecinos(i, j, 1, 2);
                }
                if (vecinos >= 4) aux[i][j] = 1;
                vecinos = 0;
            }
        }
        for (int i = 0; i < filas; i++){
            for (int j = 0; j < columnas; j++){
                if (aux[i][j] == 1){
                    matriz[i][j] = aux[i][j];
                    aux[i][j] = 5;
                }
            }
        }
        turno = 1;
    }
    
}
void *media_a_baja(void *arg){
    for (int i = 0; i < numDias; i++){
        float proba = 0.1, media_a_ba = 0, p_v = 0.03;
        while (turno != 1);
        printf("soy el hilo %ld que verificara si pasa de media a baja\n", pthread_self());
        int vecinos = 0;
        for (int i = 0; i < filas; i++){
            for (int j = 0; j < columnas; j++){
                if (matriz[i][j] == 0){
                    vecinos = vefificar_vecinos(i, j, 0, 0);
                }
                media_a_ba = proba + (vecinos * 0.03);
                int random = rand() % 11;
                if (random < media_a_ba)
                    aux[i][j] = 0;
                vecinos = 0;
            }
        }
        for (int i = 0; i < filas; i++){
            for (int j = 0; j < columnas; j++){
                if (aux[i][j] == 0){
                    matriz[i][j] = aux[i][j];
                    aux[i][j] = 5;
                }
            }
        }
        turno = 2;
    }
}
void *media_a_alta(void *arg){
    for (int i = 0; i < numDias; i++){    
        while (turno != 2);
        printf("soy el hilo %ld que verificara si pasa de media a alta\n", pthread_self());
        int vecinos = 0;
        for (int i = 0; i < filas; i++){
            for (int j = 0; j < columnas; j++){
                if (matriz[i][j] == 0){
                    vecinos = vefificar_vecinos(i, j, 2, 2);
                }
                if (vecinos >= 3)
                    aux[i][j] = 2;
                vecinos = 0;
            }
        }
        for (int i = 0; i < filas; i++){
            for (int j = 0; j < columnas; j++){
                if (aux[i][j] == 2){
                    matriz[i][j] = aux[i][j];
                    aux[i][j] = 5;
                }
            }
        }
        turno = 3;
    }
    
}
void *alta_a_media(void *arg){
    for (int i = 0; i < numDias; i++){
        while (turno != 3);
        printf("soy el hilo %ld que verificara si pasa de alta a media\n", pthread_self());
        float proba = 0.15, alta_a_media = 0;
        int vecinos = 0;
        for (int i = 0; i < filas; i++){
            for (int j = 0; j < columnas; j++){
                if (matriz[i][j] == 0){
                    vecinos = vefificar_vecinos(i, j, 0, 0);
                }
                alta_a_media = proba + (vecinos * 0.05);
                int random = rand() % 11;
                if (random < alta_a_media)
                    aux[i][j] = 1;
                vecinos = 0;
            }
        }
        for (int i = 0; i < filas; i++){
            for (int j = 0; j < columnas; j++){
                if (aux[i][j] == 1){
                    matriz[i][j] = aux[i][j];
                    aux[i][j] = 5;
                }
            }
        }
        turno = 4;
    }
    
}

void print_matriz(int ***m, int dia){
    printf("------------- Ciudad despues del dia %d ----------------\n", dia+1);
    for (int i = 0; i < filas; i++){
        for (int j = 0; j < columnas; j++){
            printf("%d ", (*m)[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char **argv){
    int numHilos = 4;

    pthread_t *hilos = (pthread_t *)malloc(numHilos*sizeof(pthread_t));

    leer_imagen(argv[1]);

    for (int i = 0; i < numHilos; i++){
        if (i == 0)
            pthread_create(&hilos[i], NULL, baja_a_media, NULL);
        if (i == 1)
            pthread_create(&hilos[i], NULL, media_a_baja, NULL);
        if (i == 2)
            pthread_create(&hilos[i], NULL, media_a_alta, NULL);
        if (i == 3)
            pthread_create(&hilos[i], NULL, alta_a_media, NULL);
    }

    for (int i = 0; i < numDias; i++){  
        while (turno != 4);
        printf("turno hilo principal\n");
        print_matriz(&matriz, i);
        turno = 0;
    }

    for (int i = 0; i < numHilos; i++){
        pthread_join(hilos[i], NULL);
    }

    printf("Hilos terminados\n");

    for (int i = 0; i < filas; i++){
        free(matriz[i]);
        free(aux[i]);
    }
    free(matriz);
    free(aux);
    
    return EXIT_SUCCESS;
}