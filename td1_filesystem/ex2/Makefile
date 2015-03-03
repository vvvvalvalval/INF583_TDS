CC      = gcc
CFLAGS  = -Wall -Werror -pedantic -g -std=c99
AR      = ar
TARGETS = mon_cp_static mon_cp_dynamic

all: $(TARGETS)

clean:
	rm -f *~ a.out core
	rm -f *.o

distclean: clean
	rm -f $(TARGETS) *.a *.so

mon_cp_static: mon_cp.c libfilemanipsta.a
	$(CC) mon_cp.c $(CFLAGS) -L. -lfilemanipsta -o mon_cp_static

mon_cp_dynamic: mon_cp.c libfilemanipdyn.so
	$(CC) mon_cp.c $(CFLAGS) -L. -lfilemanipdyn -Xlinker -rpath=. -o mon_cp_dynamic

libfilemanipsta.a: file_manip.h file_manip.c
	$(CC) $(CFLAGS) -c file_manip.c
	$(AR) cr libfilemanipsta.a file_manip.o

libfilemanipdyn.so: file_manip.h file_manip.c
	$(CC) file_manip.c $(CFLAGS) -fPIC -shared -o libfilemanipdyn.so

