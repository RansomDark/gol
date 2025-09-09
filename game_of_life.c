#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "game_of_life.h"

/*
 * input file format:
 * <r1> <c1>
 * <RLE1>
 *
 * or
 * <RLE1>
 *
 * (in this case <RLE1> must start from a non-numeric character
 * which is often the case, i.e. "#" or "x = ")
 */

int main() {
    int map[MAP_HEIGHT][MAP_WIDTH] = {0};
		
		int inputError = 0;
		FILE *preset_file = NULL;
		char full_path[256];

		snprintf(full_path, sizeof(full_path), "%s%s", PRESET_DIR, PRESET_PATH);
		preset_file = fopen(full_path, "r");

		if (!preset_file) {
				preset_file = fopen(PRESET_PATH, "r");
		}

		if (!preset_file) {
				printf("Error: Cannot open preset file\n");
				printf("Tried: %s\n", full_path);
				printf("Tried: %s\n", PRESET_PATH);
				inputError = 1;
		}

		if (!inputAllShapes(map, preset_file)) inputError = 1;

		int freopenError = 0;
		if (!inputError && !freopen("/dev/tty", "r", stdin)) {
				freopenError = 1;
		}

		if (!inputError && !freopenError) {
				game(map);
		} else {
				panic();
		}

		fclose(preset_file);
		return 0;
}

int getCountNeighbours(int map[][MAP_WIDTH], int y, int x) {
    int count = 0;
    int x1 = (x + 1) % MAP_WIDTH, x2 = (x - 1 + MAP_WIDTH) % MAP_WIDTH;
    int y1 = (y + 1) % MAP_HEIGHT, y2 = (y - 1 + MAP_HEIGHT) % MAP_HEIGHT;

    if (map[y1][x] > 0) count++;
    if (map[y2][x] > 0) count++;
    if (map[y][x1] > 0) count++;
    if (map[y][x2] > 0) count++;
    if (map[y1][x1] > 0) count++;
    if (map[y1][x2] > 0) count++;
    if (map[y2][x1] > 0) count++;
    if (map[y2][x2] > 0) count++;
    return count;
}

void update(int map[][MAP_WIDTH]) {
    int tmp[MAP_HEIGHT][MAP_WIDTH];
    int neighbours;
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            neighbours = getCountNeighbours(map, i, j);
            if (map[i][j] == 0 && neighbours == 3)
                tmp[i][j] = 1;
            else if (map[i][j] > 0 && (neighbours == 3 || neighbours == 2)) {
                tmp[i][j] = (map[i][j] != MAX_GENERATION) ? map[i][j] + 1 : map[i][j];
            } else
                tmp[i][j] = 0;
        }
    }
    for (int i = 0; i < MAP_HEIGHT; i++)
        for (int j = 0; j < MAP_WIDTH; j++) map[i][j] = tmp[i][j];
}

void printMap(int map[][MAP_WIDTH], WINDOW *mainwin, int delay) {
    wclear(mainwin);
    box(mainwin, 0, 0);

    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            printSquare(map, mainwin, i, j);
        }
    }

    update(map);
    wrefresh(mainwin);
}

void printSquare(int map[][MAP_WIDTH], WINDOW *mainwin, int i, int j) {
    if (map[i][j] > 0) {
        wattron(mainwin, COLOR_PAIR(map[i][j]));
        mvwaddch(mainwin, i + 1, j + 1, '#');
        wattroff(mainwin, COLOR_PAIR(map[i][j]));
    } else
        mvwaddch(mainwin, i + 1, j + 1, '.');
}
void game(int map[][MAP_WIDTH]) {
    int delay = INITIAL_DELAY;

		initscr();
    start_color();
		init_color(COLOR_ORANGE, 1000, 600, 0);

    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_ORANGE, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);
    curs_set(0);
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    WINDOW *mainwin = newwin(Y_LENGTH, X_LENGTH, 0, 0);

    int finish = 0;
    while (!finish) {
        printMap(map, mainwin, delay);
        timeout(delay);
				int input = getch();
				switch(input) {
						case ' ':  	
								delwin(mainwin);
								endwin();
								finish = 1;
								break;
        }
        // if (!finish) napms(delay);
    }
}

int skipTillNewline(FILE* preset_file) {
    int ch = fgetc(preset_file);
    while (ch != '\n' && ch != EOF) {
        ch = fgetc(preset_file);
    }
    return ch == '\n';
}

int inputOneShape(int ***dynamicMatrix, int *m, int *n, FILE* preset_file) {
    int errorInput = !inputDimensions(m, n, preset_file);

    int errorAllocation = 0;
    if (!errorInput) {
        if (!allocateModePointersToArrays(dynamicMatrix, *m, *n)) errorAllocation = 1;
    }

    int errorRle = 0;
    if (!errorInput && !errorAllocation) {
        zeroMatrix(*dynamicMatrix, *m, *n);
        errorRle = !inputRle(*dynamicMatrix, *m, *n, preset_file);
    }

    if (!errorInput && errorRle && !errorAllocation) deallocateModePointersToArrays(*dynamicMatrix, *m);

    return !errorInput && !errorAllocation && !errorRle;
}

int inputDimensions(int *m, int *n, FILE* preset_file) {
    int errorInput = 0;
    int ch = fgetc(preset_file);
    while (!errorInput && ch != 'x') {
        if (ch != '#') {
            errorInput = 1;
        }
        if (!errorInput && !skipTillNewline(preset_file)) {
            errorInput = 1;
        }
        if (!errorInput) ch = fgetc(preset_file);
    }
    if (!errorInput && ch != 'x') errorInput = 1;
    if (!errorInput && fgetc(preset_file) != ' ') errorInput = 1;
    if (!errorInput && fgetc(preset_file) != '=') errorInput = 1;
    if (!errorInput && fgetc(preset_file) != ' ') errorInput = 1;
    if (!errorInput && fscanf(preset_file, "%d", n) != 1) errorInput = 1;
    if (!errorInput && *n <= 0) errorInput = 1;
    if (!errorInput && *n > 80) errorInput = 1;
    if (!errorInput && fgetc(preset_file) != ',') errorInput = 1;
    if (!errorInput && fgetc(preset_file) != ' ') errorInput = 1;
    if (!errorInput && fgetc(preset_file) != 'y') errorInput = 1;
    if (!errorInput && fgetc(preset_file) != ' ') errorInput = 1;
    if (!errorInput && fgetc(preset_file) != '=') errorInput = 1;
    if (!errorInput && fgetc(preset_file) != ' ') errorInput = 1;
    if (!errorInput && (fscanf(preset_file, "%d", m) != 1)) errorInput = 1;
    if (!errorInput && *m <= 0) errorInput = 1;
    if (!errorInput && *m > 25) errorInput = 1;
    if (!errorInput && !skipTillNewline(preset_file)) errorInput = 1;

    return !errorInput;
}

int allocateModePointersToArrays(int ***dynamicMatrix, int m, int n) {
    int error = 0;
    *dynamicMatrix = malloc(m * sizeof(int *));
    if (!*dynamicMatrix) {
        error = 1;
    }

    int i;
    for (i = 0; !error && i != m; ++i) {
        (*dynamicMatrix)[i] = malloc(n * sizeof(int));
        if (!((*dynamicMatrix)[i])) {
            error = 1;
        }
    }

    if (error) {
        for (int freed = 0; freed != i; ++i) {
            free((*dynamicMatrix)[freed]);
        }
        free(*dynamicMatrix);
    }
    return !error;
}

void deallocateModePointersToArrays(int **dynamicMatrix, int m) {
    for (int i = 0; i != m; ++i) free(dynamicMatrix[i]);
    free(dynamicMatrix);
}

void panic() { printf("n/a"); }

void zeroMatrix(int **a, int m, int n) {
    for (int i = 0; i != m; ++i)
        for (int j = 0; j != n; ++j) a[i][j] = 0;
}

void pasteMatrix(int **src, int rSrc, int cSrc, int h, int w, int map[][MAP_WIDTH], int rDest, int cDest) {
    for (int i = 0; i != h; ++i)
        for (int j = 0; j != w; ++j) map[rDest + i][cDest + j] = src[rSrc + i][cSrc + j];
}

int inputRle(int **grid, int m, int n, FILE* preset_file) {
    int inputError = 0;
    int ind = 0;
    int finish = 0;
    while (!inputError && !finish) {
        int count;
        if (fscanf(preset_file, "%d", &count) != 1) {
            count = 1;
        }

        int c = fgetc(preset_file);
        if (c == 'b' || c == 'o') {
            for (int k = 0; ind != m * n && k < count; k++) {
                grid[ind / n][ind % n] = c == 'b' ? 0 : 1;
                ++ind;
            }
        } else if (c == '$') {
            int pad = (n - (ind % n)) % n;
            for (int k = 0; k != pad; k++) {
                grid[ind / n][ind % n] = 0;
                ++ind;
            }
        } else if (c == '!') {
            while (ind != m * n) {
                grid[ind / n][ind % n] = 0;
                ++ind;
            }
            finish = 1;
        } else if (c != '\n' && c != ' ') {
            inputError = 1;
        }
    }

    return !inputError;
}

int pasteOneShape(int map[][MAP_WIDTH], FILE* preset_file) {
    int **mat1;
    int m, n;

		int c, r;
		if (fscanf(preset_file, "%d%d", &r, &c) != 2) {
				r = c = 0;
		} else {
				skipTillNewline(preset_file);
		}

    int errorMat1 = 0;
    if (!inputOneShape(&mat1, &m, &n, preset_file)) errorMat1 = 1;

    int errorOutOfBounds = 0;
    if (!errorMat1) {
        if (r < 0 || c < 0) {
            r = c = 0;
        }
        if (r + m > MAP_HEIGHT || c + n > MAP_WIDTH) {
            errorOutOfBounds = 1;
        }
    }

    if (!errorMat1 && !errorOutOfBounds) {
        pasteMatrix(mat1, 0, 0, m, n, map, r, c);
    }

    if (!errorMat1) {
        deallocateModePointersToArrays(mat1, m);
    }

    return !errorMat1 && !errorOutOfBounds;
}
int inputAllShapes(int map[][MAP_WIDTH], FILE* preset_file) {
    int shapes = 0;
    while (pasteOneShape(map, preset_file)) {
        ++shapes;
    }
    return shapes != 0;
}
