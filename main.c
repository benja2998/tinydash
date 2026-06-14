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

static struct termios old_termios;
static int old_flags;

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
	int plr_row;
	int plr_col;
	int enm_row;
	int enm_col;
};

void clear(void) {
	printf("\033[1;1H\033[2J");
}

void map_init(struct map *m) {
	memset(m->pixels, '.', sizeof m->pixels);
	m->plr_row = ROWS / 2;
	m->plr_col = 0;

	m->enm_row = ROWS / 2;
	m->enm_col = COLS;
}

void map_render(struct map *m) {
	clear();
	fflush(stdout);
	for (int row = 0; row < ROWS; row++) {
		for (int col = 0; col < COLS; col++) {
			if (row == m->plr_row && col == m->plr_col) {
				write(STDOUT_FILENO, "@", 1);
			} else if (row == m->enm_row && col == m->enm_col) {
				write(STDOUT_FILENO, "$", 1);
			} else {
				write(STDOUT_FILENO, &m->pixels[row][col], 1);
			}
		}
		write(STDOUT_FILENO, "\n", 1);
	}
}

int main(void) {
	struct map my_map;
	map_init(&my_map);

	input_init();
	atexit(input_cleanup);

	int slowdown = false;

	while (1) {
		int key = get_input();

		if (key == ' ' && my_map.plr_row == ROWS / 2) {
			my_map.plr_row -= JUMPPOWER;
		}

		if (my_map.plr_row < ROWS / 2) {
			if (slowdown == false) {
				my_map.plr_row++;
				slowdown = true;
			} else {
				slowdown = false;
			}
		}
		
		if (my_map.enm_col > -1) {
			my_map.enm_col--;
		} else {
			my_map.enm_col = COLS;
		}
		if (my_map.enm_col == my_map.plr_col && my_map.enm_row == my_map.plr_row) {
			fflush(stdout);
			clear();
			fflush(stdout);
			printf("Game over, you died\n");
			break;
		}
		map_render(&my_map);
		usleep(40000);
	}
	return 0;
}
