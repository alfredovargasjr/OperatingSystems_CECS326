#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

using namespace std;

/*
child process will execute sender.cpp
*/
void child_proc_one(int qid) {
    cout << "Sender Process ID:\t" << getpid() << endl;
}

/*
child process will execute receiver.cpp
*/
void child_proc_two(int qid) {
    cout << "Receiver Process ID:\t" << getpid() << endl;
}

int main(){
    /*
    System call: msgget(key_t key, int msgflg)
            key: key arguement for queue
            bits: permissions of the message queue
    */
    //acquire a message queue from the operating system, message queue id
    int qid = msgget(IPC_PRIVATE, IPC_EXCL|IPC_CREAT|0600);
    cout << "Message Queue ID:\t" << qid << endl;

    //spawning two child processes using the fork() system call
    pid_t cpid = fork();
    
    //child process ID = 0
    if (cpid == 0) {
            //call the child function       
            child_proc_one(qid);
            //process termination
            exit(0);
    }
    
    //create the second child process using the fork() system call
    cpid = fork();
    if (cpid == 0) {  
            //call the child function     
            child_proc_two(qid);
            //process termination
            exit(0);
    }

    while(wait(NULL) != -1); // waiting for both children to terminate
    //remove qid from the message queue, qid 
    msgctl(qid, IPC_RMID, NULL);
    exit(0); //process termination
}
