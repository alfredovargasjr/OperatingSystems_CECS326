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
    //return array as an integer
    return atoi(c);
}

void send_msg(int qid){
    cout << "\nEnter a Message to be sent from Sender Process (PID: " << getpid() << "): ";
    string s;
    cin >> s;
    char m[s.length()];
    strcpy(m, s.c_str());
    // create the buffer
    buf msg;
    //set the message type
    msg.mtype = 10;
    int size = sizeof(msg)-sizeof(long);
    //copy m onto buffer
    strcat(msg.message, m);
    /*
    system call: msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg)
            -msqid: id of the message queue created by the OS
            -*msgp: pointer to the defined structure, (buf buffer)
            -msgsz: size of msgp message
            -msgflg: message flag behavior when message length is bigger than msgsz
    -add copy of message from buffer (msg) to the message queue qid
    */
    msgsnd(qid, (struct msgbuf *)&msg, size, 0);
    cout << "Sender Process (PID: " << getpid() << ") sends message: " << s << endl;
}

//main function with passed arguement from execl(path, arg0, arg1, ....) system call
int main(int n, char ** arg){
    cout << "Sender, PID: " << getpid() << ", begins..." << endl;
    // get arguement from system call, call function to return int value of arg0
    int qid = get_qid(&arg[0][0]);
    send_msg(qid);
}