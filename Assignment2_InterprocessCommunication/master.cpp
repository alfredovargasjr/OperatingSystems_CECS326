#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sstream>

using namespace std;

/*
child process will execute sender.cpp
*/
void child_proc_one(int qid) {
    cout << "[Master] Child Process 1 ID:\t" << getpid() << endl;
    //use stringstream to get string value of integer qid
    stringstream ss;
    ss << qid;
    //s = string(qid)
    string s = ss.str();
    char arg0 [s.length() + 1];
    //copy content of s ontp arg0 (arg0 + qid)
    strcpy(arg0, s.c_str());
    //add null terminator
    arg0[sizeof(arg0)] = '\0';
    //execl(path, arguement0,..., null terminator) system call
    //  - send the qid as arg0
    execl("./sender\0", arg0, (char*) NULL);
}

/*
child process will execute receiver.cpp
*/
void child_proc_two(int qid) {
    cout << "[Master] Child Process 2 ID:\t" << getpid() << endl;
    //use stringstream to get string value of integer qid
    stringstream ss;
    ss << qid;
    //s = string(qid)
    string s = ss.str();
    char arg0 [s.length() + 1];
    //copy content of s ontp arg0 (arg0 + qid)
    strcpy(arg0, s.c_str());
    arg0[sizeof(arg0)] = '\0';
    //execl(path, arguement0,..., null terminator) system call
    //  - send the qid as arg0
    execl("./receiver\0", arg0, (char*) NULL);
}

int main(){
    /*
    System call: msgget(key_t key, int msgflg)
            key: key arguement for queue
            bits: permissions of the message queue
    */
    //acquire a message queue from the operating system (kernel), message queue id
    int qid = msgget(IPC_PRIVATE, IPC_EXCL|IPC_CREAT|0600);
    cout << "[Master] Message Queue ID:\t" << qid << endl;

    //spawning two child processes using the fork() system call
    pid_t cpid = fork();
    
    //child process ID = 0
    if (cpid == 0) {
            //call the child function       
            child_proc_one(qid);
            //process termination
            cout << "[Master] child process 1 Terminated." << endl;
            exit(0);
    }
    
    //create the second child process using the fork() system call
    cpid = fork();
    if (cpid == 0) {  
            //call the child function     
            child_proc_two(qid);
            //process termination
            cout << "[Master] child process 2 Terminated." << endl;
            exit(0);
    }

    while(wait(NULL) != -1); // waiting for both children to terminate
    //remove qid from the message queue, qid 
    msgctl(qid, IPC_RMID, NULL);
    cout << "[Master] Master Process terminated." << endl;
    exit(0); //process termination
}
