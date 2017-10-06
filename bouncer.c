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
    FOREVER()
      counter = counter + 1;
      WRITE (ch, counter);
      printf ("P %d\n", counter);
    ENDFOREVER()
  PROCEND (producer);

  PROC (consumer);
    int local = 0;
    FOREVER() 
      READ (ch, local);
      local += 1;
      printf ("C %d\n", local);
    ENDFOREVER()
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
