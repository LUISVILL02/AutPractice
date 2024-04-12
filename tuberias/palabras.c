#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

void leerLineas(char *fileName, char ***vect, int *tamaño){
    int numLine = 0;
    char linea[500];
    char caracter;
    FILE *file;
    file = fopen(fileName, "r");

    if (file == NULL){
        printf("error al abrir el archivo\n");
        exit(EXIT_FAILURE);
    }
    while ((caracter = fgetc(file)) != EOF) {
        if (caracter == '\n')
            numLine++;
    }
    if (caracter != '\n' && numLine != 0){ 
        numLine++;
    }
    *tamaño = numLine;

    rewind(file);

    *vect = (char **) malloc(numLine * sizeof(char*));

    for (int i = 0; i < numLine; i++){
        (*vect)[i] = (char *) malloc(500 * sizeof(char));
        if ((*vect)[i] == NULL) {
            printf("Error al asignar memoria.\n");
            exit(EXIT_FAILURE);
        }
        
        fgets(linea, sizeof(linea), file);    
        strcpy((*vect)[i], linea);
    }
    fclose(file);

    for (int i = 0; i < numLine; i++) {
        free(vect[i]);
    }
    free(vect);
}

void escribirEnFile(char *fileName, int lineas, char *frase){
    FILE *file;
    file = fopen(fileName, "w");

    if (file == NULL){
        printf("error al abrir el archivo\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < lineas; i++){
        fprintf(file, "%s\n", frase);
    }
    fclose(file);   
}

void procesarTuberia(int t[2], int datos, char *palabra, char *fileOut) {
    char **vect = malloc(datos * sizeof(char*));
    if (vect == NULL) {
        fprintf(stderr, "Error: no se pudo asignar memoria para el vector.\n");
        exit(EXIT_FAILURE);
    }

    for (int l = 0; l < datos; l++) {
        int size = 0;
        read(t[0], &size, sizeof(int));

        vect[l] = malloc(size * sizeof(char));
        if (vect[l] == NULL) {
            fprintf(stderr, "Error: no se pudo asignar memoria para la cadena.\n");
            exit(EXIT_FAILURE);
        }

        read(t[0], vect[l], size);

        if (strstr(vect[l], palabra) != NULL) {
            escribirEnFile(fileOut, 1, vect[l]);
        }
    }
    for (int i = 0; i < datos; i++) {
        free(vect[i]);
    }
    free(vect);
}

int main(int argc, char *argv[]){
    int i = 0,n;
    int t1[2];
    int t2[2];

    pid_t sons[3];

    pipe(t1);
    pipe(t2);

    for (i = 0; i < 3; i++){
        sons[i] = fork();
        if (sons[i] == 0){
            break;
        }
    }
    if (i == 0 && sons[i] == 0){
        char **vect;
        int tama = 0;
        close(t1[0]);
        close(t2[0]);
        leerLineas(argv[1], &vect, &tama);

        write(t1[1], &tama, sizeof(int));
        write(t2[1], &tama, sizeof(int));
        
        for (int j = 0; j < tama; j++){
            int size =  strlen(vect[j]) +1;
            if (j%2 == 0){   
                write(t1[1], &size, sizeof(int));  
                write(t1[1], vect[j], size);
            }else{
                write(t2[1], &size, sizeof(int));  
                write(t2[1], vect[j], size);
            }    
        }
        printf("Acabo de leer los datos del archivo y los envie por las tuberias. Soy el hijo %d\n",i+1);
    }
    
    if (i == 1 && sons[i] == 0){
        int datos;
        close(t1[1]);
        close(t2[0]);
        close(t2[1]);
        read(t1[0], &datos, sizeof(int));
        
        procesarTuberia(t1, datos, argv[2], "uno.txt");

        printf("Ya lei los datos de la primera tuberia. Soy el hijo %d\n",i+1);
    }
    if (i == 2 && sons[i] == 0){
        int datos = 0;
        close(t1[1]);
        close(t1[0]);
        close(t2[1]);

        read(t2[0], &datos, sizeof(int));

        procesarTuberia(t2, datos, argv[3], "dos.txt");

        printf("Ya lei los datos de la segunda tuberia. Soy el hijo %d\n",i+1);
    }
    if (sons[i] != 0){
        close(t1[1]);
        close(t1[0]);
        close(t2[1]);
        close(t2[0]);
        for (int k = 0; k < 3; k++){
            wait(NULL);
        }
        printf("Envio de datos verificado por %d\n", getpid());
        
    }
    
    return 0;
}