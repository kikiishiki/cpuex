CC = gcc
CFLAGS = -Wall -O2

all: ysim

ysim: cnt.o env.o util.o io.o fmul.o runsim.o main.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

cnt.o:    cnt.h
env.o:    env.h
util.o:   env.h util.h
io.o:     env.h util.h io.h
runsim.o: env.h util.h io.h runsim.h
main.o:   env.h util.h io.h runsim.h

clean:
	rm -f *.o ysim

