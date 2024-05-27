#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

pthread_t hilo1, hilo2;

pthread_mutex_t mutex;

int count = 100;

void *increment(void *argc){
    int c = *(int *) argc;
    for (int i = 0; i < c; i++){
        pthread_mutex_lock(&mutex);
        count++;
        pthread_mutex_unlock(&mutex);
    }
    return EXIT_SUCCESS;
}
void *decrement(void *argc){
    int c = *(int *) argc;
    for (int i = 0; i < c; i++){
        pthread_mutex_lock(&mutex);
        count--;
        pthread_mutex_unlock(&mutex);
    }
    return EXIT_SUCCESS;
}

int main(int argc, char **argv){

    int iteraciones = atoi(argv[1]);
    pthread_mutex_init(&mutex, NULL);

    pthread_create(&hilo1, NULL, (void*) &increment, (void*) &iteraciones);
    pthread_create(&hilo2, NULL, (void*) &decrement, (void*) &iteraciones);

    pthread_join(hilo1, NULL);
    pthread_join(hilo2, NULL);

    printf("count: %d\n", count);

    return EXIT_SUCCESS;
}