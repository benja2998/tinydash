#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <termios.h>
#include <fcntl.h>

#define ROWS 8
#define COLS 32
#define JUMPPOWER 4
#define FRAMETIME 40000

static struct termios old_termios;
static int old_flags;

void clear(void);

void die(void) {
	fflush(stdout);
	clear();
	fflush(stdout);
	printf("Game over, you died\n");
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

struct map {
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

void map_init(struct map *m, Player *plr, Enemy *enm, Enemy *enm2) {
	memset(m->pixels, '.', sizeof m->pixels);
	plr->row = ROWS / 2;
	plr->col = 0;
	enm->row = ROWS / 2;
	enm->col = COLS / 2;

	enm2->row = ROWS / 2;
	enm2->col = COLS;
}

void update_map(struct map *m, Player *plr, Enemy *enm, Enemy *enm2) {
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

void map_render(struct map *m) {
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
	struct map my_map;
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
		}

		if (key == 'q') {
			exit(0);
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
			die();
		}

		if (plr.row == enm2.row && plr.col == enm2.col) {
			die();
		}

		update_map(&my_map, &plr, &enm, &enm2);
		map_render(&my_map);
		usleep(FRAMETIME);
	}
	return 0;
}
