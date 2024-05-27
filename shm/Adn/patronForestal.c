#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


typedef struct{
    int **patron;
    int rows;
    int cols;
}patron;


typedef struct{
    int **m;
    int rows;
    int cols;
    patron *p;
    int nThread;
    int id;
    int fMin;
    int fMax;
}vegetacion;


int r[3][3];

int main(int argc, char** argv) {


    
    patron p;
    FILE *file;
    vegetacion veg;

    file = fopen("patron.txt", "r");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        return 1;
    }

    // Leer las dimensiones del patrón
    fscanf(file, "%d", &p.rows);
    fscanf(file, "%d", &p.cols);

    // Crear una matriz para almacenar el patrón
    p.patron = (int **)malloc(p.rows * sizeof(int *));
    for (int i = 0; i < p.rows; i++) {
        p.patron[i] = (int *)malloc(p.cols * sizeof(int));
    }

    // Leer los valores del patrón
    for (int i = 0; i < p.rows; i++) {
        for (int j = 0; j < p.cols; j++) {
            fscanf(file, "%d", &p.patron[i][j]);
        }
    }

    veg.p = &p;
    
    printf("Patrón:\n");
    for (int i = 0; i < veg.p->rows; i++) {
        for (int j = 0; j < veg.p->cols; j++) {
            printf("%d ", veg.p->patron[i][j]);
        }
        printf("\n");
    }

    fclose(file);

    

    file = fopen("matriz_vegetacion.txt", "r");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        return 1;
    }

    // Leer las dimensiones del patrón
    fscanf(file, "%d", &veg.rows);
    fscanf(file, "%d", &veg.cols);

    // Crear una matriz para almacenar el patrón
    veg.m = (int **)malloc(veg.rows * sizeof(int *));
    for (int i = 0; i < veg.rows; i++) {
        veg.m[i] = (int *)malloc(veg.cols * sizeof(int));
    }

    // Leer los valores del patrón
    for (int i = 0; i < veg.rows; i++) {
        for (int j = 0; j < veg.cols; j++) {
            fscanf(file, "%d", &veg.m[i][j]);
        }
    }
    

    fclose(file);

    int nthread = atoi(argv[1]);
    int seg = veg.rows / nthread;
    veg.nThread = nthread;

    printf("seg: %d", seg);
    pthread_t *thread = (int **)malloc(nthread * sizeof(int *));

    for (size_t i = 0; i < nthread; i++){
        veg.fMin = i * seg;
        veg.fMax = (i == nthread - 1) ? veg.rows : veg.fMin + seg;
        veg.id = i;
       pthread_create(&thread[i], NULL, BuscarPatron, (void *)&veg);
    }
    

    return EXIT_SUCCESS;
}


void *BuscarPatron(void *arg){

    vegetacion *veg= (vegetacion*)arg;

    for (size_t i = veg->fMin; i < veg->fMax; i++){
       for (size_t j = 0; j < veg->cols; j++){
        for (size_t k = 0; i < veg->p->rows; k++){
            for (size_t c = 0; i < veg->p->cols; c++){
                if(veg->p->patron[k][c] == veg->m[i + k][j + c]){
                    r[k][c] = 1;
                }
            }
            
        }
        int aux = EncontroPatron(r);
        printf("es verdad: %d\n", aux);
        if()

       }
       
        
        
    }
    


}

int EncontroPatron(int m[][3]){

    int cont = 0;

    for (size_t i = 0; i < 3; i++){
        for (size_t j = 0; j < 3; j++){
            if(m[i][j] == 1){
                cont++;
            }
        }
    }
    if (cont == 6){
        return 1;
    }
    
    return 0;
    
}

 int celdasAdyacentes(int **matrix, int status, int M, int N, int row, int col){
    int cont = 0;
    for(int x = -1; x <= 1; x++){
        for(int y = -1; y <= 1; y++){
            int r = row + x;
            int c = col + y;
            if(r >= 0 && r < M && c >= 0 && c < N){
                if(matrix[r][c] == status){
    
                }          
            }
        }
    }
    return cont;
} 


