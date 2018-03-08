#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

using namespace std;

// declare my global message buffer
//data structure for the buffer that will be used in the message queue
struct buf {
        long mtype; // required
        char message[50]; // message content
};

// convert char array to integer, return int
int get_qid(char c []){
    return atoi(c);
}

void receive_msg(int qid){
    buf msg;
    int size = sizeof(msg) - sizeof(long);
    /*
    system call: msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg);
            -msqid: id of the message queue created by the OS
            -*msgp: pointer to the defined structure, (buf buffer)
            -msgsz: size of msgp message
            -msgtyp: message type of message in queue
            -msgflg: message flag behavior when message length is bigger than msgsz
    -remove message from the qid queue and place in buffer (msg)
    -msgtyp = 0, read first message in the queue
    */
    msgrcv(qid, (struct msgbuf *)&msg, size, 0, 0); 
    cout << "Receiver Process (PID: " << getpid() << ") receives message: " << msg.message << endl; 
}

int main(int n, char ** arg){
    cout << "Receiver, PID: " << getpid() << ", begins..." << endl;
    // get qid from arg0
    int qid = get_qid(&arg[0][0]);
    receive_msg(qid);
}