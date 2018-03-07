#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

using namespace std;

int main(int n, char ** arg){
    cout << "Sender, PID: " << getpid() << ", begins..." << endl;
    cout << "\nEnter a Message to be sent from Sender Process (PID: " << getpid() << "): ";
    string message;
    cin >> message;
    for(int i = 0; i < sizeof(arg); i++){
        cout << & arg[i];
    }
}