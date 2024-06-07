#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

int tam;

typedef struct{
    char fecha[11];
    float temperatura;
    float precision;
} Datos;

Datos *datos;

int error(char *mensaje){
    printf("%s\n", mensaje);
    exit(EXIT_FAILURE);
}

void leer_archivo(char *fileName){
    FILE *file = fopen(fileName, "r");
    if (file == NULL)  error("no se pudo abrir el archivo");

    fscanf(file, "%d\n", &tam);

    datos = (Datos *)malloc(tam * sizeof(Datos));

    printf("tamaño: %d\n", tam);
    char coma;
    for (int i = 0; i < tam; i++){
        fscanf(file, "%10[^,],%f,%f\n", datos[i].fecha, &datos[i].temperatura, &datos[i].precision);
    }
    
    // while (fscanf(file, "%10[^,],%lf,%lf\n", fecha, &temperatura, &precision) == 3){
    //     printf("Fecha: %s, Temperatura: %.2lf, Precisión: %.2lf\n", fecha, temperatura, precision);
    // }

    fclose(file);
}

int main(int arc, char **argv){

    leer_archivo(argv[1]);

    printf("Fecha: %s, Temperatura: %f, Precisión: %f\n", datos[i].fecha, datos[i].temperatura, datos[i].precision);

    return EXIT_SUCCESS;
}