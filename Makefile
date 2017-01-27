CC          = g++
CFLAGS      = -Wall -O3 -msse2 -march=native -ansi -pedantic -std=c++14
PLAYERNAME  = duelist

all: $(PLAYERNAME) testgame
	
$(PLAYERNAME): board.o player.o timer.o wrapper.o search.o
	$(CC) -o $@ $^ -pthread
	strip $(PLAYERNAME)

learn: board.o learn.o search.o
	$(CC) $(CFLAGS) board.o learn.o search.o -o learn
	rm board.o
	make board.o

database_solver: database_solver.o search.o board.o player.o
	$(CC) $(CFLAGS) database_solver.o search.o board.o player.o -o database_solver
	rm board.o
	make board.o

testgame: testgame.o 
	$(CC) -o $@ $^ -pthread

learn.o: defs.h learn.cpp board.cpp board.h common.h
	$(CC) -c $(CFLAGS) -funroll-loops board.cpp -D LEARNING
	$(CC) -c $(CFLAGS) learn.cpp -g

database_solver.o: defs.h database_solver.cpp board.h search.h common.h
	$(CC) -c $(CFLAGS) -funroll-loops board.cpp -D LEARNING
	$(CC) -c $(CFLAGS) database_solver.cpp

player.o: player.cpp common.h player.h board.h defs.h tt.h timer.h
	$(CC) -c $(CFLAGS) player.cpp

search.o: search.cpp common.h player.h board.h defs.h tt.h timer.h
	$(CC) -c $(CFLAGS) search.cpp

board.o: board.cpp common.h player.h board.h defs.h tt.h
	$(CC) -c $(CFLAGS) -funroll-loops board.cpp 

wrapper.o: wrapper.cpp common.h player.h board.h defs.h tt.h timer.h
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
