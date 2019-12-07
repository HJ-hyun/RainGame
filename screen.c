#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>

int main()
{
    int row;
    int col;
    
    initscr();
    clear();
    row=LINES-2;
    col=COLS-2;
    move(row/2,col/2-5);
    start_color();
    init_pair(1,4,0);
    attron(A_BOLD|COLOR_PAIR(1));
    addstr("*RAIN DROP*");
    move(row/2+1,col/2-2);
    addstr("GAME");
    move(row/2+5,col/2-11);
    attroff(COLOR_PAIR(1));
    addstr("Enter your name:");
    refresh();
    char name[10];
    getstr(name);
    
    sleep(1);
    endwin();
}
