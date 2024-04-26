#include <stdio.h>  
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char **argv){

    int N = atoi(argv[1]), i = 0;

    pid_t process[N];

    int tubs1[N][2];
    int tubs2[N][2];

    for ( i = 0; i < N; i++){
        pipe(tubs1[i]);
        pipe(tubs2[i]);

        process[i] = fork();
        if (process[i] == -1)exit(EXIT_FAILURE);
        
        if (process[i] != 0)break;    
    }

    if (process[i] != 0 && (i > 0 && i != N)){
        printf("child %d. my father is %d\n", getpid(), getppid());
        int numberIda = 0, numberVuelta = 0;

        close(tubs1[i-1][1]);
        close(tubs2[i-1][0]);
        
        if (i > 1 && i < N){   
            for ( int j = 1; j < i; j++){
                close(tubs1[i-j-1][0]);
                close(tubs2[i-j-1][1]);    
            }
        }

        (read(tubs1[i-1][0], &numberIda, sizeof(int)) != -1) ? printf("proceso %d [%d] leyendo de la tuberia izquierda\n",i, getpid()) : printf("-------proceso %d [%d] no pudo leer en tuberia izquierda-------\n", i, getpid());

        printf("numeros imprimidos por pid: %d %d %d\n",getpid(), numberIda, numberVuelta);

        numberIda++;
        (write(tubs1[i][1], &numberIda, sizeof(int)) != -1) ? printf("proceso %d [%d] escribiendo en la tuberia izquierda\n",i, getpid()) : printf("------proceso %d [%d] no pudo escribir en tuberia izquierda-------\n",i, getpid());
        

        (read(tubs2[i][0], &numberVuelta, sizeof(int)) != -1) ? printf("proceso %d [%d] leyendo de la tuberia derecha\n",i, getpid()) : printf("-------proceso %d [%d] no pudo leer en tuberia derecha------\n",i, getpid());

        numberVuelta++;

        (write(tubs2[i-1][1], &numberVuelta, sizeof(int)) != -1) ? printf("proceso %d [%d] escribiendo de la tuberia derecha\n",i, getpid()) : printf("--------proceso %d [%d] no pudo escribir en tuberia derecha----------\n",i, getpid());

        printf("numeros %d %d\n", numberIda, numberVuelta);
        
    }
    if (process[N-1] == 0){
        int numberIda;

        for ( int j = 0; j < N-1; j++){
            close(tubs1[j][1]);
            close(tubs2[j][0]);
            close(tubs1[j][0]);
            close(tubs2[j][1]);
        }

        close(tubs1[i-1][1]);
        close(tubs2[i-1][0]);

        (read(tubs1[i-1][0], &numberIda, sizeof(int)) != -1) ?  printf("proceso %d [%d] leyendo de la tuberia izquierda\n",i, getpid()) : printf("-------proceso %d [%d] no pudo leer en tuberia izquierda-------\n", i, getpid()); 

        numberIda++;

        (write(tubs2[i-1][1], &numberIda, sizeof(int)) != -1) ? printf("proceso %d [%d] escribiendo de la tuberia derecha\n",i, getpid()) : printf("--------proceso %d [%d] no pudo escribir en tuberia derecha----------\n",i, getpid());

        printf("last child. my father is %d\n", getppid());
        printf("numero recibido por el proceso %d %d\n",i-1, numberIda);
    }

    if (i == 0 && process[i] != 0){
        int numero = 0, numberVuelta; 
        
        close(tubs1[0][0]);
        close(tubs2[0][1]);

        (write(tubs1[i][1], &numero, sizeof(int)) != -1) ? printf("padre escribiendo en la tuberia izquierda\n") : printf("-------proceso padre [%d] no pudo escribir ne tuberia izquierda---------\n", getpid());
        
        (read(tubs2[i][0], &numberVuelta, sizeof(int)) != -1) ? printf("padre ha leido en la tuberia derecha\n") : printf("-------- proceso padre [%d] no pudo leer en tuberia derecha------------\n", getpid());

        numberVuelta++;

        printf("numero de salida %d\n", numero);
        printf("numero de regreso %d\n", numberVuelta);
    }

    return EXIT_SUCCESS;
}