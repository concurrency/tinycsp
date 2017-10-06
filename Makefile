all: pc_c pc_cpp commstime_c
# Almost no difference between SEQ/PAR delta
# with -O1. Kinda makes sense, in this context.
CFLAGS=-O1

clean:
	rm -f *.o
	rm -f *.exe

commstime_c:
	gcc $(CFLAGS) -flto -fwhole-program -I. -o commstime_c-par.exe list.c tinycsp.c commstime.c
	gcc $(CFLAGS) -DUSESEQDELTA -flto -fwhole-program -I. -o commstime_c-seq.exe list.c tinycsp.c commstime.c
	
pc_c:
	gcc $(CFLAGS) -flto -fwhole-program -I. -o pc_c.exe list.c tinycsp.c pc.c
	gcc $(CFLAGS) -flto -fwhole-program -I. -o bouncer.exe list.c tinycsp.c bouncer.c

#clang++ -std=c++11 -stdlib=libc++ -lc++abi verytinycsp.cpp -o verytiny.exe
pc_cpp:
	g++ -o pc_cpp.exe pc.cpp
