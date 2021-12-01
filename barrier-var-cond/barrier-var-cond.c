#include <stdio.h>
#include <pthread.h>
 
#define NUM_THREADS 5

struct my_barrier_t {
    pthread_mutex_t lock;
    pthread_cond_t  cond;
    int nr_threads_to_come;
};
 
struct my_barrier_t bar;
 
void my_barrier_init(struct my_barrier_t *bar, int nr_threads_total) {
    pthread_mutex_init(&bar->lock, NULL);
    pthread_cond_init(&bar->cond, NULL);
 
    bar->nr_threads_to_come = nr_threads_total;
}
 
void my_barrier_destroy(struct my_barrier_t *bar) {
    pthread_cond_destroy(&bar->cond);
    pthread_mutex_destroy(&bar->lock);  
}
 
void *thread_routine(void *arg) {
    int thd_id = *(int*) arg;
 
    printf("thread %d: before the barrier\n", thd_id);

    pthread_mutex_lock(&bar.lock);
    bar.nr_threads_to_come--;
 
    int is_last_to_arrive = (bar.nr_threads_to_come == 0);

    while (bar.nr_threads_to_come != 0) {
        pthread_cond_wait(&bar.cond, &bar.lock);
    } 
 
    if (is_last_to_arrive) {
        printf("releasing the barrier\n");
        pthread_cond_broadcast(&bar.cond);
    }
    pthread_mutex_unlock(&bar.lock);
 
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