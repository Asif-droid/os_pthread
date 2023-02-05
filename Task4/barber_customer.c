#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include "zemaphore.h"



#define NUM_BARBERS 3
int NUM_CHAIRS;
int NUM_CUSTOMERS;

zem_t barber_z;
zem_t barbers_z [NUM_BARBERS];
zem_t customer_z;
int day_end=0;
pthread_mutex_t mutex;
int waiting [NUM_BARBERS];
int chairs [NUM_BARBERS];
int barber_indx=0;
int total_cut=11;

void *barber(void *arg) {
    int id = *((int *)arg);
    printf("%d barber shoop\n",id);
    int i=0;
    while (1) {
        zem_down(&barbers_z[id]);
        if(day_end==1){
            break;
        }
        waiting[id]--;
        zem_up(&customer_z);
        printf("%d Barber: Cutting hair\n",id);
        i++;
        sleep(1);
    }
    printf("store closed\n");
    return 0;
}

void *customer(void *arg) {

    int id = *((int *)arg);

    
    pthread_mutex_lock(&mutex);
    printf("%d customer came\n",id);
    
    for(int i=0;i<NUM_BARBERS;i++){
       
        int index2=barber_indx % NUM_BARBERS;
        if(waiting[index2] < NUM_CHAIRS){
            printf("%d Customer: waiting for %d turn\n",id,index2);
            waiting[index2]++;
            pthread_mutex_unlock(&mutex);
            zem_up(&barbers_z[index2]);
            
            zem_down(&customer_z);
            printf("%d Customer: Getting hair cut by %d \n",id,index2);
            
            return 0;
        }
        barber_indx++;
    }
    
    
    printf("%d Customer: Leaving shops, no chairs available\n",id);
    pthread_mutex_unlock(&mutex);

    return 0;
}

int main(int argc, char *argv[]) {
  

   
    NUM_CHAIRS    = atoi(argv[1]);
    NUM_CUSTOMERS     = atoi(argv[2]);

    for(int i=0;i<NUM_BARBERS;i++){
        zem_init(&barbers_z[i],0);
        waiting[i]=0;
        chairs[i]=NUM_CHAIRS;
    }
    

    pthread_t barber_tid[NUM_BARBERS];
    pthread_t customer_tid[NUM_CUSTOMERS];
    
    zem_init(&barber_z,0);
    zem_init(&customer_z,0);
   

    for (int i = 0; i < NUM_BARBERS; i++) {
        pthread_create(&barber_tid[i], NULL, barber, (void *)&i);
    }
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        pthread_create(&customer_tid[i], NULL, customer, (void *)&i);
    }


     for (int i = 0; i < NUM_CUSTOMERS; i++) {
        pthread_join(customer_tid[i], NULL);
    }

    printf("customer joined\n");
    day_end=1;
    for(int i=0;i<NUM_BARBERS;i++){
        zem_up(&barbers_z[i]);
    }
    

    for (int i = 0; i < NUM_BARBERS; i++) {
        pthread_join(barber_tid[i], NULL);
    }
   


    

    return 0;
}