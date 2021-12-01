#define _REENTRANT    1

#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <semaphore.h>

#define NUM_THREADS    50
#define CONSUMER        0
#define PRODUCER        1

#define BUF_LEN         3

pthread_mutex_t mutex;
sem_t full_sem;     // semafor contor al elementelor pline
sem_t empty_sem;    // semafor contor al elementelor goale

char buffer[BUF_LEN];
int buf_cnt = 0;

void my_pthread_sleep (int millis) {
    struct timeval timeout;

    timeout.tv_sec = millis / 1000;
    timeout.tv_usec = (millis % 1000) * 1000;

    select (0, NULL, NULL, NULL, &timeout);
}

void *producer_func (void *arg) {
    sem_wait (&empty_sem);

    pthread_mutex_lock (&mutex);

    buffer[buf_cnt] = 'a';
    buf_cnt++;
    printf ("Produs un element.\n");

	pthread_mutex_unlock (&mutex);

    my_pthread_sleep (rand () % 1000);

    sem_post (&full_sem);

    return NULL;
}

void *consumer_func (void *arg) {
    sem_wait (&full_sem);

    pthread_mutex_lock (&mutex);

    buf_cnt--;
    char elem = buffer[buf_cnt];
    printf ("Consumat un element: %c\n", elem);

    pthread_mutex_unlock (&mutex);

    my_pthread_sleep (rand () % 1000);

    sem_post (&empty_sem);

    return NULL;
}

int main () {
    int i;
    int type;
    pthread_t tid_v[NUM_THREADS];

    pthread_mutex_init (&mutex, NULL);

    sem_init (&full_sem, 0, 0);
    sem_init (&empty_sem, 0, 3);

    srand (time (NULL));
    for (i = 0; i < NUM_THREADS; i++) {
        type = rand () % 2;
        if (type == CONSUMER) {
        	pthread_create (&tid_v[i], NULL, consumer_func, NULL);
		} else {
            pthread_create (&tid_v[i], NULL, producer_func, NULL);
        }
	}

    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join (tid_v[i], NULL);
	}

	pthread_mutex_destroy(&mutex);

    return 0;
}