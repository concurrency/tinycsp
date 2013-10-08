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
  main.running = 1;
  SET_LABEL (producer);
  SET_LABEL (consumer);  

  printf ("Jumping to main...\n");
  goto *(main.label);
  
  PROC (producer);
    int counter = 0;
    while (true) {
      counter = counter + 1;
      WRITE (PRODUCER_WRITE_CH, ch, counter);
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
    producer.running = 1;
    
    add_to_back (&consumer);   
    consumer.running = 1;

    // At PROCEND, we get removed from the runqueue, and
    // running is set to 0.
  PROCEND (main);
  
  return 0;
}
