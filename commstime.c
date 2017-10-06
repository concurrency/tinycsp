#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>

#include "tinycsp.h"
#include <time.h>


extern struct list RQ;
process *current_process;

int main (void) {	
	/* Declare channels 
   * All of the structures I've used are mostly useless as exemplified
   * in this example. That said, I wonder if they will help me later,
   * if I keep exploring and anything grows in complexity.
 	 */
  static channel a, b, c, d;

  /* Declare processes.
   * I might decide to wipe out this structure, or put more in it.
   * Currently, I'm undecided. 
   */  
  static process prefix, delta, seqdelta, succ, consume, main, id;
  
  /* The runqueue.
   * This is probably heavier than needed, but it is a clean implementation
   * of a doubly-linked list that I used recently in my OS course.
   * From the PintOS project.
   */
  list_init(&RQ);
    
  /* Get the 'main' process on the runqueue.
   */
  DEBUG(printf ("Adding main\n");)
  add_to_front(&main);
  
  /* Initialize channels.
   * The channel status bits are all set so that they're ready
   * for writing, and the value on the channel is set to -1.
   */
  CHAN_INIT (a);
  CHAN_INIT (b);
  CHAN_INIT (c);
  CHAN_INIT (d);

  /* VAL INT declarations
   * Declaring all VAL INTs as constants actually made things faster.
   * No surprise. If a compiler is generating code, this is an easy
   * thing to lift out.
   */
  #define prefix_0         0
  #define consume_0  1000000
  
  /* Set processes' GOTO labels.
   * Sets up the process structures with their initial jump targets.
   */
  SET_LABEL(id);
  id.name = 'I';
  id.running = 1;
  SET_LABEL(prefix);    
  prefix.name = 'P';
  prefix.running = 1;
  SET_LABEL(delta);
  delta.name = 'D';
  delta.running = 1;
  SET_LABEL(seqdelta);
  seqdelta.name = 'D';
  seqdelta.running = 1;
  SET_LABEL(succ);
  succ.name = 'S';
  succ.running = 1;
  SET_LABEL(consume);
  consume.name = 'C';
  consume.running = 1;
  SET_LABEL(main);
  main.name = 'M';
  main.running = 1;

  DEBUG(printf ("Jumping to main...\n");)
  goto *(main.label); 
  
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

  PROC (id);
    int local;
    while (true) {
      READ (b, local);
      WRITE (a, local);
    }
  PROCEND (id);
    
  
  /*
  --{{{  PROC prefix (VAL INT n, CHAN OF INT in, out)
  PROC prefix (VAL INT n, CHAN OF INT b?, a!)
    SEQ
      a ! n
      id (b, a)
  :
  --}}}
  */

  PROC (prefix);
    WRITE (a, prefix_0);

    // Executing a process is the same as adding
    // it to the queue? Not really. Probably should
    // jump to it.
    add_to_back (&id);
    // Remove ourselves
  PROCEND (prefix);
  
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
  PROC (delta);
    int local_0;
    process p1, p2;
    
    while (true) {
      DEBUG(printf ("<------ DELTA TOP ------>\n"););
      READ (a, local_0);
      
      // START OF PAR
      // ANONYMOUS PROCESSES
      // Add all the processes first.
      p1.label = &&DELTA_WRITE_C_1;
      p1.name = '1';
      p1.running = 1;
      add_to_back(&p1);
      
      p2.label = &&DELTA_WRITE_D_2;      
      p2.name = '2';
      p2.running = 1;
      add_to_back(&p2);
      // After loading up the PAR, schedule whomever is next.
      // We'll run these eventually. This differs from KRoC, but
      // I think the semantics will work out...
      SCHEDULE_NEXT();
    
      NAMED_WRITE (DELTA_WRITE_D_2, d, local_0);
      remove_from_queue(&p2);
      goto DELTA_END_PAR;
      
      NAMED_WRITE (DELTA_WRITE_C_1, c, local_0);
      remove_from_queue(&p1);
      goto DELTA_END_PAR;
      
      DELTA_END_PAR:
      CURRENT_LABEL(DELTA_END_PAR);

      if (p1.running | p2.running) {
        SCHEDULE_NEXT();
      }

      DEBUG(printf ("<------ DELTA BOTTOM ------>\n"););
    }
    
  PROCEND (delta);

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
  
  PROC (seqdelta);
    int local_0;
    
    FOREVER()
      READ (a, local_0);
      WRITE (d, local_0);   
      WRITE (c, local_0);   
    ENDFOREVER()
  PROCEND (seqdelta);

  
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

  PROC (succ);
    int local_0;
    while (true) {
      READ (c, local_0);
      WRITE (b, (local_0 + 1));
    }
  PROCEND (succ);
  
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
  

  PROC (consume);
    int i, local_value;
    struct timeval local_ts0, local_ts1;
     
    // int uptime = (int) (clock() / (CLOCKS_PER_SEC / 1000));
    /* Warm up */
    
    for (i = 0 ; i < 1000 ; i++) {
      int local_value;
      READ (d, local_value);
    }

    
    while (true) {
      uint diff = 0;
      uint ctx = 0;
      // Should reading the time deschedule?
      // clock_gettime(CLOCK_REALTIME, &local_ts0);
      gettimeofday(&local_ts0, NULL);
      for (i = 0 ; i < consume_0 ; i++) {
        READ (d, local_value);
        
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
    
  PROCEND (consume);
  
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
  
  PROC (main);
    current_process = &main;
    // PAR
    // Add everyone to the party.
    // Really should now check that everyone is still running 
    // See the anonymous processes in delta for how it should work.
    add_to_back(&prefix);
    add_to_back(&succ);

    // CFLAGS=-DUSESEQDELTA make commstime && ./commstime.exe
    #ifdef USESEQDELTA
       printf ("Using 'seqdelta'\n");
       add_to_back(&seqdelta); 
    #else
       printf ("Using 'delta'\n");
      add_to_back(&delta);
    #endif

    add_to_back(&consume);
  
    // Set the last process in the PAR to be next.
    current_process = &consume;
  PROCEND (main);
  
  // You can check out, but you can never leave.
  hotel_california:
  return 0;  
}
