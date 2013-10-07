all: prodcons commstime
	echo "Done"
	
prodcons:
	gcc $(CFLAGS) -I. -o prodcons.exe tinycsp.c prodcons.c

commstime:
	gcc $(CFLAGS) -I. -o commstime.exe list.c tinycsp.c commstime.c

prodcons2:
	gcc $(CFLAGS) -I. -o pc2.exe list.c tinycsp.c prodcons2.c
