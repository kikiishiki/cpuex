CC = gcc
CFLAGS = -Wall -O2

all: gsim

gsim: cnt.o env.o util.o ldst.o io.o fmul.o runsim.o main.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

cnt.o:    cnt.h
env.o:    env.h
util.o:   env.h util.h
ldst.o:	  env.h ldst.h
io.o:     env.h util.h io.h
runsim.o: env.h util.h io.h runsim.h ldst.h 
main.o:   env.h util.h io.h runsim.h

clean:
	rm -f *.o gsim

