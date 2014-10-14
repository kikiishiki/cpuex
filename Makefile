CC = gcc
CFLAGS = -Wall -O
EXEC = exec_alu.o exec_fpu.o exec_mem.o exec_IO.o exec_jump.o

all: sim make_input

test: testmain.o runsim.o $(EXEC) mem.o
	$(CC) $(CFLAGS) -o $@ $^

sim: main.o runsim.o $(EXEC) mem.o flags.h
	$(CC) $(CFLAGS) -o $@ $^

make_input: make_input_file.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f *.o test sim make_input
