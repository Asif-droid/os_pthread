#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>



int NUM_CHAIRS;
int NUM_CUSTOMERS;
int waiting=0;

pthread_mutex_t wait_mutex;
pthread_cond_t customer_ready;
pthread_cond_t barber_ready;
int day_end=0;
int customer_to_see;


void *barber_fun(void *arg) {
    //printf("opened\n");

    while(1){
        pthread_mutex_lock(&wait_mutex);
        //printf("barber wait w=%d c=%d\n",waiting,customer_to_see);

        while(waiting==0 && customer_to_see>0 ){
            pthread_cond_wait(&customer_ready, &wait_mutex);
            
        }
        //printf("here %d\n",customer_to_see);
        
        
        waiting--;
        pthread_cond_signal(&barber_ready);
        printf("Barber taking customer \n");
        printf("Barber cutting hair \n");
        pthread_mutex_unlock(&wait_mutex);
        
        sleep(.25);
        if(customer_to_see<=0 && waiting==0){
            printf("end\n");
            return 0;
        }
        

    }
    return 0;
}

void *customer_fun(void *arg) {

    int id = *((int *)arg);
    
    
    pthread_mutex_lock(&wait_mutex);
    printf("%d\n",id);
    
    if(waiting==NUM_CHAIRS){
        printf("%d is leaving the shop %d \n",id,waiting);
        customer_to_see--;
        pthread_mutex_unlock(&wait_mutex);

    }else{
        waiting++;
        customer_to_see--;
        printf("%d waiting for barber  \n",id);

        
        pthread_cond_signal(&customer_ready);
        //printf("customer wait w=%d \n",waiting);
        pthread_cond_wait(&barber_ready,&wait_mutex);
        printf("%d got his hair cut and paid for it \n",id);
        
        pthread_mutex_unlock(&wait_mutex);


    }
    
    //printf("%d to see\n",customer_to_see);
    return 0;

}

int main(int argc,char * argv[]){
    NUM_CHAIRS    = atoi(argv[1]);
    NUM_CUSTOMERS     = atoi(argv[2]);

    pthread_t barber;
    pthread_t customers[NUM_CUSTOMERS];
    customer_to_see=NUM_CUSTOMERS;

    pthread_mutex_init(&wait_mutex, NULL);
    pthread_cond_init(&barber_ready, NULL);
    pthread_cond_init(&customer_ready, NULL);
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
    
    pthread_join(barber,NULL);
    printf("barber joined\n");
   
    return 0;
    

}