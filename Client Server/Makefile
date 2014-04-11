PROGS = server client

all:$(PROGS) 

server:server.c
	gcc -o $@ server.c  -pthread

client:client.c	
	gcc -o $@ client.c
	
	
clean:
	rm -f *.0 *~ *.so *.a *.o $(PROGS)

