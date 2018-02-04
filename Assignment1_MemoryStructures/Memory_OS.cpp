#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
using namespace std;

static int PID = 0;

//system memory
struct memory {
  int size;                                                                     //size of Memory, number of blocks
  int* data;                                                                    //array of data in memory, string values in blocks
} mem;

//Memory Block Table of system memory
struct MBT {
  int size;                                                                     //size of the MBT, same as memory size
  int blocksAvailable;
  bool * states;                                                                //state of that memory block, true/false. true = not free
} mbt;

//Process Control Block, process info
struct pcb {
  int process_ID;                                                               //ID of proces
  int pageSize;
  int address;
  int * page_table;                                                             //page table array for process
  pcb * nextPT;                                                                 //pointer to next PT
};

//Ready Queue for the processes
struct ready_queue {
  int size;                                                                     //size of queue
  pcb * head;                                                                   //pointer to head
  pcb * tail;                                                                   //pointer to tail
} rq;

//return random integer, for random process size
int randomInt(){
  return rand() % 241 + 10;
}

//Initialized system memory, 32 blocks reserved for OS
void Initialize_Mem(int mem_size){
  mem.size = mem_size;                                                          //Instantiate the number of blocks in system memory
  mbt.size = mem.size;                                                          //Instantiate the number of blocks in MBT
  mem.data = new int[mem.size];                                              //Instantiate the data array for sytem memory
  mbt.states = new bool[mbt.size];                                              //Instantiate the states array for MBT
  for(int i = 0; i < 32; i++){
    mem.data[i] = 0;
    mbt.states[i] = true;
  }
  mbt.blocksAvailable = mbt.size - 32;
  rq.size = 0;
  cout << "\nMemory Initialized. Memory reserved for OS [32 blocks].\n";
}

//print the Memory Block Table
void printMBT(){
  cout << "\nMBT: size[" << mbt.size << "]\tBlocksAvailable[" << mbt.blocksAvailable << "]" << endl;
  for(int i = 0; i < mbt.size; i++){
    cout << "Block[" << i << "]: " << mbt.states[i] << endl;
  }
}

//Print the information of a process object
void printProcessInformation(const pcb & p){
  cout << "\nProcess ID:" << p.process_ID << "\nProcess Address: " << p.address << "\nProcess size: " << p.pageSize << endl;
  for(int i = 0; i < p.pageSize; i++){
    cout << "Page[" << i << "]: " << p.page_table[i] << endl;
  }
  printMBT();
}

//intitate a process to memory, if enough blocks in memory
void initiateProcess(){
  int processSize = randomInt();                                                //random process size
  if(mbt.blocksAvailable >= processSize){                                       //if enough blocks are available in memory for process
    pcb * p = new pcb();
    PID++;
    p->process_ID = PID;                                                        //initiate values of the pcb
    p->pageSize = processSize;
    p->page_table = new int[processSize];
    p->nextPT = NULL;
    mbt.blocksAvailable = mbt.blocksAvailable - processSize;                    //subtract pcb size to blocks available in MBT
    int placed = 0;
    int curBlock = 32;
    while(placed != processSize){                                               //find blocks that are available, change state
      if(mbt.states[curBlock] == false){                                          //if block free (in false state)
        mbt.states[curBlock] = true;                                              //then change state of block and add index to pagetable
        p->page_table[placed] = curBlock;
        mem.data[curBlock] = p->process_ID;
        placed++;
      }
      curBlock++;
    }
    p->address = p->page_table[0];                                              //save the first page as address block
    if(rq.size == 0){                                                           //check to see if ready queue is empty
      rq.head = p;                                                                //if true, then p will become head/tail
      rq.tail = p;
    }
    else{                                                                         //if flase, then head and tail exist, have tail.next be p and tail = p
      rq.tail->nextPT = p;
      rq.tail = p;
    }
    rq.size++;                                                                  //add size to the ready queue
    printProcessInformation(*p);
  }
  else{
    cout << "\nNot enough blocks in memory for process. Process not initiated.";
  }
}

//print the processes that are in the readt queue
void printProcesses_rq(){
  cout << "\nReady Queue Processes: " << endl;
  if(rq.size != 0){                                                             //check to see if ready queue is empty
    pcb * cur = rq.head;                                                          //set cur node as head of rq
    while(cur != NULL){                                                           //iterate through the rq until cur node is null
      cout << "\nProcess ID:" << cur->process_ID << "\nProcess Address: " << cur->address << "\nProcess size: " << cur->pageSize << endl;
      cur = cur->nextPT;
    }
  }
}

//terminate the process of userinput PID, free the allocated memory block in memory
void terminateProcess(int terminatePID){
  if(rq.size != 0){                                                             //check to see if ready queue is empty
    pcb * cur = rq.head;
    pcb * prev = NULL;
    while(cur != NULL){                                                         //itereate through the rq to find userinput PID
      if(cur->process_ID == terminatePID){                                        //if userinput PID found...
        for(int i = 0; i < cur->pageSize; i++){                                   //itereate through the page_table to chance memory block states
          mbt.states[cur->page_table[i]] = false;                                   //mbt.states[block of current pagetable[i]]
        }
        delete [] cur->page_table;

        if(prev != NULL){
          prev->nextPT = cur->nextPT;
        }                                             //delete page table array
        // cur->nextPT = NULL;
        mbt.blocksAvailable = mbt.blocksAvailable + cur->pageSize;              //new blocks will be available in memory
        cout << "\nProcess ID [" << cur->process_ID << "] has been terminated." << endl;
        if(rq.head == cur){
          rq.head = cur->nextPT;
        }
        if(rq.tail == cur){
          if(prev != NULL){
            rq.tail = prev;
          }
        }
        delete cur;
        rq.size--;
        return;
      }
      prev = cur;
      cur = cur->nextPT;
    }
    cout << "\nProcess ID was not found.";
  }
  return;
}

//clear the process ready queue
void clearProcessQueue(){
  while(rq.head != NULL){
    int pid = rq.head->process_ID;
    terminateProcess(pid);
  }
}

//user input menu for process management
void menu(){
  int input = 0;
  int pid_Terminate = 0;
  int exitPrompt = 0;
  //stay in loop untill user prompts for return
  while(true){
    cout << "\nMenu:\n[1]\tInitiate A Process\n[2]\tPrint Processes In Ready Queue\n[3]\tTerminate A Process\n[0]\tExit\n\nChoice: ";
    cin >> input;
    switch(input){
      case 1: initiateProcess();
              break;
      case 2: printProcesses_rq();
              break;
      case 3: cout << "\nEnter Process ID To Terminate: ";
              cin >> pid_Terminate;
              terminateProcess(pid_Terminate);
              break;
      case 0:{
        if(rq.size != 0){
          while(true){
            cout << "\nProcesses Ready Queue Is Not Empty. Are You Sure You Want To Exit? ([1] Yes / [0] No)\nChoice: ";
            cin >> exitPrompt;
            switch(exitPrompt){
              case 1: clearProcessQueue();
                      return;
              case 0: return;
              default: cout << "\nInvalid Input.";
            }
          }
        }
      } return;
      default: cout << "\nInvalid Input." <<  endl;
    }
  }
}

//Program Start
int main(){
  Initialize_Mem(1024);                                                         //Initialize the "pseudo memory"
  menu();                                                                       //Run the UI for process menu
  delete [] mem.data;                                                           //delete dynamic memory
  delete [] mbt.states;
  cout << "\n\nProgram Exit." << endl;
  return 0;
}
