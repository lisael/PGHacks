LIBS = -lpghx -lpq
CPPFLAGS = -I/usr/include/postgresql
CC = gcc
SOURCE = compile_pghx.c
PROG = compile_pghx

all:
	$(CC) $(CPPFLAGS) $(LIBS) $(SOURCE) -o $(PROG)

clean:
	rm -f $(PROG)

.PHONY: all clean
