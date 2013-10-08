#include <stdio.h>
#include "tinycsp.h"

extern struct list RQ;
    
int main (void) {
  // Declare channels
  static channel ch;
  static process main, producer, consumer;
  process *current_process;

  // Set up the empty queue
  list_init(&RQ);
    
  // Set up the first process.
  printf ("Adding main\n");
  add_to_front(&main);
  
  // Initialize channels
  CHAN_INIT (ch);
  // Set the labels for the processes.
  SET_LABEL (main);
  SET_LABEL (producer);
  SET_LABEL (consumer);  

  printf ("Jumping to main...\n");
  goto *(main.label);
  
  PROC (producer);
    while (true) {
      WRITE (PRODUCER_WRITE_CH, ch, 42);
    }        
  PROCEND (producer);

  PROC (consumer);
    while (true) { 
      int local;
      READ (CONSUMER_READ_CH, ch, local);
      printf ("C %d\n", local);   
    }    
  PROCEND (consumer);
  
  PROC (main);
    // PAR
    // Add everyone to the party.
    // Set up parameters
    add_to_back (&producer);
    add_to_back (&consumer);   
     
  PROCEND (main);
  
  return 0;
}
