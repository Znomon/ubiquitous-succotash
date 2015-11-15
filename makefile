CC=gcc
CFLAGS=-I.
#DEPS = hellomake.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

FinalNetworksProject: FinalClient.o FinalServer.o
	gcc -o client FinalClient.o -lnsl -I.
	gcc -o server FinalServer.o -lnsl -I.
	
run: 
	#./theCube -size 2 -seed 2 -teamA 2 -teamB 2
	
clean:
	rm FinalClient.o
	rm FinalServer.o
	rm client
	rm server