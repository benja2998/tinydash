/*
   tinydash - a terminal-based game
   Copyright (C) 2026 benja2998

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.
   */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/stat.h>

#define ROWS 8
#define COLS 32
#define JUMPPOWER 4

int frametime = 40000;

static struct termios old_termios;
static int old_flags;
int score = 0;

void clear(void);

void my_exit(void) {
	fflush(stdout);
	clear();
	fflush(stdout);
	char c[1025];

	char filename[512];
	char *home = getenv("HOME");

	if (home == NULL) {
		exit(1);
	}

	snprintf(filename, sizeof(filename), "%s%s", home, "/tinydash_hs");

	FILE *fptr;
	if ((fptr = fopen(filename, "a+")) == NULL) {
		printf("Error");
		exit(1);
	}

	struct stat st;
	if (stat(filename, &st) != 0) {
		exit(1);
	}

	int filesize = (int)st.st_size;
	if (filesize == 0) {
		fprintf(fptr, "0");
	}

	fscanf(fptr, "%[^\n]", c);
	fclose(fptr);

	if ((fptr = fopen(filename, "w+")) == NULL) {
		printf("Error");
		exit(1);
	}

	int curhs = atoi(c);

	if (score > curhs) {
		snprintf(c, 512, "%d", score);
	}

	fprintf(fptr, "%s", c);

	printf("Score: %d, High Score: %s\n", score, c);

	exit(0);
}

void input_init(void) {
	struct termios new_termios;
	tcgetattr(STDIN_FILENO, &old_termios);
	new_termios = old_termios;
	new_termios.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);

	old_flags = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, old_flags | O_NONBLOCK);
}

void input_cleanup(void) {
	tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
	fcntl(STDIN_FILENO, F_SETFL, old_flags);
}

int get_input(void) {
	char c;

	if (read(STDIN_FILENO, &c, 1) == 1) {
		return c;
	}
	return -1;
}

struct FrameBuffer {
	char pixels[ROWS][COLS];
};

typedef struct {
	int row;
	int col;
} Enemy;

typedef struct {
	int row;
	int col;
} Player;

void clear(void) {
	printf("\033[1;1H\033[2J");
}

void map_init(struct FrameBuffer *m, Player *plr, Enemy *enm, Enemy *enm2) {
	memset(m->pixels, '.', sizeof m->pixels);
	plr->row = ROWS / 2;
	plr->col = 0;
	enm->row = ROWS / 2;
	enm->col = COLS / 2;

	enm2->row = ROWS / 2;
	enm2->col = COLS;
}

void update_map(struct FrameBuffer *m, Player *plr, Enemy *enm, Enemy *enm2) {
	memset(m->pixels, '.', sizeof m->pixels);

	if (plr->row >= 0 && plr->row < ROWS && plr->col >= 0 && plr->col < COLS) {
		m->pixels[plr->row][plr->col] = '@';
	}

	if (enm->row >= 0 && enm->row < ROWS && enm->col >= 0 && enm->col < COLS) {
		m->pixels[enm->row][enm->col] = 'E';
	}

	if (enm2->row >= 0 && enm2->row < ROWS && enm2->col >= 0 && enm2->col < COLS) {
		m->pixels[enm2->row][enm2->col] = 'E';
	}
}

void map_render(struct FrameBuffer *m) {
	clear();
	fflush(stdout);
	for (int row = 0; row < ROWS; row++) {
		for (int col = 0; col < COLS; col++) {
			write(STDOUT_FILENO, &m->pixels[row][col], 1);
		}
		write(STDOUT_FILENO, "\n", 1);
	}
}

int main(void) {
	struct FrameBuffer my_map;
	Player plr = {0};
	Enemy enm = {0}, enm2 = {0};

	map_init(&my_map, &plr, &enm, &enm2);

	input_init();
	atexit(input_cleanup);

	int slowdown = false;

	while (1) {
		int key = get_input();

		if (key == ' ' && plr.row == ROWS / 2) {
			plr.row -= JUMPPOWER;
		} else if (key == ' ') {
			plr.row = ROWS / 2;
		}

		if (key == 'q') {
			my_exit();
		}

		if (plr.row < ROWS / 2) {
			if (slowdown == false) {
				plr.row++;
				slowdown = true;
			} else {
				slowdown = false;
			}
		}

		if (enm.col > -1) {
			enm.col--;
		} else {
			enm.col = COLS;
		}
		if (enm2.col > -1) {
			enm2.col--;
		} else {
			enm2.col = COLS + COLS / 2;
		}

		if (plr.row == enm.row && plr.col == enm.col){
			my_exit();
		}

		if (plr.row == enm2.row && plr.col == enm2.col) {
			my_exit();
		}

		if (plr.col == enm.col && plr.row != enm.row){
			score++;
			frametime = frametime - 10;				  
		}

		if (plr.col == enm2.col && plr.row != enm2.row) {
			score++;
			frametime = frametime - 10;
		}

		update_map(&my_map, &plr, &enm, &enm2);
		map_render(&my_map);
		usleep(frametime);
	}
	return 0;
}
