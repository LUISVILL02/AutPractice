#include <stdio.h>
#include <stdlib.h>

#define NUM_DIMENSIONES 10  // Número de dimensiones
#define TAMAÑO_DIMENSION 2 // Tamaño de cada dimensión

int calcular_tamaño_total(int *dimensiones, int num_dimensiones) {
    int tamaño_total = 1;
    for (int i = 0; i < num_dimensiones; i++) {
        tamaño_total *= dimensiones[i];
    }
    return tamaño_total;
}

void imprimir_matriz(int *matriz, int *dimensiones, int *indices, int dim_actual, int num_dimensiones) {
    if (dim_actual == num_dimensiones) {
        // Calcular el índice para acceder al elemento en la matriz
        int indice = 0;
        for (int i = 0; i < num_dimensiones; i++) {
            indice = indice * dimensiones[i] + indices[i];
        }

        // Imprimir el elemento en la matriz
        printf("matriz[");
        for (int i = 0; i < num_dimensiones; i++) {
            printf("%d", indices[i]);
            if (i < num_dimensiones - 1) {
                printf(", ");
            }
        }
        printf("] = %d\n", matriz[indice]);
    } else {
        for (int i = 0; i < dimensiones[dim_actual]; i++) {
            indices[dim_actual] = i;
            imprimir_matriz(matriz, dimensiones, indices, dim_actual + 1, num_dimensiones);
        }
    }
}

int main() {
    int *matriz;
    int dimensiones[NUM_DIMENSIONES]; // Dimensiones de la matriz
    int tamaño_total; // Tamaño total de la matriz
    int indices[NUM_DIMENSIONES] = {0}; // Índices para recorrer la matriz

    // Inicializar todas las dimensiones de la matriz con el mismo tamaño
    for (int i = 0; i < NUM_DIMENSIONES; i++) {
        dimensiones[i] = TAMAÑO_DIMENSION;
    }

    // Calcular el tamaño total de la matriz
    tamaño_total = calcular_tamaño_total(dimensiones, NUM_DIMENSIONES);

    // Asignar memoria dinámicamente para la matriz
    matriz = (int *)malloc(tamaño_total * sizeof(int));
    if (matriz == NULL) {
        perror("Error al asignar memoria para la matriz");
        exit(1);
    }

    // Utilizar la matriz (por ejemplo, inicializarla con valores)
    for (int i = 0; i < tamaño_total; i++) {
        matriz[i] = i;
    }

    // Imprimir la matriz
    imprimir_matriz(matriz, dimensiones, indices, 0, NUM_DIMENSIONES);

    // Liberar la memoria asignada
    free(matriz);

    return 0;
}
