#ifndef __TINYCSPH
#define __TINYCSPH

// #define DEBUG_FLAG

/* ***************************************************************** */
/* CONSTANTS */
#include "list.h"

#define true 1
#define false 0

#ifdef DEBUG_FLAG
#define DEBUG(exp) exp 
#else
#define DEBUG(exp)
#endif

// Matt's cheap O(1) deadlock detection
// We just have to go through the list once.
int deadlocked;

/* ***************************************************************** */
/* CHANNEL HELPERS */

#define CHAN_INIT(ch) \
do { \
  ch.value   = -1; \
  ch.read_ok =  0; \
} while (0);

#define WRITE(label, ch, v) \
do { \
  label: \
  CURRENT_LABEL(label); \
  if (!ch.read_ok) { \
    ch.value   = v; \
    ch.read_ok = 1; \
    DEBUG(fprintf(stdout, "\t" #ch " <- %d\n", v);) \
    deadlocked = false; \
  } \
  SCHEDULE_NEXT(); \
  } while (0);

#define READ(label, ch, v) \
do { \
  label: \
  CURRENT_LABEL(label); \
  if (ch.read_ok) { \
    ch.read_ok = 0; \
    v = ch.value; \
    DEBUG(fprintf(stdout, "\t" #ch " -> " #v "\n");) \
    deadlocked = false; \
  } else { \
    SCHEDULE_NEXT(); \
  } \
} while (0);

#define CURRENT_LABEL(tag) \
  current_process->label = &&tag; \
  DEBUG(fprintf(stdout, " - " #tag "\n");) \

#define GET_PARAM(type, loc) (type (current_process->params[loc]))

#define SCHEDULE_NEXT() \
do { \
  current_process = get_next_process(current_process); \
  goto *(current_process->label); \
} while (0);

#define SCHEDULE(proc) \
do { \
  DEBUG(fprintf(stdout, "SCHEDULING " #proc "\n");) \
  goto *(proc.label); \
} while (0);

/* ***************************************************************** */
/* TYPES */

typedef struct channel_record {
  int value;
  int read_ok : 1;
} channel;

typedef struct process_record {
  struct list_elem e;
  void *label;
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
