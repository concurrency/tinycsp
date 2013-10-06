all: prodcons commstime
	echo "Done"
	
prodcons:
	gcc -I. -o prodcons.exe prodcons.c

commstime:
	gcc -I. -o commstime.exe commstime.c

schedule:
	gcc -I. -o schedule.exe schedule.c