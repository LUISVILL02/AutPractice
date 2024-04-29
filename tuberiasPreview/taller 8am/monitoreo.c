#include <stdio.h>  
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <math.h>

int error(char *mensaje){
    printf("%s\n", mensaje);
    exit(EXIT_FAILURE);
}

void leer_matriz(char *file_name, int ***matriz, int *rows, int *cols){
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
    
    fclose(file);
    
}

int main(int argc, char **argv){

    int **matriz, filas, columnas, i = 0, num_hijos = atoi(argv[2]);
    int start_r, end_r, start_c, end_c;

    leer_matriz(argv[1], &matriz, &filas, &columnas);

    int tub[2];
    pipe(tub);


    pid_t hijo;

    for ( i = 0; i < num_hijos; i++){
        hijo = fork();
        if (hijo == -1) error("no se pudo crear el proceso");
        if (hijo == 0)break;
    }
    
    start_r = (i < 2) ? 0 : filas / 2;
    end_r = (i < 2) ? filas / 2 - 1 : filas - 1;

    start_c = (i % 2 == 0) ? 0 : columnas / 2;
    end_c = (i % 2 == 0) ? columnas / 2 - 1 : columnas - 1;

    if (hijo == 0){
        close(tub[0]);
        int coordenadas[50][2];
        int posible_nidos = 0;
        for (int k = start_r; k <= end_r; k++){
            for (int j = start_c; j <= end_c; j++){
                if (matriz[k][j] == 1 ){
                    coordenadas[posible_nidos][0] = k;
                    coordenadas[posible_nidos][1] = j;   
                    posible_nidos++;
                    
                    printf("posicion de un posible nido en hijo %d: %d %d\n",i+1 , k, j); 
                }  
            }
        }
        
        (write(tub[1], &posible_nidos, sizeof(int)) != -1) ? printf(" ") : error("no se pudo escribir el posible nido");
        (write(tub[1], coordenadas, sizeof(coordenadas)) != -1) ? printf(" ") : error("no se pudo escribir el vector");
        
    }else{
        close(tub[1]);

        int posibles_nidos, inI, inJ;
        int coordenada[50][2];

        for (int l = 0; l < num_hijos; l++){
            wait(NULL);

            (read(tub[0], &posibles_nidos, sizeof(int)) != -1) ? printf(" ") : error("no se pudo leer");
            (read(tub[0], coordenada, sizeof(coordenada)) != -1) ? printf(" ") : error("no se pudo leer");
            for (int k = 0; k < posibles_nidos; k++){    
                printf("coordenada (%d, %d)\n", coordenada[k][0], coordenada[k][1]);
            }
        }

        

        

        printf("soy padre\n");
        // for (int k = 0; k < filas; k++){
        //     for (int j = 0; j < columnas; j++){
                
        //     }
        // }
        
    }
    
    

    return EXIT_SUCCESS;
}