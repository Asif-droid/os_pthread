#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include "zemaphore.h"

int NUM_CHAIRS;
int NUM_CUSTOMERS;


pthread_mutex_t wait_mutex;

zem_t barber_z;
zem_t customer_z;
int waiting=0;
int customer_to_see;
int day_end=0;

void *barber_fun(void *arg) {

    while(1){
        
        zem_down(&customer_z);
        pthread_mutex_lock(&wait_mutex);
        waiting--;
        printf("taking customer w=%d\n",waiting);
        printf("cutting hair w=%d\n",waiting);
        zem_up(&barber_z);
        pthread_mutex_unlock(&wait_mutex);
        if(customer_to_see<=0 && waiting==0){
            printf("end\n");
            return 0;
        }
        

    }

}
void *customer_fun(void *arg) {
    int id = *((int *)arg);
    pthread_mutex_lock(&wait_mutex);
    if(waiting==NUM_CHAIRS){
        printf("leave the shop\n");
        customer_to_see--;
        pthread_mutex_unlock(&wait_mutex);
    }
    else{
        waiting++;
        customer_to_see--;
        printf("%d waiting for barber w= %d \n",id,waiting);
        pthread_mutex_unlock(&wait_mutex);
        zem_up(&customer_z);
        zem_down(&barber_z);
        printf("%d getting hair cut w=%d\n",id,waiting);
    }
    return 0;

    
}


int main(int argc,char * argv[]){
    NUM_CHAIRS    = atoi(argv[1]);
    NUM_CUSTOMERS     = atoi(argv[2]);

    pthread_t barber;
    pthread_t customers[NUM_CUSTOMERS];
    pthread_mutex_init(&wait_mutex, NULL);
    zem_init(&barber_z,0);
    zem_init(&customer_z,0);

    customer_to_see=NUM_CUSTOMERS;

    int customerid[NUM_CUSTOMERS];
    for(int i=0;i<NUM_CUSTOMERS;i++){
        customerid[i]=i;
    }

    
    pthread_create(&barber, NULL, barber_fun, NULL);

    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        pthread_create(&customers[i], NULL, customer_fun, (void *)&customerid[i]);
    }

    


    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        pthread_join(customers[i], NULL);
    }
    printf("customer joined\n");
    
    
    zem_up(&customer_z);
    pthread_join(barber,NULL);
    printf("barber joined\n");
   
    return 0;
    

}