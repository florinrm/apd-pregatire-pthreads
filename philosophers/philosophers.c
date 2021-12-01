#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <unistd.h>

#define THREADS 8

pthread_mutex_t locks[THREADS];

void *philosopher_function(void* arg) {
    int tid = *(int*) arg;

    if (tid == THREADS - 1) {
        pthread_mutex_lock(&locks[(tid + 1) % THREADS]);
        usleep(100);
        pthread_mutex_lock(&locks[tid]);
        printf("Philosopher %d is eating\n", tid);
        pthread_mutex_unlock(&locks[tid]);
        pthread_mutex_unlock(&locks[(tid + 1) % THREADS]);
    } else {
        pthread_mutex_lock(&locks[tid]);
        usleep(100);
        pthread_mutex_lock(&locks[(tid + 1) % THREADS]);
        printf("Philosopher %d is eating\n", tid);
        pthread_mutex_unlock(&locks[(tid + 1) % THREADS]);
        pthread_mutex_unlock(&locks[tid]);
    }

    return NULL;
}

int main() {
    pthread_t threads[THREADS];
    int tids[THREADS];

    for (int i = 0; i < THREADS; i++) {
        pthread_mutex_init(&locks[i], NULL);
    }

    for (int i = 0; i < THREADS; i++) {
        tids[i] = i;
        pthread_create(&threads[i], NULL, philosopher_function, &tids[i]);
    }

    for (int i = 0; i < THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < THREADS; i++) {
        pthread_mutex_destroy(&locks[i]);
    }

    return 0;
}