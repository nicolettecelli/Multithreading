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
#include <unistd.h>
#include <time.h>
#include <omp.h>

// Function declarations
void professor();
void student(int id);
void answer_start();
void answer_done();
void enter_office();
void leave_office();
void question_start();
void question_done();

// Global variables
int num_students, capacity, current_id, asking_id;
omp_lock_t waiting, asking, ask_question, answer_question;

void professor() {
  //printf("%d\n", omp_get_thread_num());

  //while(1) { // professor answers questions until the students are satisfied
    omp_unset_lock(&ask_question);

    omp_set_lock(&answer_question);
    answer_start();
    answer_done();
    omp_unset_lock(&answer_question);

    omp_set_lock(&ask_question);
  //}
}

void student(int id) {
  //printf("%d\n", id);
  int questions = (id % 4) + 1;

  // enter office
  omp_set_lock(&waiting);

  current_id = id;
  enter_office();

  int i;
  for (i = 0; i < questions; i++) {   // ask question
    omp_set_lock(&asking);

    omp_set_lock(&ask_question);
    asking_id = id;
    question_start();
    professor();
    omp_unset_lock(&ask_question);

    omp_set_lock(&ask_question);
    question_done();
    omp_unset_lock(&ask_question);

    omp_unset_lock(&asking);
  }

  omp_unset_lock(&waiting);
  current_id = id;
  leave_office();
}

void answer_start() {
  printf("Professor starts to answer question for student %d.\n", asking_id);
}

void answer_done() {
  printf("Professor is done with answer for student %d.\n", asking_id);
}

void enter_office() {
  printf("Student %d enters the office.\n", current_id);
}

void leave_office() {
  printf("Student %d leaves the office.\n", current_id);
}

void question_start() {
  printf("Student %d asks a question.\n", asking_id);
}

void question_done() {
  printf("Student %d is satisfied.\n", asking_id);
}

int main(int argc, char *argv[]) {
  // Too many or few arguments
  if (argc != 3) {
    printf("usage: task1-3 <number_of_students> <capacity>\n");
    exit(-1);
  }
  else {
    num_students = atoi(argv[1]);
    capacity = atoi(argv[2]);

    // validates number of students
    if (num_students <= 0) {
      printf("Number of students should be an integer greater than 0\n");
      exit(-1);
    }
    // validates office capacity
    if (capacity <= 0) {
      printf("Office capacity should be an integer greater than 0\n");
      exit(-1);
    }

    //omp_set_num_threads(num_students);

    // Create professor thread
    //professor();

    omp_init_lock(&waiting);
    omp_init_lock(&asking);
    omp_init_lock(&ask_question);
    omp_init_lock(&answer_question);

    #pragma omp parallel num_threads(num_students+1)
    {
      int tid = omp_get_thread_num();

      //if (tid == 0) {
        //professor();
      //}
      //else {
        student(tid);
      //}
    }

    /*int i;
    for (i = 0; i < num_students; i++) {
      // Create all student threads
      student(i);
    }*/

    omp_destroy_lock(&waiting);
    omp_destroy_lock(&asking);
    omp_destroy_lock(&ask_question);
    omp_destroy_lock(&answer_question);
  }
  // End of simulation
  printf("Office hours are closed!\n");

  return 0;
}
