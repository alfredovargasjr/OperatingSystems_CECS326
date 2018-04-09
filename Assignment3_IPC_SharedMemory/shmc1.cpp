/* shmc1.cpp */
#include "registration.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <memory.h>

using namespace std;

// create pointer to CLASS struct
CLASS *class_ptr;

// declare variables
void *memptr;
char *pname;
int shmid, ret;

// declare functions
void rpterror(char *), srand(), perror(), sleep();
void sell_seats();

// passed parameters from parent
//  - argv[0]: pname
//  - argv[1]: shared mem segment ID
//  - argc: number of strings passed in argv
main(int argc, char* argv[])
{
  // check to see if both strings are passed in argv
  if (argc < 2) {
    fprintf (stderr, "Usage:, %s shmid\n", argv[0]);
    // terminate process
    exit(1);
  }
  // argv[0] is pname passed by parent
  pname = argv[0];
  // store argv[1] with format into shmid/
  //  - store shmid
  sscanf (argv[1], "%d", &shmid);
  /*
    - return pointer, address of attached mem segment
    - shmat() shared memory operation, attach the shared memory segment id to
      the address space of the calling process
      - shmat(shmid, shmadder, shmflg)
        - shmid: shared mem segment if
        - smadder: address of the address namespace
        - shmflg: flag 0
  */
  memptr = shmat (shmid, (void *)0, 0);
  // check to see if shmat() returns an error
  if (memptr == (char *)-1 ) {
    // report error, calling function
    rpterror ("shmat failed");
    // terminate process
    exit(2);
  }
  // point to the shared mem segment
  class_ptr = (struct CLASS *)memptr;
  // call sell_seats()
  sell_seats();
  // shmdt() system call detaches the shared mem segment
  ret = shmdt(memptr);
  // terminate process
  exit(0);
}

// process will sell seats
void sell_seats()
{
  int all_out = 0;
  srand ( (unsigned) getpid() );
  while ( !all_out) {   /* loop to sell all seats */
  // check if seats are available in shared mem segment
  if (class_ptr->seats_left > 0) {
    // sleep for random 0 - 5 ms
    sleep ( (unsigned)rand()%5 + 1);
    // decriment the number of seats available in the shmsegment
    class_ptr->seats_left--;
    // sleep for random 0 - 5 ms
    sleep ( (unsigned)rand()%5 + 1);
    cout << pname << " SOLD SEAT -- "
         << class_ptr->seats_left << " left" << endl;
    }
    else {  // there are no seats left
      all_out++;
      cout << pname << " sees no seats left" << endl;
    }
    // sleep for random 0 - 5 ms
    sleep ( (unsigned)rand()%10 + 1);
  }
}

// report error from fork()ing process
void rpterror(char* string)
{
  char errline[50];
  sprintf (errline, "%s %s", string, pname);
  // catch error, print message
  perror (errline);
}
