#ifndef __TINYCSPH
#define __TINYCSPH

/* ***************************************************************** */
/* CONSTANTS */
#include "list.h"

#define true 1
#define false 0

#define DEBUG_FLAG 0
#define DEBUG(exp) \
if (DEBUG_FLAG) { \
  exp; \
} \

/* ***************************************************************** */
/* CHANNEL HELPERS */

#define CHAN_INIT(ch) \
do { \
  ch.value   = -1; \
  ch.read_ok =  0; \
} while (0);

#define WRITE(ch, v) \
do { \
  DEBUG(fprintf(stdout, "\tW[" #ch "]\n");) \
  if (!ch.read_ok) { \
    ch.value   = v; \
    ch.read_ok = 1; \
  } \
  goto *scheduler; \
  } while (0);

#define READ(ch, v) \
do { \
  DEBUG(fprintf(stdout, "\tR[" #ch "]\n");) \
  if (ch.read_ok) { \
    ch.read_ok = 0; \
    v = ch.value; \
  } else { \
    goto *scheduler; \
  } \
} while (0);

static int counter = 0;

#define CURRENT_LABEL(tag) \
counter++; \
  current_process->label = &&tag; \
  DEBUG(fprintf(stdout, #tag ": %d\n", counter);) \

#define GET_PARAM(type, loc) (type (current_process->params[loc]))

/* ***************************************************************** */
/* TYPES */

typedef struct channel_record {
  int value;
  int read_ok : 1;
  void *next;
} channel;

typedef struct process_record {
  struct list_elem e;
  int *params;
  volatile void *label;
} process;

/* ***************************************************************** */
/* QUEUE HLPERS */
    
void remove_from_queue (process *p);
void add_to_front (process *p);
void add_to_back (process *p);
process * get_next_process (process *p);
process * first (void);


// Dirty tricks on the AVR
// http://tinyurl.com/olmtvyo

#endif /* __TINYCSPH */
