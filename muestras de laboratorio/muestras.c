#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <math.h>

void muestras(char *fileName, int *Nmuestras, int *Cmuestras){
    int numMuestras, cantidadMuestras;
    FILE *file;
    file = fopen(fileName, "r");

    if (file == NULL){
        printf("error al abrir el archivo con nombre: %s\n", fileName);
        exit(EXIT_FAILURE);
    }

    fscanf(file, "%d %d", &cantidadMuestras, &numMuestras);

    *Nmuestras = numMuestras;
    *Cmuestras = cantidadMuestras;   

    fclose(file);
}

void leerMuestras(char *fileName, int ***matriz, int numMuestras, int canMuestras){
    FILE *file;
    file = fopen(fileName, "r");
    int cantidadMuestras, numMues; 

    if (file == NULL){
        printf("error al abrir el archivo con nombre: %s\n", fileName);
        exit(EXIT_FAILURE);
    }

    fscanf(file, "%d %d", &cantidadMuestras, &numMues);

    *matriz = (int **)malloc(canMuestras * sizeof(int *));
    for (int i = 0; i < canMuestras; i++){
        (*matriz)[i] = (int*)malloc(numMuestras * sizeof(int));
        for (int j = 0; j < numMuestras; j++){
            fscanf(file, "%d", &(*matriz)[i][j]);
        }
        
    }
    
    fclose(file);
}

int main(int argc, char *argv[]){

    int numMuestras, cantMuestras, **matriz, i = 0;
    int tub[2];
    
    pipe(tub);

    muestras(argv[1], &numMuestras, &cantMuestras);

    pid_t pids[numMuestras];

    leerMuestras(argv[1], &matriz, numMuestras, cantMuestras);  

    for (i = 0; i < numMuestras; i++){
        pids[i] = fork();

        if (pids[i] == 0){
            break;
        }   
    }

    
    if (pids[i] != 0){
        close(tub[1]);
        float media, varianza;
        double desviacion, coeficiente;
        for (int k = 0; k < numMuestras; k++){
            wait(NULL);
            read(tub[0], &media, sizeof(float));
            read(tub[0], &varianza, sizeof(float));
            read(tub[0], &desviacion, sizeof(double));
            read(tub[0], &coeficiente, sizeof(double));
            printf("\n\nmedia conjunto de muestras %d: %f ",k+1 , media);
            printf("\nvarianza: conjunto de muestras %d %f ",k+1 , varianza);
            printf("\ndesviacion conjunto de muestras %d: %f ",k+1 , desviacion);
            printf("\ncoeficiente conjunto de muestras %d: %f ",k+1 , coeficiente);
        } 
        
    }else{
        close(tub[0]);

        float suma = 0, varianza, sum, value;
        double desviacion, coeficiente;
        for (int k = 0; k < cantMuestras; k++){
            suma +=  matriz[k][i];
        } 
        float media = suma / cantMuestras;
        write(tub[1], &media, sizeof(float));

        for (int k = 0; k < cantMuestras; k++){
            value = matriz[k][i] - media;
            sum += value * value;
        }
        varianza = sum / (cantMuestras - 1);
        write(tub[1], &varianza, sizeof(float));

        desviacion = sqrt(varianza);
        write(tub[1], &desviacion, sizeof(double));

        coeficiente = desviacion / media;
        write(tub[1], &coeficiente, sizeof(double));
    }
    
}