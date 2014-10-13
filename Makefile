CC = gcc
CFLAGS = -Wall -O
EXEC = exec_alu.o exec_fpu.o exec_mem.o exec_IO.o exec_jump.o

all: test

test: testmain.o runsim.o $(EXEC) mem.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f *.o test
