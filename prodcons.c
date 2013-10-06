#include <stdio.h>
#include "pt.h"

#define true 1
#define NOT_READY -1
#define READY 1
#define MESSAGE 42

int ch;
char ch_state;

static int total = 10;
PT_THREAD (producer (struct pt *pth)) {
  PT_BEGIN (pth);
  total--;
  
    PT_WAIT_UNTIL(pth, ch_state == NOT_READY);
    ch = MESSAGE;
    printf ("P %d\n", ch);
    ch_state = READY;
  
  PT_END (pth);
}

PT_THREAD (consumer (struct pt *pth)) {
  PT_BEGIN (pth);
  
  while (1) {
    PT_WAIT_UNTIL(pth, ch_state == READY);
    printf ("C %d\n", ch);
    ch_state = NOT_READY;
  }
  
  PT_END (pth);  
}

struct pt pth1, pth2;

int main () {
  int par_count;
  PT_INIT (&pth1);
  PT_INIT (&pth2);
  
  ch_state = NOT_READY;
  
  do {
    par_count = 0;
    par_count += consumer (&pth2);
    par_count += producer (&pth1);
  } while (par_count != 2);
}
