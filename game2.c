#include <stdio.h>
#include <curses.h>
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#define BUF_SIZE 1024

#define START_POINT 4
#define END_POINT 18
#define COL 10
#define ADD_MILLISECONDS 50
#define MAX 15
#define MAX_LIFE 100
#define MAX_WORD_COUNT 20
#define MAX_WORD_SIZE 30

typedef struct Rain
{
   int row;
   int col;
   char* word;
}Rain;

char* dataBase[MAX_WORD_COUNT] = {"culture","experience","education","symbol","effect","liberty","affair", "comfort","tradition","subject","object","source","pollution","system", "computer","competition","life","machine","art","corn"};

Rain queue[MAX];
int rear=0;
int front=0;
int pos = START_POINT;
int dir = +1;
int firstTime = 900;
int lifeScore = MAX_LIFE;
int demage = 4;
int flag = 0;
int cnt=0;

void RainDrop(int);
int set_ticker(int time);
void QueueAdd(Rain element);
Rain QueueDelete();
Rain CreateRain();
WINDOW *win0,*win1;
int main(void)
{
   int time = firstTime;

   initscr();
   //clear();
  
   win0=newwin(LINES-10,COLS-1,1,1);
   
   win1=newwin(5,COLS,LINES-5,0);
   box(win1,0,0);
   wrefresh(win1);
   wrefresh(win0);
   char buf[BUF_SIZE];
   int len;
   int fds[2];
   int i;
   int str_len=0;
   pipe(fds);
   pid_t pid = fork();
   //printf("%d",cnt++);	
   if(pid==-1){
	puts("forkerror!");
	exit(1);
	}
   if(pid==0){ //child
	//WINDOW* =win0=newwin(LINES-10,COLS-1,1,1);
	
   
   	signal(SIGALRM,RainDrop);
  	if(set_ticker(time)==-1)
      		perror("set_ticker");
   	else
      		while(1){
			
          		pause();
      		}
	return 0;
    }
    else{ //parent
	 
	while(1){
	mvwgetstr(win1, 3, 3, buf);
	//write(fds[1],buf,strlen(buf));
	for(i = 0 ; i < strlen(buf) ; i++)
           {
               buf[i] = 0x20;
          }
	mvwprintw(win1,3,3,"%s",buf);
	wrefresh(win1);
	//wrefresh(win0);
	curs_set(0);
  	}
	}
   endwin();
   return 0;
}

void RainDrop(int signum)
{
   int i;
   char blank[] = "              ";
   char message[MAX_WORD_SIZE];
   
   if(flag==0){
      flag=1;
      QueueAdd(CreateRain());
   }
   else
      flag=0;

   for(i=front; i<rear; i++)
   {
      strcpy(message,queue[i].word);
      
      //move(queue[i].row, queue[i].col);
      //addstr(message);
      mvwprintw(win0,queue[i].row,queue[i].col,message);
   }   
   //move(LINES-1,COLS-1);
   //refresh();
   wrefresh(win0);
   wrefresh(win1);
   for(i=front; i<rear; i++)
   {
      //move(queue[i].row, queue[i].col);
      //addstr(blank);
      mvwprintw(win0,queue[i].row,queue[i].col,blank);
      queue[i].row += dir;

      if(queue[i].row > END_POINT)
      {
         QueueDelete();
      }
   }

   if(lifeScore == 0)
      exit(1);
}

void QueueAdd(Rain element)
{
   queue[rear] = element;
   rear = ++rear % MAX;
}

Rain QueueDelete()
{
   Rain res;

   res = queue[front];
   front = ++front % MAX;
   return res;
}

Rain CreateRain()
{
   Rain* temp;
   temp = (Rain*)malloc(sizeof(Rain));
   
   srand(time(NULL));
   temp->word = dataBase[rand()%MAX_WORD_COUNT];
   temp->row = START_POINT;
   temp->col = (rand()%8)*13;

   return *temp;
}

int set_ticker( int n_msecs )
{
        struct itimerval new_timeset;
        long    n_sec, n_usecs;

        n_sec = n_msecs / 1000 ;      
        n_usecs = ( n_msecs % 1000 ) * 1000L ;   

        new_timeset.it_interval.tv_sec  = n_sec;        
        new_timeset.it_interval.tv_usec = n_usecs;      
        new_timeset.it_value.tv_sec     = n_sec  ;      
        new_timeset.it_value.tv_usec    = n_usecs ;     
   
   return setitimer(ITIMER_REAL, &new_timeset, NULL);
}
