#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

char *cadena, *fileName_escritura; 
int tamaño, N_a = 0, N_t = 0, N_g = 0, N_c = 0, cont = 0, turno = 1;
int *numSecuencias;

pthread_mutex_t mutex;

typedef struct{
    int indice_inicio;
    int indice_fin;
    int hilo;
} Indice;

typedef struct{
    int pos_init;
    int pos_finist;
    pthread_t h;
} Info;

Info *info;

int error(char *mensaje){
    printf("%s\n", mensaje);
    exit(EXIT_FAILURE);
}

void leer_archivo(char *fileName){
    FILE *file;
    file = fopen(fileName, "r");
    if (file == NULL) error("no se pudo abrir el archivo");

    fscanf(file, "%d\n", &tamaño);

    printf("tamaño: %d\n", tamaño);

    cadena = (char *)malloc((tamaño) * sizeof(char));
    if (cadena == NULL) error("no se puede asignar memoria para la cadena");

    for (int i = 0; i < tamaño; i++){
        if ((i % 60) == 0){
            fscanf(file, "\n");
        }
        fscanf(file, "%c", &cadena[i]);
    }
    fclose(file);
}


void imprimir_cadena(){
    int c = 0;
    for (int i = 0; i < tamaño; i++){
        if (c != 59){
            printf("%c", cadena[i]);
            c++;
        }else{
            c = 0;
            printf("\n");
        }
    }
}
void *estadisticas(void *arg){
    for (int i = 0; i < tamaño; i++){
        if (cadena[i] == 'a') N_a++;
        if (cadena[i] == 't') N_t++;
        if (cadena[i] == 'g') N_g++;
        if (cadena[i] == 'c') N_c++; 
    }
    pthread_exit(0);
}
void *secuencia(void *arg){
    Indice *indice = (Indice *) arg;
    int inicio = indice->indice_inicio;
    int fin = indice->indice_fin;
    int hilo = indice->hilo;
    printf("secuencia por hilo: %d desde [%d, %d]\n", hilo, inicio, fin);
    while(turno != hilo);
    int s_local = 0;
    for (int i = inicio; i < fin; i++){
        if (cadena[i] == 'g' && cadena[i + 1] == 'c' && cadena[i + 2] == 'g' && cadena[i + 3] == 't' && cadena[i + 4] == 'g' && cadena[i + 5] == 'a'){
            info[cont].pos_init = i;
            info[cont].pos_finist = i + 5;
            info[cont].h = pthread_self();
            s_local++;
            cont++;
        }
    }
    numSecuencias[hilo] = s_local;
    turno += 1;
    pthread_exit(0);
}

int main(int argc, char **argv){
    pthread_mutex_init(&mutex, NULL);

    int numHilos = atoi(argv[2]);

    numSecuencias = (int *)malloc((numHilos - 1) * sizeof(int));

    fileName_escritura = argv[3];

    pthread_t *hilos = (pthread_t *)malloc(numHilos * sizeof(pthread_t));
    Indice *indice = (Indice *)malloc(numHilos * sizeof(Indice));

    if (hilos == NULL || indice == NULL) error("no se puedo asignar memoria");
    

    leer_archivo(argv[1]);

    info = (Info *)malloc(tamaño * sizeof(Info));
    if (info == NULL) error("No se pudo asignar memoria a la informacion");
    

    if ((tamaño % (numHilos - 1)) != 0) error("El numero de hilos debe ser multiplo del tamaño de la cadena");

    int delta = tamaño / (numHilos -1);

    pthread_create(&hilos[0], NULL, estadisticas, NULL);

    for (int i = 0; i < numHilos - 1; i++){
        indice[i+1].indice_inicio = i * delta;
        indice[i+1].indice_fin = indice[i+1].indice_inicio + delta;
        indice[i+1].hilo = i+1;
        if (i == numHilos - 1) indice[i+1].indice_fin = tamaño;
        pthread_create(&hilos[i+1], NULL, secuencia, (void *)&indice[i+1]);
    }

    for (int i = 0; i < numHilos; i++){
        pthread_join(hilos[i], NULL);
    }

    printf("-----hilo principal---------\n");

    printf("Numer de secuencias: %d\n", cont);

    FILE *file = fopen(fileName_escritura, "w");
    if (file == NULL) error("No se pudo abrir el archivo para escribir");

    fprintf(file, "%s", "Estadisticas:\n");
    fprintf(file, "%c: %d veces\n", 'A', N_a);
    fprintf(file, "%c: %d veces\n", 'T', N_t);
    fprintf(file, "%c: %d veces\n", 'G', N_g);
    fprintf(file, "%c: %d veces\n", 'C', N_c);

    fprintf(file, "%s", "Secuencia:\n");
    for (int i = 1; i < numHilos; i++){
        (numSecuencias[i] == 0) ? printf(" ") : fprintf(file, "Hilo: [%ld]\n", hilos[i]);
        printf("num secuencias por hilo: %d\n", numSecuencias[i]);
        for (int j = 0; j < cont; j++){
            if(hilos[i] == info[j].h)
                fprintf(file, "Secuencia en posicion: [%d hasta %d]\n", info[j].pos_init, info[j].pos_finist);
        }
        printf("\n");
    }

    fclose(file);

    return EXIT_SUCCESS;
}