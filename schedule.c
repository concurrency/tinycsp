#include <stdio.h>
#include <setjmp.h>
#include "pt.h"

// Dirty tricks on the AVR
// http://tinyurl.com/olmtvyo

#define true 1

// Channels and State
typedef struct channel_record {
  int value;
  int read_ok : 1;
} channel;

static channel ch;
void* scheduler;

#define RUN_QUEUE_LENGTH 2
static void* queue[RUN_QUEUE_LENGTH];

static int counter = 0;
void* next_in_queue () {
  counter = (counter + 1) % RUN_QUEUE_LENGTH;
  return queue[counter]; 
}

int main () {
  void* next;
  
  // Initialize channels
  ch.value    = -1;
  ch.read_ok  = 0;
  
  // Set up the run queue
  queue[0] = &&producer;
  queue[1] = &&consumer;

  // Set up the pointer to the scheduler
  printf ("Jumping to the scheduler...\n");
  scheduler = &&scheduler;
  goto *scheduler;
  
  producer:
  // Could generate while loops to deschedule
  // after 'n' iterations at the bottom.
  while (true) {
    if (!ch.read_ok) {
      ch.read_ok = 1;
      ch.value = 42;
      printf ("P -> 42\n");
    }
      // Complete communication by
      // going back to the scheduler
      goto *scheduler;
    } else {
      // If not ready, deschedule.
      goto *scheduler;
    }
  }

  consumer:
  while (true) {    
    if (ch.read_ok) {
      ch.read_ok = 0;
      printf ("C %d\n", ch.value);
      goto *scheduler;
    } else {
      goto *scheduler;
    }
  }
  
  scheduler:
  next = next_in_queue();
  goto *next;
  
}
