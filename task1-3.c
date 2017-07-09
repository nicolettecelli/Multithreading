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
#include <pthread.h>
#include <semaphore.h>

// Function declarations
void professor();
void *start_professor();
void student(int id);
void *start_student(void *n);
void answer_start();
void answer_done();
void enter_office();
void leave_office();
void question_start();
void question_done();

// Global variables
int num_students, capacity, current_id, asking_id;
pthread_t *student_threads;
sem_t waiting, asking, ask_question, answer_question;

// Student structure
struct student {
  int id;         // student id
  int questions;  // number of questions to ask
};

void professor() {
  pthread_t professor;
  pthread_create(&professor, NULL, start_professor, NULL);
}

void *start_professor() {
  while(1) { // professor will answer questions until the students are satisfied
    sem_post(&ask_question);
    sem_wait(&answer_question);

    answer_start();
    answer_done();

    sem_post(&ask_question);
    sem_wait(&answer_question);
  }
}

void student(int id) {
  struct student *this_student = (struct student *)malloc(sizeof(struct student));

  this_student->id = id;
  this_student->questions = (id % 4) + 1;

  pthread_t student_thread;
  pthread_create(&student_thread, NULL, start_student, (void *)this_student);
  *(student_threads + id) = student_thread;
  //pthread_join(&student_thread, NULL);
}

void *start_student(void *n) {
  struct student *this_student = (struct student *)n;

  int id = this_student->id;
  //current_id = this_student->id;
  int questions = this_student->questions;

  // If the office is full, wait
  sem_wait(&waiting);

  current_id = id;
  enter_office();

  int i;
  for (i = 0; i < questions; i++) { // until all their questions are answered
    // if other students are asking, wait
    sem_wait(&asking);

    sem_wait(&ask_question);
    asking_id = id;
    question_start();

    sem_post(&answer_question);
    sem_wait(&ask_question);
    question_done();

    sem_post(&answer_question);
    sem_post(&asking);
  }

  sem_post(&waiting); // student leaves the office
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
    // Initialize semaphores
    sem_init(&waiting, 0, capacity);
    sem_init(&asking, 0, 1);
    sem_init(&ask_question, 0, 0);
    sem_init(&answer_question, 0, 0);

    // Create professor thread
    professor();

    student_threads = malloc(sizeof(struct student) * num_students);

    int i;
    for (i = 0; i < num_students; i++) {
      // Create all student threads
      student(i);
    }

    for (i = 0; i < num_students; i++) {
      pthread_join(*(student_threads + i), NULL);
    }

    // Deallocate memory & destroy semaphores
    sem_destroy(&waiting);
    sem_destroy(&asking);
    sem_destroy(&ask_question);
    sem_destroy(&answer_question);

    free(student_threads);
  }
  // End of simulation
  printf("Office hours are closed!\n");

  return 0;
}
