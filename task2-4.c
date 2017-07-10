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

#define M 500
#define N 500

int main(int argc, char *argv) {
  //set number of threads here
  omp_set_num_threads(32);

  int i, j, k;
  double sum;
  double **A, **B, **C;

  A = malloc(M*sizeof(double *));
  B = malloc(M*sizeof(double *));
  C = malloc(M*sizeof(double *));

  for (i = 0; i < M; i++) {
    A[i] = malloc(N*sizeof(double));
    B[i] = malloc(N*sizeof(double));
    C[i] = malloc(N*sizeof(double));
  }

  double start, end;

  for (i = 0; i < M; i++) {
     for (j = 0; j < N; j++) {
       A[i][j] = j*1;
       B[i][j] = i*j+2;
       C[i][j] = j-i*2;
     }
  }

  start = omp_get_wtime();

 /*// Original, no parallel
  for (i = 0; i < M; i++) {
    for (j = 0; j < N; j++) {
      sum = 0;
      for (k=0; k < M; k++) {
        sum += A[i][k]*B[k][j];
      }
      C[i][j] = sum;
    }
  }*/

  /*// Outer for-loop ONLY
  #pragma omp parallel shared(A,B,C) private(i,j,k)
  {
    #pragma omp for
    for (i = 0; i < M; i++) {
      for (j = 0; j < N; j++) {
         sum = 0;
         for (k=0; k < M; k++) {
            sum += A[i][k]*B[k][j];
         }
         C[i][j] = sum;
     }
    }
  }*/

  /*// Middle for-loop ONLY
  #pragma omp parallel shared(A,B,C) private(i,j,k)
  {
    for (i = 0; i < M; i++) {
      #pragma omp for
      for (j = 0; j < N; j++) {
         sum = 0;
         for (k=0; k < M; k++) {
            sum += A[i][k]*B[k][j];
         }
         C[i][j] = sum;
     }
    }
  }*/

  // BOTH outer and middle
  #pragma omp parallel for shared(A,B,C) private(i) schedule(static)
  for (i = 0; i < M; i++) {
    #pragma omp parallel for shared(A,B,C) private(j,k,sum)
    for (j = 0; j < N; j++) {
       sum = 0;
       for (k=0; k < M; k++) {
          sum += A[i][k]*B[k][j];
       }
       C[i][j] = sum;
   }
 }

  end = omp_get_wtime();

  printf("Time of computation: %f\n", end-start);
}
