CC      = gcc
CFLAGS  = -Wall -pedantic -g -std=c99
BIN     = my_pipe
TARGETS = $(BIN)

all: $(TARGETS)

clean:
	rm -f *~ a.out core
	rm -f *.o

distclean: clean
	rm -f $(TARGETS) *.a *.so

$(BIN): $(BIN).c
	$(CC) $(BIN).c $(CFLAGS) -o $(BIN)

