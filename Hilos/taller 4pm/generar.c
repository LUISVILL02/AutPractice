#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Función para generar una base de ADN aleatoria
char random_base() {
    char bases[] = {'a', 't', 'c', 'g'};
    return bases[rand() % 4];
}

// Función para generar la secuencia de ADN
void generate_dna_sequence(char *sequence, int length) {
    for (int i = 0; i < length; i++) {
        sequence[i] = random_base();
    }
    sequence[length] = '\0'; // Agregar terminador nulo al final de la cadena
}

// Función para guardar la secuencia de ADN formateada en un archivo
void save_dna_sequence(const char *filename, char *sequence, int length, int line_length) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Error al abrir el archivo %s para escritura.\n", filename);
        exit(1);
    }
    
    for (int i = 0; i < length; i++) {
        fputc(sequence[i], file);
        if ((i + 1) % line_length == 0) {
            fputc('\n', file);
        }
    }
    
    fclose(file);
}

int main(int argc, char **argv) {
    int length = 400000;
    int line_length = 60;
    char *sequence = (char *)malloc((length + 1) * sizeof(char)); // +1 para el terminador nulo

    if (sequence == NULL) {
        fprintf(stderr, "Error al asignar memoria.\n");
        return 1;
    }

    // Inicializar la semilla del generador de números aleatorios
    srand(time(NULL));

    // Generar la secuencia de ADN
    generate_dna_sequence(sequence, length);

    // Guardar la secuencia de ADN en un archivo
    save_dna_sequence(argv[1], sequence, length, line_length);

    printf("Secuencia de ADN generada y guardada en 'dna_sequence.txt'\n");

    // Liberar la memoria asignada
    free(sequence);

    return 0;
}