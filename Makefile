CC          = g++
CFLAGS      = -Wall -O3 -msse2 -march=corei7 -ansi -pedantic -ggdb -std=c++14 -pthread
OBJS        = player.o board2.o 
PLAYERNAME  = duelist

all: $(PLAYERNAME) testgame
	
$(PLAYERNAME): $(OBJS) timer.o math.o wrapper.o 
	$(CC) -o $@ $^ -lboost_system -pthread

testgame: testgame.o 
	$(CC) -o $@ $^ -pthread

testminimax: $(OBJS) testminimax.o
	$(CC) -o $@ $^ -pthread

#%.o: %.cpp
#	$(CC) -c $(CFLAGS) -x c++ $< -o $@ -pthread

player.o: player.cpp board2.cpp common.h player.h board2.h
	$(CC) -c $(CFLAGS) player.cpp -pthread

board2.o: board2.cpp common.h player.h board2.h
	$(CC) -c $(CFLAGS) board2.cpp -pthread

wrapper.o: wrapper.cpp player.h
	$(CC) -c $(CFLAGS) wrapper.cpp -pthread

testgame.o: testgame.cpp
	$(CC) -c $(CFLAGS) testgame.cpp -pthread

timer.o: timer.cpp timer.h
	$(CC) -c $(CFLAGS) timer.cpp

math.o: math.s
	as -o math.o math.s
	
java:
	make -C java/

cleanjava:
	make -C java/ clean

clean:
	rm -f *.o $(PLAYERNAME) testgame testminimax
	
.PHONY: java testminimax
