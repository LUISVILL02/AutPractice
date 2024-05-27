#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <stdbool.h>

typedef struct snodo{
    char *part;
    struct snodo *sig;
}ListaPartes;

typedef struct nodo{
    char marca[100];
    bool chasis;
    bool motor;
    bool trasmision;
    bool sistema_supension;
    bool sistema_frenos;
}Automovil;

typedef struct nodo3{
    Automovil automovil;
    struct nodo3 *sig;
}ListaAutos;


int error(char *mesagge){
    printf("%s\n", mesagge);
    exit(EXIT_FAILURE);
}

void leer_partes(char *fileName, ListaPartes **lista,  int *tamañoLista){
    char namePart[100];
    int partes;
    FILE *file;
    file = fopen(fileName, "r");

    if(!file) error("no se pudo abrir el archivo");

    fscanf(file, "%d", &partes);

    *tamañoLista = partes;

    *lista = malloc(sizeof(ListaPartes));

    if (!*lista) error("no se pudo signar memoria a la lista");

    ListaPartes *primer = *lista;
    

    for (int i = 0; i < partes; i++){
        fscanf(file, "%s", namePart);
        primer->part = strdup(namePart);
        if (i < partes - 1) {
            primer->sig = malloc(sizeof(ListaPartes));
            if (!primer->sig) error("Error al asignar memoria para el siguiente nodo de la lista");
            primer = primer->sig;
        } else {
            primer->sig = NULL;
        }
    }
    primer = *lista;
    while(primer != NULL){
        printf("%s\n", primer->part);
        primer = primer->sig;
    }
}

void imprimir_lista_autos(ListaAutos **listaAutos){
    ListaAutos *primer = *listaAutos;

    while (primer != NULL){
        printf("%s\n", primer->automovil.marca);
        primer = primer->sig;
    }
    
}

void llenar_autos(Automovil *automovil, ListaAutos **listAutos){
    ListaAutos *nuevo; 
    nuevo = malloc(sizeof(ListaAutos));
    nuevo->automovil = *automovil;
    nuevo->sig = *listAutos;
    *listAutos = nuevo;
}

int main(int argc, char **argv){

    int tamañoList, i = 0;
    pid_t hijos[3];

    ListaPartes *lista;
    ListaAutos *listaAutos;
    Automovil automovil1;
    

    int idShm1 = shmget(IPC_PRIVATE, tamañoList * sizeof(ListaPartes), IPC_CREAT | 0666);
    int idShm2 = shmget(IPC_PRIVATE, tamañoList * sizeof(ListaAutos), IPC_CREAT | 0666);

    ListaPartes *segmento1 = shmat(idShm1, NULL, 0);
    ListaAutos *segmento2 = shmat(idShm2, NULL, 0);

    for (i = 0; i < 3; i++){
        hijos[i] = fork();
        if (hijos[i] == 0) break;
    }

    if (i == 3){
        printf("padre\n");
        leer_partes(argv[1],  &lista, &tamañoList);
        llenar_autos(&automovil1, &listaAutos);
        segmento1.part = lista->part;
        segmento2->automovil = listaAutos->automovil;
        for (int a = 0; a < 3; a++){
            wait(NULL);
        }
    }

    if (i == 2){
        printf("voy a ensamblar\n");
        ListaAutos *primer = *segmento2;
        ListaPartes *partes = segmento1;
        while (primer != NULL){
            (strcmp(partes->part, "Chasis") == 0) ? primer->automovil.chasis = true : printf("pieza no disponible\n");
            primer->automovil.motor = true;
            primer->automovil.trasmision = true;
            primer->automovil.sistema_frenos = true;
            primer->automovil.sistema_supension = true;
            primer = primer->sig;
        }

        *segmento2 = *primer;
        
    }
    if (i == 1){
        printf("voy a pintar\n");
    }
    if (i == 0){
        printf("voy a validar la calidad\n");
    }
    
    
    
    
    
    return EXIT_SUCCESS;
}