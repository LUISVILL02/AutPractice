#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void error(char *msg){
    perror(msg);
    exit(-1);
}

int leerNumeros(char *fileName, int **vect){
    int c, numero, totalNumeros;

    FILE *file;
    file = fopen(fileName, "r");
    if (!file) error("file error");

    fscanf(file, "%d", &totalNumeros);

    *vect = (int*) malloc(totalNumeros * sizeof(int));
    if (!*vect) error("error malloc");

    for (c = 0; c < totalNumeros; c++){
        fscanf(file, "%d", &numero);
        (*vect)[c] = numero;
    }
    fclose(file);
    return c;
}

void sumar(int tub1[2], int tub2[2], int **vect, int start, int end){
        close(tub1[0]);
        close(tub2[0]);
        close(tub2[1]);
        int suma = 0;

        for ( int j = start; j < end; j++){
            suma += (*vect)[j];
        }
        printf(": %d\n", suma);
        write(tub1[1], &suma, sizeof(int));

        close(tub1[1]);
}

int main(int argc, char *argv[]){
    int *vector, tub1[2], tub2[2], i, start, end;

    pid_t pids[2];

    int canNume = leerNumeros(argv[1], &vector);
    
    int delta = canNume / 2;

    pipe(tub1);
    pipe(tub2);

    for ( i = 0; i < 2; i++){
        pids[i] = fork();
        if (pids[i] == 0) break;
        if (pids[i] == -1) error("no se pudo crear el proceso");
        
    }
    int suma = 0;
    start = delta * i;
    end = start + delta - 1;

    if (i == 0 && pids[i] == 0){
        printf("suma desde hijo %d ", i+1);
        sumar(tub1, tub2, &vector, start, end);
    }

    if (i == 1 && pids[i] == 0){ 
        printf("suma desde hijo %d ", i+1);
        sumar(tub2, tub1, &vector, start, end);
    }
    
    if (pids[i] != 0){
        wait(NULL);
        wait(NULL);

        close(tub1[1]);
        close(tub2[1]);

        int sumaParcial1, sumaParcial2, sumaTotal;

        read(tub1[0], &sumaParcial1, sizeof(int));
        read(tub2[0], &sumaParcial2, sizeof(int));

        sumaTotal = sumaParcial1 + sumaParcial2;
        printf("la suma total es: %d\n", sumaTotal);

        close(tub1[0]);
        close(tub2[0]);
    }
    free(vector);
    return 0;
}