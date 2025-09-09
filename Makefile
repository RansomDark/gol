CC=gcc
CFLAGS=-O -Wall

all: gol

gol: game_of_life.c game_of_life.h
	$(CC) $(CFLAGS) -o gol game_of_life.c -lncurses

clean:
	rm -f gol

distclean: clean
