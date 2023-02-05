#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include "zemaphore.h"

zem_t child;
zem_t parent;

void *justprint(void *data)
{
  int thread_id = *((int *)data);
  zem_down(&child);
  printf("This is thread %d\n", thread_id);
  zem_up(&parent);
  return 0;
}

int main(int argc, char *argv[])
{

  pthread_t t1, t2,t3;
  int t1id = 1, t2id = 2,t3id = 3;

  zem_init(&child, 0);
  zem_init(&parent, 0);
  
  pthread_create(&t1, NULL, justprint, &t1id);
  pthread_create(&t2, NULL, justprint, &t2id);
  pthread_create(&t3, NULL, justprint, &t3id);
  
  sleep(1);


  //in spite of sleep, this should print first
  printf("This is main thread. This should print first\n");

  zem_up(&child);
  zem_down(&parent);
  printf("One thread has printed\n");
  
  zem_up(&child);
  zem_down(&parent);
  printf("Second thread has printed\n");

  zem_up(&child);
  zem_down(&parent);
  printf("Third thread has printed\n");
  
  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  pthread_join(t3, NULL);
  
  return 0;
}
