#include <stdio.h>
#include "tinycsp.h"

extern struct list RQ;
    
int main (void) {
  // Declare channels
  static channel ch;
  static process main, producer, consumer;

  // Parameters
  // We could do arrays with struct/union combos...
  // but I think it would waste too much space.
  channel *producer_0;
  channel *consumer_0;

  // Declare the scheduler jump point.
  static void *scheduler;

  // And, the next item to run.
  void *next, *end, *start;
  process *current_process;
  
  // Set up the empty queue
  list_init(&RQ);
    
  // Set up the first process.
  printf ("Adding main\n");
  add_to_front(&main);
  
  // Initialize channels
  CHAN_INIT (ch);
  
  // Set up all the processes with initial labels.
  main.label = &&MAIN;
  
  consumer.label = &&CONSUMER;
  consumer_0 = &ch;
  
  producer.label = &&PRODUCER;
  producer_0 = &ch;

  // Set the end point
  end   = &&hotel_california;

  // Set up the pointer to the scheduler
  scheduler = &&scheduler;
  printf ("Jumping to main...\n");
  goto *(list_entry(list_front(&RQ), process, e)->label);
  
  PRODUCER:
  current_process = &producer;
  do {
    // Set up the process
    // The compiler would lay this out.    
    while (true) {
      PRODUCER_CH:
      current_process->label = &&PRODUCER_CH;
      // Set the next destination
      WRITE (producer_0, 42);
      printf ("<5>\n");
    }        
  } while (0);

  CONSUMER:
  current_process = &consumer;
  do {
    while (true) { 
      int locals[1];
      CONSUMER_CH:
      current_process->label = &&CONSUMER_CH;
      READ (consumer_0, locals[0]);
      printf ("C %d\n", locals[0]);   
    }    
  } while (0);
  
  MAIN:
  current_process = &main;
  // PAR
  // Add everyone to the party.
  // Set up parameters
  add_to_front(&producer);
  add_to_front(&consumer);
  
  // Remove ourself from the queue.
  remove_from_queue(&main);
  // Set the last process in the PAR to be next.

  current_process = &consumer;
  goto *scheduler;
  goto *end;
      
  scheduler:
  do {
    current_process = get_next_process(current_process);
    // sleep (1);
    goto *(current_process->label);

  } while (0);

  
  // You can check out, but you can never leave.
  hotel_california:
  return 0;  
}
