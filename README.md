# tinycsp

A tiny CSP-ish channel implementation using GOTOs and some macros.

## Compiling

<pre>
$ make commstime
$ ./commstime.exe
</pre>

You can also do

<pre>
$ CFLAGS=-DUSESEQDELTA make commstime
</pre>

to get a sequential delta, and adding <code>-O1</code> speeds things up a bit.

## About

This is an exploration. My goal is to have a small, easy-to-maintain tool for teaching occam-style (process-and-channel-centric) concurrency on the Arduino (and other single-core, embedded platforms). 

* If I use the Transterpreter, I have to write new wrappers and libraries for every platform I encounter. This becomes tedious. And, it requires a big, aging compiler to generate the code (occ21). 

* If I use CCSP, I can't run on small devices.

And, in both cases, they support more language than I need (occam-pi). 

So, I started to try and simplify.

## Generating C

We once experimented with generating C from TCE bytecode, but this was sloppy, and involved some ugly hacks to the linker and VM. But, what if I just generated C directly? We avoided this in the past because it involved:

1. Lots of GOTO statements, and 
2. Putting everything in a single function.

(Well, that's not entirely true; there's tock, which generates C that interacts, through CIF, with CCSP, but that's too big/heavy for tiny embedded targets. I think.)

However, Arduino programs are typically small. So, what if I could take occam code, and generate C that implemented the scheduling correctly through cooperative jumps around a program? This seemed like a reasonable exploration, but I didn't want to write the compiler first.

## Testing By Hand

This exploration takes code like this:

<pre>
PROC producer (CHAN INT ch!)
  INITIAL INT counter IS 0:
  WHILE TRUE
    SEQ
      counter = counter + 1
      ch ! counter
:

PROC consumer (CHAN INT ch?)
  WHILE TRUE
    INT local:
    SEQ
      ch ? local
      -- Code to print things...
</pre>

and turns it into C that looks like this:

<pre>
  PROC (producer);
    int counter = 0;
    while (true) {
      counter = counter + 1;
      WRITE (PRODUCER_WRITE_CH, ch, counter);
    }        
  PROCEND (producer);


  PROC (consumer);
    while (true) { 
      int local;
      READ (CONSUMER_READ_CH, ch, local);
      printf ("C %d\n", local);   
    }    
  PROCEND (consumer);
</pre>

I belive, that through careful code construction, I can implement **PAR** and **ALT** without any real difficulty. (I've done **PAR**, and have **ALT** in my head.) Currently, I'm running producer/consumer and *commstime* (everyone's favorite non-benchmark) in reasonably clean code. I'm confident it could be generated automatically from occam source. **IF** should present no real difficulty, **STOP** and **SKIP** are simple enough, and that doesn't leave a lot of other things. 

I even think I have my deadlock detection correct. (If I go through all of my channels, and nothing happens, I declare deadlock. However, I need to add TIMERs into the mix, which will change things a bit.)

## The Question

Is this something that will fall over quickly as I try and use it for "real" programming on the Arduino (sensors and the like), or is this something that I can grow enough to be useful? 

It's currently a small exploration. Is it worth a bit more? Certainly, it's an easy generation target at the moment.
