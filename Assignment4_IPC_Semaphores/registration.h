/*
  Header file to be used with
  shmp1.c and shmc1.c
 */

#ifndef REGISTRATION_H
#define REGISTRATION_H
#include <semaphore.h>

// data structure that will be used for the shared mem segment
struct CLASS {
  sem_t * sem;
  char class_number[6];
  char date[7];
  char title[50];
  int seats_left;
};

#endif
