#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

#include "tinycsp.h"
#include <time.h>


extern struct list RQ;
process *current_process;

int main (void) {
  // Declare channels
  static channel a, b, c, d;
  // Declare processes
  static process prefix, delta, seqdelta, succ, consume, main, id;

  // And, the next item to run.
  void *next, *end, *start;
  
  // Set up the empty queue
  list_init(&RQ);
    
  // Set up the first process.
  DEBUG(printf ("Adding main\n");)
  add_to_front(&main);
  
  // Initialize channels
  CHAN_INIT (a);
  CHAN_INIT (b);
  CHAN_INIT (c);
  CHAN_INIT (d);
  
  // Set up all the processes with initial labels
  id.label   = &&ID;
  
  prefix.label = &&PREFIX;
  // VAL INTs can become defines
  #define prefix_0 0
    
  delta.label  = &&DELTA;
  
  seqdelta.label = &&SEQDELTA;
    
  succ.label   = &&SUCC;
  
  consume.label = &&CONSUME;
  // VAL INTs can become defines
  #define consume_0  1000000
  
  main.label = &&MAIN;
  
  // Set the end point
  end   = &&hotel_california;

  DEBUG(printf ("Jumping to main...\n");)
  goto *(list_entry(list_front(&RQ), process, e)->label);
  
  /* 
  --{{{  PROC id (CHAN OF INT in, out)
  PROC id (CHAN OF INT in, out)
    WHILE TRUE
      INT n:
      SEQ
        in ? n
        out ! n
  :
  --}}}
  */
  ID:
  current_process = &id;
  DEBUG(printf("ID\n");)
  do {
    int local;
    while (true) {
      READ (ID_READ_CH, b, local);
      WRITE (ID_WRITE_CH, a, local);
    }
    SCHEDULE_NEXT();
  } while (0);
  
  /*
  --{{{  PROC prefix (VAL INT n, CHAN OF INT in, out)
  PROC prefix (VAL INT n, CHAN OF INT b?, a!)
    SEQ
      a ! n
      id (b, a)
  :
  --}}}
  */
  PREFIX:
  current_process = &prefix;
  DEBUG(printf ("PREFIX\n");)
  do {

    WRITE (PREFIX_WRITE_A, a, prefix_0);
    // Executing a process is the same as adding
    // it to the queue? Not really. Probably should
    // jump to it.
    add_to_front (&id);
    goto *id.label;
    
    // The last thing a process does is dive into the
    // scheduler, I suppose.
    SCHEDULE_NEXT();
  } while (0);
  
  /*
  --{{{  PROC delta (CHAN OF INT in, out.0, out.1)
  PROC delta (CHAN OF INT in, out.0, out.1)
    WHILE TRUE
      INT n:
      SEQ
        in ? n
        PAR
          out.0 ! n
          out.1 ! n
  :
  --}}}
  a, c, d
  */
  DELTA:
  current_process = &delta;
  DEBUG(printf ("DELTA\n");)
  do {
    int local_0;
    process p1, p2;
    
    while (true) {
      READ (DELTA_READ_A, a, local_0);
      
      // START OF PAR
      p1.label = &&DELTA_WRITE_C;
      p2.label = &&DELTA_WRITE_D;
      add_to_back(&p1);
      add_to_back(&p2);
      
      // ANONYMOUS PROCESSES?
      WRITE (DELTA_WRITE_C, c, local_0);
      remove_from_queue(&p1);
      SCHEDULE_NEXT();

      WRITE (DELTA_WRITE_D, d, local_0);
      remove_from_queue(&p2);
      SCHEDULE_NEXT();
      
    }
  } while (0);

  /*
  --{{{  PROC seq.delta (CHAN OF INT in, out.0, out.1)
  PROC seq.delta (CHAN OF INT in, out.0, out.1)
    WHILE TRUE
      INT n:
      SEQ
        in ? n
        out.0 ! n
        out.1 ! n
  :
  --}}}
  */
  
  SEQDELTA:
  current_process = &seqdelta;
  DEBUG(printf ("SEQ DELTA\n");)
  do {
    int local_0;
    
    while (true) {
      READ (SEQDELTA_READ_A, a, local_0);
      /* THIS BREAKS IF I SEND ON CHANNEL C FIRST */
      WRITE (SEQDELTA_WRITE_D, d, local_0);   
      WRITE (SEQDELTA_WRITE_C, c, local_0);   
    }
    
    SCHEDULE_NEXT();
  } while (0);
  

  
  /*
  --{{{  PROC succ (CHAN OF INT in, out)
  PROC succ (CHAN OF INT in, out)
    WHILE TRUE
      INT n:
      SEQ
        in ? n
        out ! n PLUS 1
  :
  --}}}
  */
  SUCC:
  current_process = &succ;
  DEBUG(printf ("SUCC\n");)
  do {
    int local_0;
    while (true) {
      READ (SUCC_READ_C, c, local_0);
      WRITE (SUCC_WRITE_B, b, (local_0 + 1));
    }
  } while (0);
  
  /*
  --{{{  PROC consume (VAL INT n.loops, CHAN OF INT in, CHAN OF BYTE out)
  PROC consume (VAL INT n.loops, CHAN OF INT in, CHAN OF BYTE out)
    TIMER tim:
    INT t0, t1:
    INT value:
    SEQ
      --{{{  warm-up loop
      VAL INT warm.up IS 16:
      SEQ i = 0 FOR warm.up
        in ? value
      --}}}
      WHILE TRUE
        SEQ
          tim ? t0
          --{{{  bench-mark loop
          SEQ i = 0 FOR n.loops
            in ? value
          --}}}
          tim ? t1
          --{{{  report
          VAL INT millisecs IS t1 MINUS t0:
          VAL INT32 microsecs IS (INT32 millisecs) * 1000:
          SEQ
            out.string ("Last value received = ", 0, out)
            out.int (value, 0, out)
            out.string ("*c*n", 0, out)
            out.string ("Time = ", 0, out)
            out.int (millisecs, 0, out)
            out.string (" ms*c*n", 0, out)
            out.string ("Time per loop = ", 0, out)
            out.int (INT (microsecs / (INT32 n.loops)), 0, out)
            out.string (" us*c*n", 0, out)
            out.string ("Context switch = ", 0, out)
            out.int (INT ((microsecs / (INT32 n.loops)) / 4), 0, out)
            out.string (" us*c*n*n", 0, out)
          --}}}
  :
  --}}}
  */
  
  CONSUME:
  current_process = &consume;
  DEBUG(printf ("CONSUME\n");)
  do {
    int i, local_value;
    struct timeval local_ts0, local_ts1;
     
    // int uptime = (int) (clock() / (CLOCKS_PER_SEC / 1000));
    /* Warm up */
    
    for (i = 0 ; i < 1000 ; i++) {
      int local_value;
      READ (CONSUME_READ_D1, d, local_value);
    }

    
    while (true) {
      uint diff = 0;
      uint ctx = 0;
      // Should reading the time deschedule?
      // clock_gettime(CLOCK_REALTIME, &local_ts0);
      gettimeofday(&local_ts0, NULL);
      for (i = 0 ; i < consume_0 ; i++) {
        READ (CONSUME_READ_D2, d, local_value);
        
      }
      // clock_gettime(CLOCK_REALTIME, &local_ts1);
      gettimeofday(&local_ts1, NULL);
      diff = ((uint)local_ts1.tv_usec * 1000) - ((uint)local_ts0.tv_usec * 1000);
      ctx = ((diff / consume_0) / 4);
      
      // Cheating; I don't want to handle rollover right now.
      if (ctx < 300) {
        printf ("dT ns: %u\n", diff);
        printf ("Ctx Switch: %u\n", ctx);        
      }
    }
    
  } while (0);
  
  /*

    CHAN OF INT a, b, c, d:
    PAR
      prefix (0, b, a)
      IF
        use.seq.delta
          seq.delta (a, c, d)    -- the one defined above
        TRUE
          delta (a, c, d)        -- the one that does a parallel output
      succ (c, b)
      consume (10000, d, screen)
  */
  
  MAIN:
  current_process = &main;
  // PAR
  // Add everyone to the party.
  add_to_front(&prefix);
  add_to_front(&succ);

  // CFLAGS=-DUSESEQDELTA make commstime && ./commstime.exe
#ifdef USESEQDELTA
     add_to_front(&seqdelta); 
#else
    add_to_front(&delta);
#endif

  add_to_front(&consume);
  
  // Remove ourself from the queue.
  remove_from_queue(&main);
  
  // Set the last process in the PAR to be next.
  current_process = &consume;
  SCHEDULE_NEXT();
  goto *end;
  
  // You can check out, but you can never leave.
  hotel_california:
  return 0;  
}
