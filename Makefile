CC=gcc
CFLAGES=-O -Wall

all: gol

gol: game_of_life.c game_of_life.h
		$(CC) $(CFLAGES) -o /usr/bin/gol game_of_life.c -lncurses

install:
		sudo mkdir -p /usr/bin /usr/share/gol/presets
		sudo cp -r presets/* /usr/share/gol/presets
		sudo chmod 755 /usr/bin/gol
		sudo chmod -R 644 /usr/share/gol/presets/*

uninstall:
		sudo rm -f /usr/bin/gol
		sudo rm -rf /usr/share/gol

rebuild: clean all

clean:
		rm -f gol

.PHONY: all install uninstall clean
