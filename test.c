#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 1024
int cnt=0;
int main() {
	//char buf[100];
	int i;
 	int fds[2]; //pipe
	char buf[BUF_SIZE];
	int str_len=0;
	initscr();
	//  noecho();
	curs_set(0);
	WINDOW *win0 = newwin(LINES-10, COLS-1, 1, 1);
	//box(win0,0,0);	
	WINDOW *win1 = newwin(5,COLS,LINES-10+1, 0);
	box(win1,0,0);
	pid_t pid = fork();
	pipe(fds);
	

  if (pid == -1)
    exit(1);
  if (pid == 0) { //child
    int n = 0;
    while (1) {
     	

     	//mvwgetstr(win1, 2, 1, buf);
	//mvwprintw(win1,1,1,"%s",buf);
	//write(fds[1],buf,strlen(buf));	
	//cnt=strlen(buf);
	//if(buf[cnt]=='\n')
	//write(fds[1],buf,strlen(buf));	
	/*for(i = 0 ; i < strlen(buf) ; i++)
           {
               buf[i] = 0x20;
          }*/

        //wrefresh(win1);
	
    }
  }
  else {
    int n = 0;
    while (1) { //parent
	mvwgetstr(win1, 2, 1, buf);
	//mvwprintw(win1,1,1,"%s",buf);
	write(fds[1],buf,strlen(buf));	
	
	//cnt=strlen(buf);
	//if(buf[cnt]=='\n')
	//write(fds[1],buf,strlen(buf));	
	for(i = 0 ; i < strlen(buf) ; i++)
           {
               buf[i] = 0x20;
          }
	mvwprintw(win1,2,1,"%ssd",buf);
	wrefresh(win1);
	if((str_len=read(fds[0],buf,BUF_SIZE))>0)
		wprintw(win0,"%s\n",buf);
	wrefresh(win0);
	
    }
  }
  endwin();

  return 0;
}
