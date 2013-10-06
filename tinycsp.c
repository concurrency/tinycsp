#include "tinycsp.h"

struct list RQ;
    
void remove_from_queue (process *p) {
  // We want to remove this one
  struct list_elem *tgt = &p->e;
  struct list_elem *e;
  // printf ("RFQ %8p\n", p);
  
  for (e =  list_begin (&RQ); 
       e != list_end (&RQ);
       e =  list_next (e))
    {
      if (e == tgt) {
        list_remove(e);
        break;
      }
    }
  
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
