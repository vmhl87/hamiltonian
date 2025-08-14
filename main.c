#include <sys/ioctl.h>
#include <stdbool.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

void clear() { printf("\033c"); fflush(stdout); }

void move(int x, int y){
	printf("\033[%d;%dH", y, x);
	fflush(stdout);
}

void tbuf(int f){
	static struct termios old, new;
	static int init = 0;

	if(!init) init = 1,
		tcgetattr(STDIN_FILENO, &old),
		new = old, new.c_lflag &= (~ICANON & ~ECHO);

	tcsetattr(STDIN_FILENO, TCSANOW, f ? &new : &old);
	printf("\033[?25%c", f ? 'l' : 'h'), fflush(stdout);
}

int min(int a, int b){
	return a < b ? a : b;
}

int w = 1, h = 1, length;
void get_dimensions(){
	struct winsize W;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &W);
	h = 11, w = 17;
	h = min(W.ws_row-3, h);
	w = min(W.ws_col/2-2, w);
	length = w*h * 3/4;
}

int frame();
void generate();
void draw();

int solved = 1;

int main(){
	srand(time(NULL));

	get_dimensions();

	puts("generating..");

	generate();

	clear();
	tbuf(1);

	while(true) if(frame()) break;

	tbuf(0);

	printf("Completion: %d%%\n", solved*100/length);
}

int board[1024][1024];
int x, y;
void generate(){
	int now;

	do{
		for(int i=0; i<w; ++i)
			for(int j=0; j<h; ++j)
				board[i][j] = 0;

		x = 0, y = 0, now = 1, board[0][0] = 1;

		int dir[4][2], count = 0;

		while(now < length){
#define add_dir(dx, dy) if(x+dx >= 0 && x+dx < w && y+dy >= 0 && y+dy < h) \
			if(board[x+dx][y+dy] == 0) dir[count][0] = dx, dir[count][1] = dy, \
			++count;

			add_dir(-1, 0);
			add_dir(1, 0);
			add_dir(0, -1);
			add_dir(0, 1);

#undef add_dir

			if(count == 0) break;

			count = rand() % count;

			++now, x += dir[count][0], y += dir[count][1], board[x][y] = 1;

			count = 0;
		}

	}while(now < length);

	x = 0, y = 0;
}

void draw(){
	for(int j=0; j<h; ++j){
		move(3, j+2);

		for(int i=0; i<w; ++i){
			if(i == x && j == y)
				printf("\033[31m<>");
			else if(board[i][j] == 0)
				printf("\033[0m##");
			else if(board[i][j] == 1)
				printf("\033[1;30m[]");
			else if(board[i][j] == 2)
				printf("\033[0;35m[]");
			printf("\033[0m");
		}
	}

	move(1, h+2);

	fflush(stdout);
}

int frame(){
	draw();

	int count = 0;

#define dir(dx, dy) if(x+dx >= 0 && x+dx < w && y+dy >= 0 && y+dy < h) \
	if(board[x+dx][y+dy] == 1) ++count;

	dir(-1, 0);
	dir(1, 0);
	dir(0, -1);
	dir(0, 1);

#undef dir

	if(!count) return 1;

	char c = getchar();
	if(c == 'q') return 1;

#define dir(C, dx, dy) if(c == C) { \
	if(x+dx >= 0 && x+dx < w && y+dy >= 0 && y+dy < h) \
	if(board[x+dx][y+dy] == 1) \
		board[x][y] = 2, x += dx, y += dy, ++solved; }

	dir('h', -1, 0);
	dir('l', 1, 0);
	dir('k', 0, -1);
	dir('j', 0, 1);

	if(c == '\033'){
		c = getchar();
		c = getchar();

		dir('A', 0, -1);
		dir('B', 0, 1);
		dir('C', 1, 0);
		dir('D', -1, 0);
	}

#undef dir

	return 0;
}
