#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <unistd.h>
#include <semaphore.h>

#define WRITERS 6
#define READERS 8

int current_readers = 0;
int current_writers = 0;

int waiting_readers = 0;
int waiting_writers = 0;

sem_t write_sem;
sem_t read_sem;
sem_t enter;

int has_read[READERS];
int has_written[WRITERS];

pthread_barrier_t barrier;

void *reader(void* arg) {
    int tid = *(int*) arg;

    usleep(500000);
    pthread_barrier_wait(&barrier);
    for(;;) {
        sem_wait(&enter);
        
        if (waiting_writers > 0 || current_writers > 0) {
            waiting_readers++;
            sem_post(&enter);
            sem_wait(&read_sem);
        }

        current_readers++;
        if (waiting_readers > 0) {
            --waiting_readers;
            sem_post(&read_sem);
        } else if (waiting_readers == 0) {
            sem_post(&enter);
        }

        usleep(500000);
        printf("Reader %d is reading, readers = %d\n", tid, current_readers);
        has_read[tid] = 1;
        sem_post(&read_sem);
        --current_readers;

        if (current_readers == 0 && waiting_writers > 0) {
            waiting_writers--;
            sem_post(&write_sem);
        } else if (current_readers > 0 || waiting_readers == 0) {
            sem_post(&enter);
        }

        if (has_read[tid]) {
            break;
        }
    }
}

void *writer(void* arg) {
    int tid = *(int*) arg;

    usleep(500000);
    pthread_barrier_wait(&barrier);
    for(;;) {
        sem_wait(&enter);
        if (current_readers > 0 || current_writers > 0) {
            waiting_writers++;
            sem_post(&enter);
            sem_wait(&write_sem);
        }

        current_writers++;
        sem_post(&enter);

        usleep(5000000);
        printf("Writer %d is writing\n", tid);
        has_written[tid] = 1;

        sem_wait(&enter);
        --current_writers;

        if (waiting_readers > 0 && waiting_writers == 0) {
            waiting_readers--;
            sem_post(&read_sem);
        } else if (waiting_writers > 0) {
            waiting_writers--;
            sem_post(&write_sem);
        } else if (waiting_readers == 0 && waiting_writers == 0) {
            sem_post(&enter);
        }

        if (has_written[tid]) {
            break;
        }
    }
}

int main() {
    pthread_t writers[WRITERS];
    int writers_tids[WRITERS];

    pthread_t readers[READERS];
    int readers_tids[READERS];

    sem_init(&enter, 0, 1);
    sem_init(&write_sem, 0, 0);
    sem_init(&read_sem, 0, 0);

    pthread_barrier_init(&barrier, NULL, READERS + WRITERS);

    for (int i = 0; i < READERS; i++) {
        has_read[i] = 0;
    }

    for (int i = 0; i < WRITERS; i++) {
        has_written[i] = 0;
    }

    for (int i = 0; i < READERS; i++) {
        readers_tids[i] = i;
        pthread_create(&readers[i], NULL, reader, &readers_tids[i]);
    }

    for (int i = 0; i < WRITERS; i++) {
        writers_tids[i] = i;
        pthread_create(&writers[i], NULL, writer, &writers_tids[i]);
    }

    for (int i = 0; i < READERS; i++) {
        pthread_join(readers[i], NULL);
    }

    for (int i = 0; i < WRITERS; i++) {
        pthread_join(writers[i], NULL);
    }

    pthread_barrier_destroy(&barrier);

    return 0;
}