#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

typedef struct _thread_arg {
	int thread_index;
	int thread_num;
	int array_size;
    int *array;
    int *ranks;
    int *result;
	pthread_barrier_t *barrier;
} thread_arg;


int min(int a, int b) {
    return a <= b ? a : b;
}

void *sort(void* args) {
    thread_arg *targs = (thread_arg *) args;
    
    int thread_index = targs->thread_index;
    int thread_num = targs->thread_num;
    int array_size = targs->array_size;

    int start = thread_index * ((double) array_size / thread_num);
    int end = min(array_size, (thread_index + 1) * ((double) array_size / thread_num));

    for (int i = start; i < end; i++) {
        for (int j = 0; j < array_size; j++) {
            if (targs->array[i] > targs->array[j]) {
                targs->ranks[i]++;
            }
        }
    }

    pthread_barrier_wait(targs->barrier);

    for (int i = start; i < end; i++) {
        targs->result[targs->ranks[i]] = targs->array[i];
    }

    pthread_barrier_wait(targs->barrier);

    if (targs->thread_index == 0) {
        int is_correctly_sorted = 1;
        for (int i = 0; i < targs->array_size - 1; i++) {
            if (targs->result[i] > targs->result[i + 1]) {
                is_correctly_sorted = 0;
                break;
            }
        }

        if (is_correctly_sorted) {
            printf("Correcly sorted\n");
        } else {
            printf("Incorrecly sorted\n");
        }

        for (int i = 0; i < targs->array_size; i++) {
            printf("%d ", targs->result[i]);
        }
        printf("\n");
    }

    return NULL;
}

int main(int argc, char const *argv[]) {
    if (argc < 3) {
        perror("Insufficient arguments!");
        exit(EXIT_FAILURE);
    }

    int size = atoi(argv[1]);
    int thread_num = atoi(argv[2]);

    pthread_t *threads = calloc(thread_num, sizeof(pthread_t));
	thread_arg *targs = calloc(thread_num, sizeof(thread_arg));

    int *array = calloc(size, sizeof(int));
    int *ranks = calloc(size, sizeof(int));
    int *result = calloc(size, sizeof(int));

    for (int i = 0; i < size; i++) {
        array[i] = size - i + 5;
    }

	pthread_barrier_t barrier;
	pthread_barrier_init(&barrier, NULL, thread_num);

    for (int i = 0; i < thread_num; ++i) {
		targs[i].thread_index = i;
        targs[i].array_size = size;
        targs[i].thread_num = thread_num;
        targs[i].ranks = ranks;
        targs[i].array = array;
        targs[i].result = result;
		targs[i].barrier = &barrier;
		pthread_create(&threads[i], NULL, sort, &targs[i]);
	}

    for (int i = 0; i < thread_num; ++i) {
		pthread_join(threads[i], NULL);
 	}

    pthread_barrier_destroy(&barrier);
	free(targs);
	free(threads);
    free(array);
	free(ranks);
    free(result);

    return 0;
}