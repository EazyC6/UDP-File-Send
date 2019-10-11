
myclient:	myclient.o
		gcc -o myclient myclient.c

myserver:	myserver.o
		gcc -o myserver myserver.c

clean:
		rm -f myserver myclient myclient.o myserver.o