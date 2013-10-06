all: prodcons commstime
	echo "Done"
	
prodcons:
	gcc -I. -o prodcons.exe tinycsp.c prodcons.c

commstime:
	gcc -I. -o commstime.exe list.c tinycsp.c commstime.c

prodcons2:
	gcc -I. -o pc2.exe list.c tinycsp.c prodcons2.c
