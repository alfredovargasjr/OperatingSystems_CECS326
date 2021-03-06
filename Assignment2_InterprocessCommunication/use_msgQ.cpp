// use_msgQ.cpp

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
        char greeting[50]; // message content
};

/*
-create the first child process
-use system calls to write and read messages from message queue
*/
void child_proc_one(int qid) {
        //create the buffer using data structure
        buf msg;
        int size = sizeof(msg)-sizeof(long);
        
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
        msgrcv(qid, (struct msgbuf *)&msg, size, 113, 0);                                    
        cout << getpid() << ": gets message" << endl;
        //print greeeting from buffer (msg)
        msg.greeting << endl;
        //append string to msg.greeting
        strcat(msg.greeting, " and Adios.");
        cout << getpid() << ": sends reply" << endl;
        msg.mtype = 114;         // prepare message with type mtype = 114
        
        /*
        system call: msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg)
                -msqid: id of the message queue created by the OS
                -*msgp: pointer to the defined structure, (buf buffer)
                -msgsz: size of msgp message
                -msgflg: message flag behavior when message length is bigger than msgsz
        -add copy of message from buffer (msg) to the message queue qid
        */
        msgsnd(qid, (struct msgbuf *)&msg, size, 0);
        cout << getpid() << ": now exits" << endl;     
}

/*
-Create a second child process
-use system call to write and read messages from message queue
 */
void child_proc_two(int qid) {
        //create a buffer using the data structure
        buf msg;
        int size = sizeof(msg)-sizeof(long);
        // sending a message that will never be read
        msg.mtype = 12;
        // copy string to msg.greeting
        strcpy(msg.greeting, "Fake message");
        // copy message from buffer (msg) to the message queue
        msgsnd(qid, (struct msgbuf *)&msg, size, 0);

        // prepare my message to send
        strcpy(msg.greeting, "Hello there");

        cout << getpid() << ": sends greeting" << endl;
        msg.mtype = 113;         // set message type mtype = 113
       /*
        system call: msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg)
                -msqid: id of the message queue created by the OS
                -*msgp: pointer to the defined structure, (buf buffer)
                -msgsz: size of msgp message
                -msgflg: message flag behavior when message length is bigger than msgsz
        -add copy of message from buffer (msg) to the message queue qid
        */        
        // copy message from buffer to message queue       
        msgsnd(qid, (struct msgbuf *)&msg, size, 0);
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
        // remove message from message queue and place it in buffer
        msgrcv(qid, (struct msgbuf *)&msg, size, 114, 0);
        cout << getpid() << ": gets reply" << endl;
        // print message from gretting in buffer
        cout << "reply: " << msg.greeting << endl;
        cout << getpid() << ": now exits" << endl;
}
 
//Message Queue on Linux
int main() {

        /*
        System call: msgget(key_t key, int msgflg)
                key: key arguement for queue
                bits: permissions of the message queue
        */
		//system call: msgget(key_t key,int msgflg)
        //acquire a message queue from the operating system, message queue id
		//	-IPC_PRIVATE: create message queue if key has value IPC_PRIVATE
		//	-IPC_EXCL|IPC_CREAT: specific message flag
		//	-0600: permissions given to the message queue
        int qid = msgget(IPC_PRIVATE, IPC_EXCL|IPC_CREAT|0600);

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
		//	-IPC_RMID: remove qid message queue
        msgctl(qid, IPC_RMID, NULL);
        cout << "parent proc: " << getpid()
                << " now exits" << endl;
        exit(0); //process termination
}