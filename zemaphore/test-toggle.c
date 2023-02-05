#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include "zemaphore.h"

#define NUM_THREADS 3
#define NUM_ITER 10

zem_t childs[NUM_THREADS];
zem_t parent;

void *justprint(void *data)
{
  int thread_id = *((int *)data);

  for(int i=0; i < NUM_ITER; i++)
    {
      zem_down(&childs[thread_id]);
      printf("This is thread %d\n", thread_id);
      zem_up(&parent);
    }
  return 0;
}

int main(int argc, char *argv[])
{

  pthread_t mythreads[NUM_THREADS];
  int mythread_id[NUM_THREADS];

  zem_init(&childs[0],1);
  zem_init(&childs[1],0);
  zem_init(&childs[2],0);
  zem_init(&parent, 0);

  
  for(int i =0; i < NUM_THREADS; i++)
    {
      mythread_id[i] = i;
      pthread_create(&mythreads[i], NULL, justprint, (void *)&mythread_id[i]);
    }

  for(int i =1; i < NUM_ITER*3; i++)
    {
      zem_up(&childs[i%NUM_THREADS]);
      zem_down(&parent);
    }
  
  for(int i =0; i < NUM_THREADS; i++)
    {
      pthread_join(mythreads[i], NULL);
    }
  
  return 0;
}
