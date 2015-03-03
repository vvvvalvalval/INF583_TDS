CC      = gcc
CFLAGS  = -Wall -Wextra -pedantic -g -std=c99
TARGETS = libmemory.so test

all: $(TARGETS)

clean:
	rm -f *~ a.out core
	rm -f *.o

distclean: clean
	rm -f $(TARGETS) *.a *.so

test: test.c libmemory.so
	$(CC) test.c $(CFLAGS) -L. -lmemory -Xlinker -rpath=. -o test

libmemory.so: memory.h memory.c
	$(CC) memory.c $(CFLAGS) -fPIC -shared -o libmemory.so

