#include <stdio.h>
#include <curses.h>
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>

#define BUF_SIZE 1024

#define START_POINT 2
#define END_POINT 14
#define COL 10
#define SUB_MILLISECONDS 10
#define MAX 500
#define MAX_LIFE 100
#define MAX_WORD_COUNT 100
#define MAX_WORD_SIZE 30

typedef struct Rain
{
   int row;
   int col;
   char word[MAX_WORD_SIZE];
}Rain;



Rain queue[MAX];
int rear=0;
int front=0;
int pos = START_POINT;
int dir = +1;
int runTime = 800;
int lifeScore = MAX_LIFE;
int score = 0;
int damage = 10;
int flag = 0;
int cnt=0;
int arrayCount=0;
char input[MAX_WORD_SIZE];
char wordData[MAX_WORD_COUNT][MAX_WORD_SIZE];
char collectBuf[MAX_WORD_SIZE];
WINDOW *win0,*win1;

void RainDrop(int);
int set_ticker(int time);
void QueueAdd(Rain element);
Rain QueueDelete();
Rain CreateRain();
void MakeCollectBuf();
void StartGame();
void StopGame();

void *InputThread();
void *GameThread();

void main()
{
	char buff[MAX_WORD_SIZE];
	FILE *fp;
	int i=0;
	
	fp=fopen("test.txt","r");

	while(!feof(fp))
	{
		fscanf(fp,"%s",buff);
	  	strcpy(wordData[arrayCount++],buff);
	}
	fclose(fp);

	StartGame();
}

void StartGame()
{
	MakeCollectBuf();
   	initscr();
   	clear();
  
   	win0=newwin(LINES-5,COLS-1,1,1);
   
   	win1=newwin(4,COLS,LINES-4,0);
   	box(win1,0,0);
   	wrefresh(win1);
   	wrefresh(win0);

   	pthread_t t1,t2;

   	pthread_create(&t1,NULL,GameThread,NULL);
   	pthread_create(&t2,NULL,InputThread,NULL);
   
   	pthread_join(t1,NULL);
   	pthread_join(t2,NULL);
   	
	endwin();
   	return ;
}

void MakeCollectBuf()
{
	int i;

	for(i=0; i<MAX_WORD_SIZE; i++)
	{
		collectBuf[i] = 0x20;
	}
}

void RainDrop(int signum)
{
   	int i;
   	char blank[] = "              ";
   
   	if(flag==0){
      		flag=1;
      		QueueAdd(CreateRain());
   	}
   	else
      		flag=0;

   	for(i=front; i<rear; i++)
		mvwprintw(win0,queue[i].row,queue[i].col,queue[i].word);
   	
		mvwprintw(win0,15,0,"------------------------------------------------------------------------------");
   	mvwprintw(win0,16,60,"score: %d",score);
	mvwprintw(win0,17,60,"life score: %d ",lifeScore);
	
	wrefresh(win0);
   	wrefresh(win1);
   	for(i=front; i<rear; i++)
   	{
      		mvwprintw(win0,queue[i].row,queue[i].col,blank);
      		queue[i].row += dir;

      		if(queue[i].row > END_POINT)
      		{
			if(strcmp(queue[i].word,collectBuf)!=0)
         		{
				lifeScore -= damage;
			}
			QueueDelete();
      		}
   	}

   	if(lifeScore == 0)  //die
      	{
		exit(1);
	}
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
	int fd;
   	Rain* temp;
   	temp = (Rain*)malloc(sizeof(Rain));
   
   	srand(time(NULL));
   	strcpy(temp->word,wordData[rand()%arrayCount]);
   	temp->row = START_POINT;
   	temp->col = (rand()%8)*8;

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

void *GameThread()
{
   	signal(SIGALRM,RainDrop);
  	if(set_ticker(runTime)==-1)
      		perror("set_ticker");
   	else
      		while(1){	
          		pause();
      		}
}

void *InputThread()
{
	int i;
	char buf[MAX_WORD_SIZE];
	char answer[]="INPUT: ";
	char res[MAX_WORD_SIZE];

	while(1){
		mvwprintw(win1,2,1,"%s",answer);
		mvwgetstr(win1, 2, 8, input);

		for(i=front; i<rear; i++)
		{
			if(strcmp(input,queue[i].word)==0)
			{	
				strcpy(queue[i].word,collectBuf);
				score += 10;
				if(runTime>400){
					runTime -= SUB_MILLISECONDS;
					set_ticker(runTime);
				}
				memset(&input,0,sizeof(input));
				break;
			}
		}

		mvwprintw(win1,2,8,"%s",collectBuf);
		wrefresh(win1);
	
		curs_set(0);
  	}
}
