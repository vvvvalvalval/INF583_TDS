CC      = gcc
CFLAGS  = -Wall -pedantic -g -std=c99
BIN1    = my_pipe
BIN2    = my_dup
BIN3    = my_popen
TARGETS = $(BIN1) $(BIN2) $(BIN3)

all: $(TARGETS)

clean:
	rm -f *~ a.out core
	rm -f *.o

distclean: clean
	rm -f $(TARGETS) *.a *.so

$(BIN1): $(BIN1).c
	$(CC) $(BIN1).c $(CFLAGS) -o $(BIN1)

$(BIN2): $(BIN2).c
	$(CC) $(BIN2).c $(CFLAGS) -o $(BIN2)

$(BIN3): $(BIN3).c
	$(CC) $(BIN3).c $(CFLAGS) -o $(BIN3)

