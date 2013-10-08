#include "tinycsp.h"
#include <stdio.h>
#include <stdlib.h>

struct list RQ;
    
    
void show_queue (process *p) {
  struct list_elem *e;
  process *ap;
  // printf ("RFQ %8p\n", p);
  
  printf ("\n");
  for (e =  list_begin (&RQ); 
       e != list_end (&RQ);
       e =  list_next (e)) {
     ap = list_entry(e, process, e);
     if (ap == p) {
       printf("- *%c* ", ap->name);
     } else {
       printf("- %c ", ap->name);
     }
  }
  
  printf ("\n");
}  

void remove_from_queue (process *p) {
  // We want to remove this one
  struct list_elem *tgt = &p->e;
  struct list_elem *e;
  // printf ("RFQ %8p\n", p);
  
  DEBUG(printf("Before remove: %d\n", (int)list_size(&RQ)););
  for (e =  list_begin (&RQ); 
       e != list_end (&RQ);
       e =  list_next (e))
    {
      if (e == tgt) {
        p = list_entry(e, process, e);
        p->running = 0;
        DEBUG(printf("Removing %c\n", p->name));

        list_remove(tgt);
        break;
      }
    }
    DEBUG(printf("After remove: %d\n", (int)list_size(&RQ)););
    
    
  // printf ("RFQ DONE\n", p);
}

void add_to_front (process *p) {
  // printf ("ATF %8p\n", p);
  list_push_front (&RQ, &p->e);
  // printf ("ATF DONE\n");
}

void add_to_back (process *p) {
  // printf ("ATF %8p\n", p);
  list_push_back (&RQ, &p->e);
  // printf ("ATF DONE\n");
}

process *
get_next_process (process *p) {
  struct list_elem *tmp;
  process *next;
  
  tmp = list_next(&p->e);
  
  if (tmp == list_tail(&RQ)) {
    
    if (deadlocked == true) {
      printf ("WE HAVE DEADLOCKED\n");
      exit(-1);
    }
    
    DEBUG(printf("next = front of queue\n");)
    deadlocked = true;
    next = list_entry(list_front(&RQ), process, e);
  } else {
    next = list_entry(tmp, process, e);
  }
  
  // printf("Next from %8p is %8p\n", p, next);
  return next;
}

process * 
first (void) {
  return list_entry(list_front(&RQ), process, e);
}
