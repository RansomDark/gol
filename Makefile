CC=gcc
CFLAGES=-O -Wall

all: gol

gol: game_of_life.c game_of_life.h
		$(CC) $(CFLAGES) -o gol game_of_life.c -lncurses

clean:
		rm -f gol

distclean: clean
