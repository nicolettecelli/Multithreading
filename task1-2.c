/*##########################################################
## COP4610 – Principles of Operating Systems – Summer C 2017
## Prof. Jose F. Osorio
## Student: Nicolette Celli – 4174075
##
## Project: Multithreaded Programming
## Specs:
## Due Date: 07/9/2017 by 11:55pm
## Module Name:
##
## I certify that this program code has been written by me
## and no part of it has been taken from any sources.
##########################################################*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#ifdef PTHREAD_SYNC
  // initilaze mutex and barrier
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_barrier_t barrier;
#endif

int shared_variable = 0;

// a loop that iterates 20 times for each thread
void *simple_thread(void *which) {
  int num, val = 0;

  for (num = 0; num < 20; num++) {
    if (random() > RAND_MAX / 2) {
      usleep(10);
    }

    #ifdef PTHREAD_SYNC
      pthread_mutex_lock(&mutex);   // Lock mutex
    #endif

    // Critical section
    val = shared_variable;
    printf("*** thread %d sees value %d\n", which, val);
    shared_variable = val + 1;

    #ifdef PTHREAD_SYNC
      pthread_mutex_unlock(&mutex);   // Unlock mutex
    #endif
  }

  #ifdef PTHREAD_SYNC
    pthread_barrier_wait(&barrier); // Wait for all threads
  #endif

  val = shared_variable;
  printf("Thread %d sees final value %d\n", which, val);
}

int main(int argc, char *argv[]) {
  // check argc == 2    ("./task1-1 50", for example)
  if (argc != 2) {
    printf("usage: task1-2 <threadcount>\n");
    exit(-1);
  }
  else {
    // atoi argv[1]
    int threadcount = atoi(argv[1]);

    if (threadcount <= 0) {   // User entered 0 or a noninteger
      printf("Please choose an integer that is greater than 0\n");
      exit(-1);
    }
    else {
      #ifdef PTHREAD_SYNC
        pthread_barrier_init(&barrier, NULL, threadcount);
      #endif

      printf("Creating %d thread(s)\n", threadcount);

      pthread_t threads[threadcount];
      long n;

      // create threads
      for (n = 0; n < threadcount; n++) {
        pthread_create(&threads[n], NULL, simple_thread, (void *)n);
      }
      // join threads
      for (n = 0; n < threadcount; n++) {
        pthread_join(threads[n], NULL);
      }
    }
  }

  return 0;
}
