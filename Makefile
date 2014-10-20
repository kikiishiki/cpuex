CC = gcc
CFLAGS = -Wall -O2

all: sim

sim: env.o util.o io.o runsim.o main.o
	$(CC) $(CFLAGS) -o $@ $^

env.o:    env.h
util.o:   env.h util.h
io.o:     env.h util.h io.h
runsim.o: env.h util.h io.h runsim.h
main.o:   env.h util.h io.h runsim.h

clean:
	rm -f *.o sim

