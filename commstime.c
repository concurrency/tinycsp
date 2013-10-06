#include <stdio.h>
#include "pt.h"

#define forever 1
#define NOT_READY -1
#define READY 1
#define MESSAGE 42

#define SET(word, bit) (word |= (1 << bit))
#define CLEAR(word, bit) (word &= ~(1 << bit))
#define TOGGLE(word, bit) (word ^= (1 << bit))

// Channels and State
typedef struct channel_record {
  int value;
  int read : 1;
  int write : 1;
} channel;

// Channels used in this program
static channel a;
static channel b;
static channel c;
static channel d;

// Processes
static struct pt main_prefix, main_delta, main_succ, main_consume;
static struct pt prefix_id;

#define READ(thread, channel, v) \
do { \
  PT_WAIT_UNTIL(thread, channel->read); \
  *v = channel->value; \
  channel->read = 0; \
} while (0)
  
#define WRITE(thread, channel, v) \
do { \
  PT_WAIT_UNTIL(thread, channel->write); \
  channel->value = *v; \
  channel->write = 0; \
} while (0)

static int id_n;
int id (struct pt *th, channel* in, channel *out, int* id_n) {
  PT_BEGIN (th);  
  while (forever) {
    READ (th, in, id_n);
    WRITE (th, out, id_n);
  }
  PT_END (th);
}

int prefix (struct pt *th, int* n, channel *in, channel *out) {
  
  PT_BEGIN (th);
  
  while (forever) {
    WRITE (th, out, n);
    id (&prefix_id, in, out, &id_n);
  }
  PT_END (th);
}

static int delta_tmp;
int delta (struct pt *th, channel *in, channel *out1, channel *out2, int* delta_tmp) {
  PT_BEGIN (th);
  while (forever) {
    READ (th, in, delta_tmp);
    WRITE (th, out1, delta_tmp);
    WRITE (th, out2, delta_tmp);
  }
  PT_END (th);
}

static int succ_v;
int succ (struct pt *th, channel* in, channel* out, int* succ_v) {
  PT_BEGIN (th);
  while (forever) {
    READ (th, in, succ_v);
    *succ_v = *succ_v + 1;
    WRITE (th, out, succ_v);
  }
  PT_END (th);
}

static int consume_v;
int consume (struct pt *th, int* n, channel *in, int* consume_v) {
  PT_BEGIN (th);
  while (forever) {
    READ (th, in, consume_v);
    *n--;
    if (*n == 0) {
      printf("Done.");
    }
  }
  PT_END (th);
}



// We can't have local variables. So, we need
// to pass all local state into the function?
// Meaning, we need the compiler to allocate space for
// every single local variable for every instance of succ.
// (For example)

int main () {

  
  // Init all of the processes
  PT_INIT (&main_prefix);
  PT_INIT (&main_delta);
  PT_INIT (&main_succ);
  PT_INIT (&main_consume);
  
  // Even the nested processes.
  PT_INIT (&prefix_id);
  
  // I have to think about how constants are handled.
  // That is, VAL INTs in my current READ/WRITE framework
  // do not work... perhaps special forms?
  int zero = 0;
  int loop_count = 1000;
  
  // Run these forever... but, that is really not
  // the semantics of PAR.
  while (forever) {
    prefix (&main_prefix, &zero, &b, &a);
    
    delta (&main_delta, &a, &c, &d, &delta_tmp);
    succ (&main_succ, &c, &b, &succ_v);
    consume (&main_consume, &loop_count, &d, &consume_v);
  }
}
