#include <iostream>
#include <string>
using namespace std;

//system memory
struct memory {
  int size;                                                                     //size of Memory, number of blocks
  string* data;                                                                 //array of data in memory, string values in blocks
} mem;

//Memory Block Table of system memory
struct MBT {
  int size;                                                                     //size of the MBT, same as memory size
  bool* states;                                                                  //state of that memory block, true/false. true = not free
} mbt;

//Page table for a process, address translation
struct page_table {
  int address;                                                                  //location of memory block process starts
  int pages;                                                                    //number of blocks taken by process
  int *blocks;                                                                  //array of blocks of process
};

//Process Control Block, process info
struct pcb {
  int process_ID;                                                               //ID of proces
  page_table *pt;                                                               //pointer to page table of process
  pcb * nextPT;                                                                 //pointer to next PT
};

//Ready Queue for the processes
struct ready_queue {
  int size;                                                                     //size of queue
  pcb * head;                                                                   //pointer to head
  pcb * tail;                                                                   //pointer to tail
};


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
  cout << "\nMemory Initialized. Memory reserved for OS.\n";
}

int main(){
  Initialize_Mem(1024);
  cout << "\nEnter to exit the program: ";
  cin.ignore().get();
  delete [] mem.data;
  delete [] mbt.states;
  return 0;
}
