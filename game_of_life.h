#ifndef GAME_OF_LIVE_H
#define GAME_OF_LIVE_H

#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>

#define X_LENGTH 82
#define Y_LENGTH 27
#define DELAY_STEP 15
#define DELAY_MIN 40
#define DELAY_MAX 1500
#define MAP_WIDTH 80
#define MAP_HEIGHT 25
#define MAX_GENERATION 4
#define INITIAL_DELAY 500

#define PRESET_PATH "presets/preset3-gosper-gun-eater.txt"

#define COLOR_ORANGE 8


int skipTillNewline(FILE* preset_file);
int inputAllShapes(int map[][MAP_WIDTH], FILE* preset_file);
int inputOneShape(int ***dynamicMatrix, int *m, int *n, FILE* preset_file);
int inputDimensions(int *m, int *n, FILE* preset_file);
int pasteOneShape(int map[][MAP_WIDTH], FILE* preset_file);
void panic();

int allocateModePointersToArrays(int ***dynamicMatrix, int m, int n);
void deallocateModePointersToArrays(int **dynamicMatrix, int m);

void pasteMatrix(int **src, int rSrc, int cSrc, int h, int w, int map[][MAP_WIDTH], int rDest, int cDest);
void zeroMatrix(int **a, int m, int n);
int inputRle(int **grid, int m, int n, FILE* preset_file);

int getCountNeighbours(int map[][MAP_WIDTH], int y, int x);
void update(int map[][MAP_WIDTH]);
void game(int map[][MAP_WIDTH]);
void printMap(int map[][MAP_WIDTH], WINDOW *, int delay);
void printSquare(int map[][MAP_WIDTH], WINDOW *mainwin, int i, int j);

#endif
