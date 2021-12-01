#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>

#define CHAIRS 3
#define CLIENTS 7

#define SERVED 1
#define UNSERVED 2
    
pthread_mutex_t served_client_mutex;

sem_t barber_ready; 
sem_t client_ready;
sem_t modifySeats;

int total_clients;

int available_chairs;
int left_clients[CLIENTS];
time_t waiting_time_sum;

int are_remaining_clients() {
    int count = 0;
    for (int i = 0; i < CLIENTS; i++) {
        if (left_clients[i]) {
            count++;
        }
    }

    return count != CLIENTS;
}

// dummy function for serving a client
void service_client() {
    int s = rand() % 401;
    s = s * 1000;
    printf("brrr, cutting client's hair\n");
    usleep(s);
    printf("done cutting hair\n");
}

void *barber_function(void *idp) {    
    int counter = 0;
    
    for(;;) {
        // Lock semaphore "client_ready" - try to get a customer or sleep if there is none
        sem_wait(&client_ready);

        // Lock semaphore "modifySeats" - try to get access to seats
        sem_wait(&modifySeats);

        // Increment by 1 the available seats
        available_chairs++;

        // Unlock semaphore "modifySeats"
        sem_post(&modifySeats);

        // Unlock semaphore "barber_ready" - set barber ready to serve"
        sem_post(&barber_ready);        

        // Lock mutex "served_client_mutex - protect service from the same barber from other threads (clients)
        pthread_mutex_lock(&served_client_mutex);

        // serve client
        service_client();    

        // Unlock mutex "served_client_mutex - finished service
        pthread_mutex_unlock(&served_client_mutex);
        
        printf("Client %d was served.\n", counter);
        counter++; 

        if (!are_remaining_clients()) {
            break;
        }

    }
    pthread_exit(NULL);    
}

void *customer_function(void *idp) {  
    int client_id = *(int*) idp;
    // Lock semaphore "modifySeats"
    sem_wait(&modifySeats); 

    // If there is available seat
    if (available_chairs >= 1) {
        // Occupy a seat
        available_chairs--;

        printf("Client %d is waiting.\n", client_id);
        printf("Available seats: %d\n", available_chairs);
        
        // Unlock semaphore "client_ready" - set the customer ready to be served
        sem_post(&client_ready);

        // Unlock semaphore "modifySeats"
        sem_post(&modifySeats);         

        // Lock semaphore "barber_ready" - wait for barber to get ready
        sem_wait(&barber_ready); 

        printf("Client %d is being served. \n", client_id);
        left_clients[client_id] = SERVED; 
    } else {
        // Unlock semaphore "modifySeats"
        sem_post(&modifySeats);
        printf("Client %d left.\n", client_id);
        left_clients[client_id] = UNSERVED;
    }
        
    pthread_exit(NULL);
}

int main() {
    srand(time(NULL));   

    // barber
    pthread_t barber;

    // clients
    pthread_t clients[CLIENTS];

    pthread_mutex_init(&served_client_mutex, NULL);

    // init semaphores
    sem_init(&client_ready, 0, 0);
    sem_init(&barber_ready, 0, 0);
    sem_init(&modifySeats, 0, 1);
    
    total_clients = CLIENTS;
    available_chairs = CHAIRS;

    for (int i = 0; i < CLIENTS; i++) {
        left_clients[i] = 0;
    }
    
    /* Create barber thread*/
    int barber_result = pthread_create(&barber, NULL, (void *)barber_function, NULL);  
    if (barber_result) {
        perror("Failed to create thread.\n");
        exit(EXIT_FAILURE);
    } 
    
    /* Create customer_maker thread*/
    for (int i = 0; i < CLIENTS; i++) {
        int client_result = pthread_create(&clients[i], NULL, (void *)customer_function, &i);  
        if (client_result) {
            printf("Failed to create thread.\n");
            exit(EXIT_FAILURE);
        }
        usleep(100000);
    } 

    pthread_join(barber, NULL);

    for (int i = 0; i < CLIENTS; i++) {
        pthread_join(clients[i], NULL);
    }

    int unserved_clients = 0;
    for (int i = 0; i < CLIENTS; i++) {
        if (left_clients[i] == UNSERVED) {
            unserved_clients++;
        }
    }

    printf("There are %d unserved clients\n", unserved_clients);
}