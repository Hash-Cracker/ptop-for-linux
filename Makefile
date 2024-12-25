CC=gcc
CFLAGS=-Wall -Wextra
LIBS=-lncurses
SRCDIR=src
PROGRAM=ptop

.PHONY: clean install

$(PROGRAM): $(SRCDIR)/ptop.c
	$(CC) $(CFLAGS) -o $(PROGRAM) $(SRCDIR)/ptop.c $(LIBS)

clean:
	rm -f $(PROGRAM)

install:
	install -m 755 $(PROGRAM) /usr/local/bin/
