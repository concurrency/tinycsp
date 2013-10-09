all: prodcons3 commstime
# Almost no difference between SEQ/PAR delta
# with -O1. Kinda makes sense, in this context.
CFLAGS=-O1

clean:
	rm -f *.o
	rm -f *.exe

commstime:
	gcc $(CFLAGS) --combine -fwhole-program -I. -o commstime-par.exe list.c tinycsp.c commstime.c
	gcc $(CFLAGS) -DUSESEQDELTA --combine -fwhole-program -I. -o commstime-seq.exe list.c tinycsp.c commstime.c

prodcons3:
	gcc $(CFLAGS) --combine -fwhole-program -I. -o pc3.exe list.c tinycsp.c prodcons3.c
