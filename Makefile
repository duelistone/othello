CC          = g++
CFLAGS      = -Wall -O3 -msse2 -march=native -ansi -pedantic -std=c++14
OBJS        = player.o board.o 
PLAYERNAME  = duelist

all: $(PLAYERNAME) testgame
	
$(PLAYERNAME): $(OBJS) timer.o wrapper.o 
	$(CC) -o $@ $^ -pthread
	strip $(PLAYERNAME)

learn: board.o learn.o
	$(CC) $(CFLAGS) board.o learn.o -o learn
	rm board.o
	make board.o

testgame: testgame.o 
	$(CC) -o $@ $^ -pthread

learn.o: defs.h learn.cpp board.cpp board.h
	$(CC) -c $(CFLAGS) -funroll-loops board.cpp -D LEARNING
	$(CC) -c $(CFLAGS) learn.cpp -g

player.o: player.cpp common.h player.h board.h defs.h tt.h
	$(CC) -c $(CFLAGS) player.cpp

board.o: board.cpp common.h player.h board.h defs.h tt.h
	$(CC) -c $(CFLAGS) -funroll-loops board.cpp 

wrapper.o: wrapper.cpp common.h player.h board.h defs.h tt.h
	$(CC) -c $(CFLAGS) wrapper.cpp

timer.o: timer.cpp timer.h
	$(CC) -c $(CFLAGS) timer.cpp

testgame.o: testgame.cpp 
	$(CC) -c $(CFLAGS) testgame.cpp

java:
	make -C java/

cleanjava:
	make -C java/ clean

clean:
	rm -f *.o $(PLAYERNAME) testgame 
	
.PHONY: java
