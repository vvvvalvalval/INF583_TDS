CC      = gcc # Compilateur à utiliser
CFLAGS  = -Wall -Werror -pedantic -g -std=c99
					# Options le compilateur (man gcc)
LDFLAGS =
TARGETS = mon_cp

all: $(TARGETS)

mon_cp: mon_cp.c
	$(CC) mon_cp.c $(CFLAGS) $(LDFLAGS) -o mon_cp

clean:
	rm -f *~ a.out core
	rm -f *.o

distclean: clean
	rm -f $(TARGETS)

