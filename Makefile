all: prodcons3 commstime verytinycsp
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
	gcc $(CFLAGS) --combine -fwhole-program -I. -o prodcons3.exe list.c tinycsp.c prodcons3.c

verytinycsp:
	clang++ -std=c++11 -stdlib=libc++ -lc++abi verytinycsp.cpp -o verytiny.exe
