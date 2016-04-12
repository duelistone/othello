CC          = g++
CFLAGS      = -Wall -O3 -msse2 -march=corei7 -ansi -pedantic -ggdb -std=c++14 -pthread
OBJS        = player.o board2.o 
PLAYERNAME  = duelist

all: $(PLAYERNAME) testgame
	
$(PLAYERNAME): $(OBJS) wrapper.o 
	$(CC) -o $@ math.o $^ -pthread

testgame: testgame.o 
	$(CC) -o $@ $^ -pthread

testminimax: $(OBJS) testminimax.o
	$(CC) -o $@ $^ -pthread

%.o: %.cpp
	$(CC) -c $(CFLAGS) -x c++ $< -o $@ -pthread
	
java:
	make -C java/

cleanjava:
	make -C java/ clean

clean:
	rm -f *.o $(PLAYERNAME) testgame testminimax
	
.PHONY: java testminimax
