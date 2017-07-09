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

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define CHUNKSIZE    10
#define N            100

int main (int argc, char *argv[]) {
  int nthreads, tid, i, chunk;
  float a[N], b[N], c[N];
  double start, end;    // variables to calculate run time

  for (i=0; i < N; i++) {
    a[i] = b[i] = i * 1.0; // initialize arrays
  }

  chunk = CHUNKSIZE;

  #pragma omp parallel shared(a,b,c,nthreads,chunk) private(i,tid)
  {
    tid = omp_get_thread_num();

    if (tid == 0) {
      nthreads = omp_get_num_threads();
      printf("Number of threads = %d\n", nthreads);
    }

    printf("Thread %d starting...\n",tid);
    start = omp_get_wtime();

    #pragma omp for schedule(static,chunk)
    for (i=0; i<N; i++) {
      c[i] = a[i] + b[i];
      printf("Thread %d: c[%d]= %f\n",tid,i,c[i]);
    }

    end = omp_get_wtime();
    //printf("End time: %d seconds\n", end);
    //printf("Total runtime: %d seconds\n", (end-start));
  } /* end of parallel section */
}
