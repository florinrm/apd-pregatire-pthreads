#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
 
#define NUM_THREADS 5
 
struct my_barrier_t {
    pthread_mutex_t lock;
    sem_t sem;
    int nr_threads_to_come;
    int nr_threads_total;
};
 
struct my_barrier_t bar;
 
void my_barrier_init(struct my_barrier_t *bar, int nr_threads_total) {
    pthread_mutex_init(&bar->lock, NULL);
    sem_init (&bar->sem, 0, 0);
    bar->nr_threads_to_come = nr_threads_total;
    bar->nr_threads_total = nr_threads_total;
}
 
void my_barrier_destroy(struct my_barrier_t *bar) {
    sem_destroy(&bar->sem);
    pthread_mutex_destroy(&bar->lock);  
}
 
void *thread_routine(void *arg) {
    int thd_id = *(int*) arg;
 
    printf("thread %d: before the barrier\n", thd_id);

    pthread_mutex_lock(&bar.lock);
    bar.nr_threads_to_come--;
    pthread_mutex_unlock(&bar.lock);
 
    int is_last_thread_to_come = (bar.nr_threads_to_come == 0);

    if (is_last_thread_to_come) {
        printf("releasing the barrier\n");
        for (int i = 0; i < bar.nr_threads_total; i++) {
            sem_post(&bar.sem);
        }
    }

    sem_wait(&bar.sem);
 
    printf("thread %d: after the barrier\n", thd_id);
    return NULL;
}
 
int main() {
    pthread_t tids[NUM_THREADS];
 
    my_barrier_init(&bar, NUM_THREADS);
 
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&tids[i], NULL, thread_routine, (void *) &i);
    }
 
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(tids[i], NULL);
    }
 
    my_barrier_destroy(&bar);
 
    return 0;
}