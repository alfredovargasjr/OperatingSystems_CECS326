#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
using namespace std;

static int PID = 0;

//system memory
struct memory {
  int size;                                                                     //size of Memory, number of blocks
  string* data;                                                                 //array of data in memory, string values in blocks
} mem;

//Memory Block Table of system memory
struct MBT {
  int size;                                                                     //size of the MBT, same as memory size
  int blocksAvailable;
  bool * states;                                                                //state of that memory block, true/false. true = not free
} mbt;

// //Page table for a process, address translation
// struct page_table {
//   int address;                                                                  //location of memory block process starts
//   int pages;                                                                    //number of blocks taken by process
//   int * blocks;                                                                  //array of blocks of process
// };

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

int randomInt(){
  return rand() % 241 + 10;
}

//return unput string for program name
// string Program_Name(){
//   cout << "\nProgram Name: ";
//   string s = "";
//   getline(cin, s);                                                              //get user input, string
//   return s;
// }

//Initialized system memory, 32 blocks reserved for OS
void Initialize_Mem(int mem_size){
  mem.size = mem_size;                                                          //Instantiate the number of blocks in system memory
  mbt.size = mem.size;                                                          //Instantiate the number of blocks in MBT
  mem.data = new string[mem.size];                                              //Instantiate the data array for sytem memory
  mbt.states = new bool[mbt.size];                                              //Instantiate the states array for MBT
  for(int i = 0; i < 32; i++){
    mem.data[i] = "Operating System";
    mbt.states[i] = true;
  }
  mbt.blocksAvailable = mbt.size - 32;
  rq.size = 0;
  cout << "\nMemory Initialized. Memory reserved for OS [32 blocks].\n";
}

//intitate a process to memory, if enough blocks in memory
void initiateProcess(){
  int processSize = randomInt();                                                //random process size
  if(mbt.blocksAvailable >= processSize){                                       //if enough blocks are available in memory for process
    // cout << "\nProcess Name: ";
    pcb * p = new pcb();
    PID++;
    p->process_ID = PID;                                                        //initiate values of pcb
    p->pageSize = processSize;
    p->page_table = new int[processSize];
    p->nextPT = NULL;
    mbt.blocksAvailable = mbt.blocksAvailable - processSize;                    //subtract pcb size to blocks available in MBT
    int placed = 0;
    int curBlock = 32;
    while(placed != processSize){                                               //find blocks that are available, change state
      if(mbt.states[curBlock] == false){
        mbt.states[curBlock] = true;
        p->page_table[placed] = curBlock;
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
  }
  else{
    cout << "\nNot enough blocks in memory for process. Process not initiated.";
  }
}

//print the processes that are in the readt queue
void printProcesses_rq(){
  if(rq.size != 0){                                                             //check to see if ready queue is empty
    pcb * cur = rq.head;                                                          //set cur node as head of rq
    while(cur != NULL){                                                           //iterate through the rq until cur node is null
      cout << "Process ID:" << cur->process_ID << "\nProcess Address: " << cur->address << "\nProcess size: " << cur->pageSize << endl;
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
        delete [] cur->page_table;                                              //delete page table array
        prev->nextPT = cur->nextPT;
        cur->nextPT = NULL;
        mbt.blocksAvailable = mbt.blocksAvailable + cur->pageSize;              //new blocks will be available in memory
        cout << "\nProcess ID [" << cur->process_ID << "] has been terminated." << endl;
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


int main(){
  Initialize_Mem(1024);
  printProcesses_rq();
  initiateProcess();
  cout << "\nEnter to exit the program: ";
  cin.ignore().get();
  delete [] mem.data;
  delete [] mbt.states;
  return 0;
}
