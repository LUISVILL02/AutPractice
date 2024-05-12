#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>

int error(char *mensaje){
    printf("%s\n", mensaje);
    exit(EXIT_FAILURE);
}

void leer_matriz(char *file_name, int ***matriz, int ***matriz2, int *rows, int *cols){
    int columnas, filas;

    FILE *file;
    file = fopen(file_name, "r");
    if (file == NULL) error("error al abrir el archivo");
    
    fscanf(file, "%d", &filas);
    fscanf(file, "%d", &columnas);

    *rows = filas;
    *cols = columnas;

    *matriz = (int **)malloc(filas * sizeof(int *));
    if (matriz == NULL) error("error al asignar memoria");

    for (int i = 0; i < filas; i++){
        (*matriz)[i] = (int *)malloc(columnas * sizeof(int));
        for (int j = 0; j < columnas; j++){
            fscanf(file, "%d", &(*matriz)[i][j]);
        }
    }

    *matriz2 = (int **)malloc(filas * sizeof(int *));
    if (matriz2 == NULL) error("error al asignar memoria");
    for (int i = 0; i < filas; i++){
        (*matriz2)[i] = (int *)malloc(columnas * sizeof(int));
        for (int j = 0; j < columnas; j++){
            fscanf(file, "%d", &(*matriz2)[i][j]);
        }
    }

    for (int g = 0; g < filas; g++){
        for (int h = 0; h < columnas; h++){
            printf("%d ", (*matriz)[g][h]);
        }
        printf("\n");
    }
    printf("\n");
    for (int g = 0; g < filas; g++){
        for (int h = 0; h < columnas; h++){
            printf("%d ", (*matriz2)[g][h]);
        }
        printf("\n");
    }

    
    fclose(file);
    
}

int main(int argc, char **argv){

    int **matriz1, **matriz2, filas, columnas, i = 0;

    leer_matriz(argv[1], &matriz1, &matriz2, &filas, &columnas);

    int tuberia[2];
    pipe(tuberia);

    pid_t hijo;

    for ( i = 0; i < 2; i++){
        hijo = fork();

        if (hijo == -1)error("error al crear el proceso");

        if (hijo == 0)break;
    }
    
    if (i == 0 && hijo == 0){
        close(tuberia[0]);
        int coordenadas[50][2];
        int valores = 0;
        for (int j = 0; j < filas; j++){
            for (int k = 0; k < columnas; k++){
                if (matriz1[j][k] <= 200 && matriz2[j][k] <= 200){
                    if ((matriz1[j][k] - matriz2[j][k]) >= 10){
                        coordenadas[valores][0] = j;
                        coordenadas[valores][1] = k;
                        valores++;
                    }
                    
                }
                
            }
            
        }
        (write(tuberia[1], &valores, sizeof(int)) != -1 ) ? printf(" ") : error("no se pudo escribir"); 
        (write(tuberia[1], coordenadas, sizeof(coordenadas)) != -1 ) ? printf(" ") : error("no se pudo escribir");        
        close(tuberia[1]);

       
        
    }else if(i == 1){
        close(tuberia[0]);
        int coordenadas[50][2];
        int valores = 0;
        for (int j = 0; j < filas; j++){
            for (int k = 0; k < columnas; k++){
                if ((matriz1[j][k] <= 300 && matriz1[j][k] > 200) && (matriz2[j][k] > 200 && matriz2[j][k] <= 300)){
                    if ((matriz1[j][k] - matriz2[j][k]) >= 10){
                        coordenadas[valores][0] = j;
                        coordenadas[valores][1] = k;
                        valores++;
                    }
                    
                }
                
            }
            
        }
        (write(tuberia[1], &valores, sizeof(int)) != -1 ) ? printf(" ") : error("no se pudo escribir"); 
        (write(tuberia[1], coordenadas, sizeof(coordenadas)) != -1 ) ? printf(" ") : error("no se pudo escribir");  
        close(tuberia[1]);

    }else{
        close(tuberia[1]);
        int coordenada[50][2], coordenada2[50][2], valores1, valores2;
        for (int j = 0; j < 2; j++){
            wait(NULL);
        }
        (read(tuberia[0], &valores1, sizeof(int)) != -1) ? printf("\n") : error("no se pudo leer");
        (read(tuberia[0], coordenada, sizeof(coordenada)) != -1) ? printf("\n") : error("no se pudo leer");
        printf("Hubo una disminución de la vegetación en las siguientes coordenadas: \n");
        for (int k = 0; k < valores1; k++){    
            printf("coordenada (%d, %d)\n", coordenada[k][0], coordenada[k][1]);
        }

        (read(tuberia[0], &valores2, sizeof(int)) != -1) ? printf("\n") : error("no se pudo leer");
        (read(tuberia[0], coordenada2, sizeof(coordenada2)) != -1) ? printf("\n") : error("no se pudo leer");
        printf("Hubo una reducción de los niveles de agua en las siguientes coordenadas: \n");
        for (int f = 0; f < valores2; f++){    
            printf("coordenada (%d, %d)\n", coordenada2[f][0], coordenada2[f][1]);
        }
        
        close(tuberia[0]);
    }
    

    for (int m = 0; m < columnas; m++){
        free(matriz1[m]);
        free(matriz2[m]);

    }
    free(matriz1);
    free(matriz2);

    return EXIT_SUCCESS;
}
