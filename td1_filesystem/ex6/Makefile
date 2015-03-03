CC      = gcc
CFLAGS  = -Wall -Werror -pedantic -g -std=c99
LDFLAGS =
TARGETS = lecteur

all: $(TARGETS)

mon_cp: lecteur.c
	$(CC) lecteur.c $(CFLAGS) $(LDFLAGS) -o lecteur

clean:
	rm -f *~ a.out core
	rm -f *.o

distclean: clean
	rm -f $(TARGETS)

