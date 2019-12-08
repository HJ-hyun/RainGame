#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <curses.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>

#define BUF_SIZE 1024

//game
#define START_POINT 2
#define END_POINT 14
#define COL 10
#define SUB_MILLISECONDS 50
#define MAX 500
#define MAX_LIFE 100
#define MAX_WORD_COUNT 100
#define MAX_WORD_SIZE 30

#define NUM 12
typedef struct Rain
{
   int row;
   int col;
   char word[MAX_WORD_SIZE];
}Rain;

typedef struct userRank{//파일에 저장할 유저들의 정보 저장
    char username[100];
    int score;
    int num;
}USERRANK;

USERRANK rank[NUM];
//Rain queue[MAX];
Rain queue[MAX];
int rear=0;
int front=0;
int pos = START_POINT;
int dir = +1;
int runTime = 1000;
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

void* tret1=NULL;
void* tret2=NULL;
//startScreen
char name[10];

int Game();
void error_handling(char *message);
void startScreen();
void endScreen();
//game
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

int main(int argc, char *argv[])
{
   //int sock;
   char buff[MAX_WORD_SIZE];
      FILE *fp;
      int i=0;
   
   char buf[BUF_SIZE];
   int str_len,fd_num,fdmax;
   int servfd;
   struct sockaddr_in serv_adr;
   fd_set reads,temps;
   struct timeval timeout;
   char message[BUF_SIZE];
   

   //startScreen
   int row,col;

   /*timeout.tv_sec=3;
   timeout.tv_usec=0;*/
   
   if(argc!=3) {
      printf("Usage : %s <IP> <port>\n", argv[0]);
      exit(1);
   }
      
   
   servfd=socket(PF_INET, SOCK_STREAM, 0);   
   if(servfd==-1)
      error_handling("socket() error");
   
   memset(&serv_adr, 0, sizeof(serv_adr))   ;
   serv_adr.sin_family=AF_INET;
   serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
   serv_adr.sin_port=htons(atoi(argv[2]));
   
   if(connect(servfd, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
      error_handling("connect() error!");
   else{
      puts("Connected.....Serv ");
      
   }
   
   startScreen();
   write(servfd,name,sizeof(name));

   //game
   fp=fopen("test.txt","r");

      while(!feof(fp))
      {
            fscanf(fp,"%s",buff);
           strcpy(wordData[arrayCount++],buff);
      }
      fclose(fp);
   
   StartGame();
   //puts("3");
    char bff[20];
    sprintf(bff,"%d",score);
   write(servfd,bff,sizeof(bff));
   read(servfd, rank,sizeof(rank));
   
   sleep(3);
   endScreen();
   
   //close(sock);
   return 0;
}

void error_handling(char *message)
{
   fputs(message, stderr);
   fputc('\n', stderr);
   exit(1);
}

void startScreen(){
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
       
       getstr(name);
   clear();
   refresh();
       endwin();

}
void endScreen(){
   char buf[BUF_SIZE];
   initscr();
   int r=3,c=5;
   char s1[BUF_SIZE];    // 크기가 30인 char형 배열을 선언

    
   WINDOW *window1 =newwin(LINES-7 ,COLS-1,1,1);
   WINDOW *window2=newwin(4,COLS-1,20,1);
   box(window1,0,0);
   box(window2,0,0);
   
//mvwprintw(win0,queue[i].row,queue[i].col,queue[i].word);
         mvwprintw(window1,r-1,COLS/2-5,"<RANKING>");
   for(int i=1;i<=rank[1].num;i++){
        sprintf(s1, "%d %s %d", i, rank[i].username, rank[i].score);
      mvwprintw(window1,r+i,c,s1);
   }   
   sprintf(s1,"%s %s %d","<user>",name,score);
   mvwprintw(window2,2,2,s1);
   wrefresh(window1);
   wrefresh(window2);
   
   //mvwgetstr(window2,2,75,buf);
   sleep(15);
   clear();
   refresh();
   endwin();
   
   

}

//game

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
      
   //printf("1\n");
      pthread_join(t1,&tret1);
      printf("<Rank>\n");
      pthread_join(t2,&tret2);    
   //printf("End\n");
   clear();
   //refresh();      
   wrefresh(win0);
   wrefresh(win1);
   endwin();
      return;
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
   
   set_ticker(0);   
   clear();
   refresh();   
   //wrefresh(win0);
   //wrefresh(win1);
   endwin();
   return;   
     
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
   char sss[]="not exit";    
   static int retval = 999;     
   signal(SIGALRM,RainDrop);
     if(set_ticker(runTime)==-1)
            perror("set_ticker");
      else
            while(1){   
      if (lifeScore <= 0)
               {
               
                return;
      
           }    
      
            }
   
   return;
}

void *InputThread()
{
   int i;
   char buf[MAX_WORD_SIZE];
   char answer[]="INPUT: ";
   char res[MAX_WORD_SIZE];
    static int retval = 999;   
   while(1){
      if(lifeScore <= 0)
            {
            // 반환값 설정
              printf("a\n");
              clear();
        set_ticker(0);
      refresh();
      endwin();
               //pthread_exit((void*)&retval);
      return;
        }
      mvwprintw(win1,2,1,"%s",answer);
      mvwgetstr(win1, 2, 8, input);

      for(i=front; i<rear; i++)
      {
          if(lifeScore <= 0)
           {
             // 반환값 설정
                printf("b\n");
                set_ticker(0);
               clear();
            wrefresh(win0);
      wrefresh(win1);
            endwin();
                
         return;
         }
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