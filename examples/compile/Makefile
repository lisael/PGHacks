LIBS = -lpghx -lpq
CPPFLAGS = -I/usr/include/postgresql
CC = gcc
SOURCE = pghx_compile.c
PROG = pghx_compile

all:
	$(CC) $(CPPFLAGS) $(LIBS) $(SOURCE) -o $(PROG)

clean:
	rm -f $(PROG)

.PHONY: all clean
