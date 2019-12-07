#include <stdio.h>
#include <ncurses.h>
#define BUF_SIZE 1024
int main(){
	char buf[BUF_SIZE];
	initscr();
	WINDOW *win =newwin(LINES-10 ,COLS-1,1,1);
	box(win,0,0);
	
	wrefresh(win);
	mvwgetstr(win,2,2,buf);
	
	
	if(!strcmp("q",buf))
	{
		endwin();
		return 0;
	}
	
	//endwin();

}
