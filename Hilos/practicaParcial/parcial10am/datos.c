#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <dirent.h>
#include <string.h>
#include <semaphore.h>

#define MAX_LONGITUD_NOMBRE 256
#define NUM_SIMULTANEOS 2

sem_t sem;

int num_archivos, tam;
char  **nombres;

typedef struct{
    char *nombres;
} NombreFile;

typedef struct{
    long transanccionId;
    float monto;
    char categoria[100];
}UsersDatas;

typedef struct{
    int idUser;
    UsersDatas *datas;
}Datos;

Datos *data;


int error(char *mensaje){
    printf("%s\n", mensaje);
    exit(EXIT_FAILURE);
}

void leer_archivo(char *fileName)
{
    FILE *file = fopen(fileName, "r");
    if (file == NULL)
        error("no se pudo abrir el archivo");

    fscanf(file, "%d\n", &tam);

    Datos *datos = (Datos *)malloc(tam * sizeof(Datos));

    printf("tamaño: %d\n", tam);
    for (int i = 0; i < tam; i++){
        //fscanf(file, "%lg,%f,%c\n", );
    }

    fclose(file);
}

int nombre_archivos_en_direectorio(char *direct_route){
    int pipe_fds[2];
    pipe(pipe_fds);
    DIR *directoro;
    struct dirent *dir;
    char **fileNames = (char **)malloc(num_archivos * sizeof(char*));
    directoro = opendir(direct_route);
    if (directoro == NULL) error("no encontro la ruta");
    int i = 0;
    if (directoro){
        while ((dir = readdir(directoro)) != NULL && i < num_archivos){
            if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
                continue;
            fileNames[i] = malloc(MAX_LONGITUD_NOMBRE);
            strncpy(fileNames[i], dir->d_name, MAX_LONGITUD_NOMBRE + 1);
            i++;
        }
        closedir(directoro);
    }

    int escritura = pipe_fds[1];
    for (int j = 0; j < i; j++){
        printf("%s\n", fileNames[j]);
        char temporal[MAX_LONGITUD_NOMBRE] = {0};
        strncpy(temporal, fileNames[j], MAX_LONGITUD_NOMBRE - 1);
        write(escritura, fileNames[j], MAX_LONGITUD_NOMBRE);
        }
    close(escritura);
    return pipe_fds[0];
}

void *funtion(void *arg){
    //NombreFile *nombre = (NombreFile *) arg;
    //strcat(cadena1, cadena2); concatenar la ruta
    sem_wait(&sem);
    for (int i = 0; i < num_archivos; i++){
        leer_archivo(nombres[i]);
        printf("hilo %ld procesando el archivo: %s\n", pthread_self(), nombres[i]);
    }
    sleep(4);
    sem_post(&sem);
    pthread_exit(0);
}

int main(int argc, char **argv){
    sem_init(&sem, 0, NUM_SIMULTANEOS);

    int num_hilos = 5;
    num_archivos = atoi(argv[1]);

    pthread_t *hilos = (pthread_t  *)malloc(num_hilos*sizeof(pthread_t));

    int lectura =  nombre_archivos_en_direectorio(argv[2]);

    char nombre[MAX_LONGITUD_NOMBRE];
    nombres = (char **)malloc(num_archivos * sizeof(char *));
    for (int i = 0; i < num_archivos; i++){  
        read(lectura, nombre, MAX_LONGITUD_NOMBRE);
        nombre[MAX_LONGITUD_NOMBRE - 1] = '\0';
        printf("nombre archivo: %s\n", nombre);
        nombres[i] = malloc(strlen(nombre) + 1);
        strcpy(nombres[i], nombre);
    }
    
    for (int i = 0; i < num_archivos; i++){
        printf("nombres desde  un vector: %s\n", nombres[i]);
    }
    
    NombreFile *nombreFile = (NombreFile *)malloc(num_hilos * sizeof(NombreFile));
    
    for (int i = 0; i < num_hilos; i++){
        if (num_archivos < i) break;        
        pthread_create(&hilos[i], NULL, funtion, NULL);
    }
    for (int i = 0; i < num_hilos; i++){
        pthread_join(hilos[i], NULL);
    }

    printf("hilo principal\n");

    sem_destroy(&sem);
    return EXIT_SUCCESS;
}