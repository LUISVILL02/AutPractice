#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <math.h>

bool esPrimo(int numero) {
    if (numero <= 1) return false;

    for (int i = 2; i * i <= numero; i++) {
        if (numero % i == 0) return false;
    }
    return true;
}

void hanleSignal(int signal){
    printf("señal recibida\n");
}

int main(int argc, char *argv[]){
    int i = 0, j = 0;
    int semilla = atoi(argv[1]),
    p = atoi(argv[2]), q = atoi(argv[3]), 
    numNumbers = atoi(argv[4]), 
    numProcess = atoi(argv[5]);


    int tub[2];
    pipe(tub);

    int secuencia = numNumbers / numProcess;
    int M = p * q;

    if (!esPrimo(p) || !esPrimo(q)){
        printf("p o q no son numeros primos\n");
        exit(EXIT_FAILURE);
    }
    if(numNumbers % numProcess != 0) {
        printf("la cantidad de numeros debe ser divisible entre el numero de procesos\nPor ejemplo 30 / 5 = 6 un numero entero. En tu caso el resultado de %d / %d da %f\n", numNumbers, numProcess, (float)numNumbers / numProcess);
        exit(EXIT_FAILURE);
    }
    
    pid_t pids[numProcess];

    for (i = 0; i < numProcess; i++){
        pids[i] = fork();
        if (pids[i] == 0) break;   
    }
    
    if (pids[i] == 0){
        signal(SIGUSR1, hanleSignal);

        int numberGenerate = 0, numGenAnte;
        int star = i * secuencia;
        int end = star + secuencia - 1;

        if(i != numProcess-1){
            pause();
            read(tub[0], &numberGenerate, sizeof(int));
        }else 
            sleep(1);
        
        (i > 0) ? kill(pids[i-1], SIGUSR1) : printf("\n");

        printf("hijo %d\n", i + 1);
        
        for ( j = end; j >= star; j--){
            numGenAnte = numberGenerate;
            printf("number generate1: %d\n", numberGenerate);
            printf("number anterior: %d\n", numGenAnte);
            printf("--------------------\n");
            if (j == numNumbers - 1){
                numGenAnte = semilla;
            }
            numberGenerate = (int) pow(numGenAnte, 2) % M;
            printf("inicio %d fin %d\n", end, star);
            printf("hola soy hijo %d y me corresponde el numero generado: %d\n", i + 1, numberGenerate);
        }
        printf("number generate2: %d\n", numberGenerate);
        write(tub[1], &numberGenerate, sizeof(int));
    }else{
        close(tub[0]);
        close(tub[1]);
        for (int j = 0; j < i; j++){
            wait(NULL);
        } 
    }
    
    return 0;
}