#define _REENTRANT    1

#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#define NUM_THREADS    50
#define CONSUMER        0
#define PRODUCER        1

#define BUF_LEN         3

pthread_mutex_t mutex; // folosit pentru incrementarea si decrementarea marimii buffer-ului
pthread_cond_t full_cond; // cand buffer-ul este gol
pthread_cond_t empty_cond; // cand buffer-ul este plin

char buffer[BUF_LEN];
int buf_cnt = 0;

void my_pthread_sleep(int millis) {
	struct timeval timeout;

	timeout.tv_sec = millis / 1000;
	timeout.tv_usec = (millis % 1000) * 1000;

	select (0, NULL, NULL, NULL, &timeout);
}

void *producer_func (void *arg) {
	pthread_mutex_lock (&mutex);
	
	// cat timp buffer-ul este plin, producatorul asteapta
	while (buf_cnt == BUF_LEN) {
		pthread_cond_wait (&full_cond, &mutex);
	}

	buffer[buf_cnt] = 'a';
	buf_cnt++;
	printf ("Produs un element.\n");

	pthread_cond_signal (&empty_cond);
	my_pthread_sleep (rand () % 1000);

	pthread_mutex_unlock (&mutex);

	return NULL;
}

void *consumer_func (void *arg) {
	pthread_mutex_lock (&mutex);

	// cat timp buffer-ul este gol, consumatorul asteapta
	while (buf_cnt == 0) {
		pthread_cond_wait (&empty_cond, &mutex);
	}

	buf_cnt--;
	char elem = buffer[buf_cnt];
	printf ("Consumat un element: %c\n", elem);

	pthread_cond_signal (&full_cond);
	my_pthread_sleep (rand () % 1000);

	pthread_mutex_unlock (&mutex);

	return NULL;
}

int main() {
	int i;
	int type;
	pthread_t tid_v[NUM_THREADS];

	pthread_mutex_init (&mutex, NULL);
	pthread_cond_init (&full_cond, NULL);
	pthread_cond_init (&empty_cond, NULL);

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