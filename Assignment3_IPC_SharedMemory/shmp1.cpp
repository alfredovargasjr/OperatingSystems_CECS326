/* shmp1.cpp */
#include "registration.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <memory.h>
#include <string.h>

using namespace std;

/*
  -instantiate CLASS struct
    -{class_number, date, title, seats_left}
    - class number = 1001
    - date = 120186
    - title = "operating systems"
    - seats_available = 15
*/
CLASS myclass = { "1001", "120186", "Operating Systems", 15 };

//replace all instances of NCHILD to 3
#define NCHILD 3

//declare shm_init function
int shm_init( void * );
//declare wait_and_wrap_up function
void wait_and_wrap_up( int [], void *, int );
//declare rpterror function
void rpterror( char *, char * );


int main(int argc, char *argv[])
{
  // declare variables
  //  - create int child[] array with size of NCHILD
  int child[NCHILD], i, shmid, semid;
  // create pointer shm_ptr of unknown type
  void * shm_ptr;
  // declare char arrays
  char ascshmid[10], ascsemid[10], pname[14];
  // copy argv[0] in pname
  strcpy (pname, argv[0]);
  // call shm_init and return int
  //  - id of shared mem segment
  shmid = shm_init(shm_ptr);
  // store shmid with format onto ascshmid
  sprintf (ascshmid, "%d", shmid);
  // fork() NCHILD child processes
  for (i = 0; i < NCHILD; i++) {
    // fork() child process
    //  - fork()
    //    - returns -1: fork() failed
    //    - returns 0: fork() sucess
    child[i] = fork();
    // check case for fork() return
    switch (child[i]) {
      // if return -1, call rpterror() function
      case -1: rpterror ("fork failure", pname);
               // terminate process
               exit(1);
      case 0:
               // print formated
               sprintf (pname, "shmc%d", i+1);
               /*
                  - call to execute a file passing a list of arguements
                  - execl(path, arg0, ... , null terminator)
                    - path: open file with pathname "shmc1"
                    - arg0: pass pname, process name
                    - arg1: pass ascshmid, shared mem segment id
                    - null terminator
               */
               execl("shmc1", pname, ascshmid, (char *)0);
               // catch error and print message
               perror ("execl failed");
               // terminate process
               exit (2);
    }
  }
  // call wait_and_wrap_up() function
  wait_and_wrap_up (child, shm_ptr, shmid);
  return 0;
}

// function shm_init with passes pointer
int shm_init(void *shm_ptr)
{
  int shmid;
  /*
    - system call
    - allocates a system shared memory segment
    - smhget() returns the id of the shared memory segment
      - shmget(key, size, smhflg)
        - key: return key genereated
          - ftok() - convert a pathname and a project identifier to a IPC key
        - size: shared memory segment with size of CLASS struct
        - smhflg:
          - 0600: permission given to the process accessing segment
          - IPC_CREAT: create a new segment
  */
  shmid = shmget(ftok(".",'u'), sizeof(CLASS), 0600 | IPC_CREAT);
  // check to see if shmget() returns error, -1
  if (shmid == -1) {
    // catches error and prints message
    perror ("shmget failed");
    // process termination
    exit(3);
  }
  /*
    - return pointer, address of attached mem segment
    - shmat() shared memory operation, attach the shared memory segment id to
      the address space of the calling process
      - shmat(shmid, shmadder, shmflg)
        - shmid: shared mem segment if
        - smadder: address of the address namespace
        - shmflg: flag 0
  */
  shm_ptr = shmat(shmid, (void * ) 0, 0);
  // if returns -1, then shmat() fails, errno
  if (shm_ptr == (void *) -1) {
    // catch error, print message
    perror ("shmat failed");
    // process termination
    exit(4);
  }
  // memcpy(destination, source, size num)
  // - copy values of myclass to memory block
  memcpy (shm_ptr, (void *) & myclass, sizeof(CLASS) );
  // return id of shared mem segment
  return (shmid);
}

// wait for child processes to terminate to clear shared mem segment
void wait_and_wrap_up(int child[], void *shm_ptr, int shmid)
{
  int wait_rtn, w, ch_active = NCHILD;
  // loop until ch_active is less than 1
  while (ch_active > 0) {
    // wait() system call, wait until child process terminates
    //  - return -1 on error
    //  - return process ID on sucessful termination
    wait_rtn = wait( (int *)0 );
    // loop through the child array, check form terminated children
    for (w = 0; w < NCHILD; w++)
      // if child[w] pid is equal to wait_rtn, decriment num of children
      if (child[w] == wait_rtn) {
        ch_active--;
        break;

    }
  }
  cout << "Parent removing shm" << endl;
  // shmdt() system call detaches the shared mem segment shm_ptr
  shmdt (shm_ptr);
  /*
      - shared memory control
      - shmctl() performs the control operation specified by smd
        - shmctl(shmid, cmd, buffer)
          - shmid: id of shared mem segment
          - cmd: IPC_RMID, the segment be destroyed after last process
                 detaches it
          - null pointer to buffer
  */
  shmctl (shmid, IPC_RMID, (struct shmid_ds *) 0);
  // terminate process
  exit (0);
}

// report error from fork()ing process
void rpterror(char *string, char *pname)
{
  char errline[50];
  sprintf (errline, "%s %s", string, pname);
  // catch error, print message
  perror (errline);
}
