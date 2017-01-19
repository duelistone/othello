CC          = g++
CFLAGS      = -Wall -O3 -msse2 -march=native -ansi -pedantic -std=c++14 -pthread
OBJS        = player.o board.o 
PLAYERNAME  = duelist

all: $(PLAYERNAME) testgame
	
$(PLAYERNAME): $(OBJS) timer.o wrapper.o 
	$(CC) -o $@ $^ -pthread

learn: board.o learn.o
	$(CC) $(CFLAGS) board.o learn.o -o learn

testgame: testgame.o 
	$(CC) -o $@ $^ -pthread

learn.o: defs.h learn.cpp board.h
	$(CC) -c $(CFLAGS) learn.cpp

player.o: player.cpp common.h player.h board.h defs.h
	$(CC) -c $(CFLAGS) player.cpp -pthread

board.o: board.cpp common.h player.h board.h defs.h
	$(CC) -c $(CFLAGS) board.cpp -pthread

wrapper.o: wrapper.cpp common.h player.h board.h defs.h
	$(CC) -c $(CFLAGS) wrapper.cpp -pthread

timer.o: timer.cpp timer.h
	$(CC) -c $(CFLAGS) timer.cpp

testgame.o: testgame.cpp 
	$(CC) -c $(CFLAGS) testgame.cpp -pthread

java:
	make -C java/

cleanjava:
	make -C java/ clean

clean:
	rm -f *.o $(PLAYERNAME) testgame 
	
.PHONY: java
