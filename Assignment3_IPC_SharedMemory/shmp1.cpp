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

CLASS myclass = { "1001", "120186", "Operating Systems", 15 };

#define NCHILD 3

int shm_init( void * );
void wait_and_wrap_up( int [], void *, int );
void rpterror( char *, char * );

int main(int argc, char *argv[])
{
  int child[NCHILD], i, shmid, semid;
  void * shm_ptr;
  char ascshmid[10], ascsemid[10], pname[14];
  strcpy (pname, argv[0]);
  shmid = shm_init(shm_ptr);
  sprintf (ascshmid, "%d", shmid);
  for (i = 0; i < NCHILD; i++) {
    child[i] = fork();
    switch (child[i]) {
      case -1: rpterror ("fork failure", pname);
               exit(1);
      case 0:
               sprintf (pname, "shmc%d", i+1);
               execl("shmc1", pname, ascshmid, (char *)0);
               perror ("execl failed");
               exit (2);
    }
  }
  wait_and_wrap_up (child, shm_ptr, shmid);
  return 0;
}

int shm_init(void *shm_ptr)
{
  int shmid;
  shmid = shmget(ftok(".",'u'), sizeof(CLASS), 0600 | IPC_CREAT);
  if (shmid == -1) {
    perror ("shmget failed");
    exit(3);
  }
  shm_ptr = shmat(shmid, (void * ) 0, 0);
  if (shm_ptr == (void *) -1) {
    perror ("shmat failed");
    exit(4);
  }
  memcpy (shm_ptr, (void *) &myclass, sizeof(CLASS) );
  return (shmid);
}

void wait_and_wrap_up(int child[], void *shm_ptr, int shmid)
{
  int wait_rtn, w, ch_active = NCHILD;
  while (ch_active > 0) {
    wait_rtn = wait( (int *)0 );
    for (w = 0; w < NCHILD; w++)
      if (child[w] == wait_rtn) {
        ch_active--;
        break;
      
    }
  }
  cout << "Parent removing shm" << endl;
  shmdt (shm_ptr);
  shmctl (shmid, IPC_RMID, (struct shmid_ds *) 0);
  exit (0);
}

void rpterror(char *string, char *pname)
{
  char errline[50];
  sprintf (errline, "%s %s", string, pname);
  perror (errline);
}
