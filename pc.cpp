#include <stdio.h>
#include <stdlib.h>

// ... processes ...

class Process {
public:
    virtual void run() = 0;
    Process *next;
};

Process *next_process = NULL;

#define PROC(name) \
    class name : public Process { \
    public: \
        int line_; \
        name() : line_(0) {}

// local variables will go here, i.e. end up as class members

#define PROC_ARGS(...) \
        void init(__VA_ARGS__) {

// initialisation code will go here

#define PROC_BODY \
        } \
        void run() { \
            switch (line_) { \
            case 0:

// code for the proc will go here (with DESCHEDULE adding extra cases)

#define PROC_END \
            } \
        } \
    };

// This trick courtesy of Simon Tatham:
//   http://www.chiark.greenend.org.uk/~sgtatham/coroutines.html
#define DESCHEDULE \
    do { \
        line_ = __LINE__; \
        return; \
        case __LINE__: \
            ; \
    } while (false)

// ... scheduler ...
      
#define SCHEDULE(proc) \
  do { \
    (proc)->next = next_process; \
    next_process = proc; \
  } while (0)

#define START_PROC(proc, ...) \
    do { \
        proc.init(__VA_ARGS__); \
        SCHEDULE (&proc); \
    } while (false)

int scheduler() {
  while (true) {
    if (!next_process) {
      return -1;
    }

    next_process->run();
    next_process = next_process->next;
  }
}

// ... channels ...

class Channel {
public:
    Process *waiting_;
    int *value_;
    Channel() : waiting_(0), value_(0) {}
};

// in occam the pointer to the value is actually stored in the process...

#define CHAN_WRITE(chan, value) \
    do { \
        if (chan->waiting_) { \
            *(chan->value_) = value; \
            SCHEDULE (chan->waiting_); \
            chan->waiting_ = 0; \
        } else { \
            chan->value_ = &value; \
            chan->waiting_ = this; \
            DESCHEDULE; \
        } \
    } while (false)

#define CHAN_READ(chan, value) \
    do { \
        if (chan->waiting_) { \
            value = *(chan->value_); \
            SCHEDULE (chan->waiting_); \
            chan->waiting_ = 0; \
        } else { \
            chan->value_ = &value; \
            chan->waiting_ = this; \
            DESCHEDULE; \
        } \
    } while (false)

// (Note those two are identical except for the direction the data is copied
// once the two processes have met. You could have a direction argument...)

// ... program ...

// Having to list the arguments three times is pretty ugly.
// There are probably some preprocessor gymnastics to get around that!

PROC (Producer)
    int counter;
    int id;
    Channel *chan;
PROC_ARGS (int id_, Channel *chan_)
    id = id_;
    chan = chan_;
PROC_BODY
    counter = 0;
    while (true) {
        ++counter;
        printf ("Producer %d sending %d\n", id, counter);
        CHAN_WRITE (chan, counter);
    }
PROC_END

PROC (Consumer)
    int local;
    int id;
    Channel *chan;
PROC_ARGS (int id_, Channel *chan_)
    id = id_;
    chan = chan_;
PROC_BODY
    while (true) {
        CHAN_READ (chan, local);
        printf ("Consumer %d received %d\n", id, local);
    }
PROC_END

int main() {
    Channel chan1, chan2;
    Producer p1, p2;
    Consumer c1, c2;
    
    /*
    do { 
      p1.init(1, &chan1); 
      do { 
        // Need to parenthesize the proc in the macro.
        // & has lower precedence than ->.
        &p1->next = next_process; 
        next_process = &p1; 
      } while (0); 
    } while (false);
    */
    
    START_PROC (p1, 1, &chan1);
    START_PROC (c1, 1, &chan1);
    START_PROC (p2, 2, &chan2);
    START_PROC (c2, 2, &chan2);

    return scheduler();
}
