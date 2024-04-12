#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

int vuelta = 0;

void handleSignal(int signal){
    printf("señal %d recibida\n", signal);   
}

void writeLog(char *log, char ***message, int tamaño){
    FILE *file;
    file = fopen(log, "w");

    if (file == NULL){
        printf("error al abrir el archivo");
        exit(1);
    }
    fprintf(file, "%d\n", tamaño);

    *message = (char **) malloc(tamaño * sizeof(char*));
    for (int i = 0; i < tamaño; i++){
        (*message)[i] = (char *) malloc(tamaño * sizeof(char));
        sprintf((*message)[i], "aviso recibido numero: %d", vuelta+1);
        fprintf(file, "%s\n", (*message)[i]);    
        vuelta++; 
    }
    fclose(file);

    for (int i = 0; i < tamaño; i++) {
        free((*message)[i]);
    }
    free(*message);
}

void readLog(char *log, char ***messages, int *tam){
    int tamaño;
    FILE *file;
    file = fopen(log, "r");

    if (file == NULL){
        printf("error al abrir el archivo");
        exit(1);
    }
    fscanf(file, "%d", &tamaño);
    *tam = tamaño;

    char linea[200];

    fgets(linea, sizeof(linea), file); 

    *messages = (char **) malloc(tamaño * sizeof(char*));
    for (int i = 0; i < tamaño; i++){
        (*messages)[i] = (char *) malloc(200 * sizeof(char));
        fgets(linea, sizeof(linea), file);    
        strcpy((*messages)[i], linea);
    }
    fclose(file);
}

int main(int argc, char *argv[]){
    int i = 0;
    pid_t pid[2];
    int tamaño;
    
    char **messageSend;
    char **message;

    for (i = 0; i < 2; i++){
        pid[i] = fork();
        if (pid[i] == 0 )
            break;   
    }
    if (i == 0 && pid[i] == 0 ){
        int j = 0; 
        printf("soy registrador con pid: %d\n", getpid());
        
        signal(SIGUSR1, handleSignal);
        for (j = 0; j < 5; j++){
            pause();
            printf("aviso recibido\n\n"); 
        }  
        writeLog(argv[1], &messageSend, j);
    }
    if (i == 1 && pid[i] == 0){
        printf("soy avisasor con pid: %d\n\n", getpid());
        sleep(1);
        for (int j = 0; j < 5; j++){ 
            printf("mensaje enviado a %d\n", pid[i-1]);
            kill(pid[i-1], SIGUSR1);
            sleep(5);
        }
        kill(pid[i-1], SIGKILL);
    }
    if (pid[i] != 0 ){
        for (int k = 0; k < 2; k++){
            wait(NULL);
        }
        readLog(argv[1], &message, &tamaño);
        printf("padre y señales: %d %d\n", getpid(), tamaño);
        for (int l = 0; l < tamaño; l++){
            printf("%s\n", message[l]);
        }  
        for (int i = 0; i < tamaño; i++) {
            free(message[i]);
        }
        free(message);
    }
    
}