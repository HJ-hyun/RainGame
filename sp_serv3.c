#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 1024
#define NUM 12//11명 저장할거라
typedef struct userInfo{//현재 연결된 클라이언트 정보 저장   
   int fd;
   char username[100];
    int score;
}USER;
typedef struct userRank{//파일에 저장할 유저들의 정보 저장
    char username[100];
    int score;
}USERRANK;

USER user[10];
USERRANK rank[NUM];
int cnt=0;
void error_handling(char *buf);
void sort();

int main(int argc, char *argv[])
{
   int serv_sock, clnt_sock;
   struct sockaddr_in serv_adr, clnt_adr;
   struct timeval timeout;   
   fd_set reads, cpy_reads;
   socklen_t adr_sz;
   int fd_max, str_len, fd_num, i;
   char buf[BUF_SIZE];
    FILE*fp2;
   if(argc!=2) {
      printf("Usage : %s <port>\n", argv[0]);
      exit(1);
      }

   printf("<English Typing Game made by Comidle>\n");

   serv_sock=socket(PF_INET, SOCK_STREAM, 0);
   memset(&serv_adr, 0, sizeof(serv_adr));
   serv_adr.sin_family=AF_INET;
   serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
   serv_adr.sin_port=htons(atoi(argv[1]));
   
   if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1)
      error_handling("bind() error");
   if(listen(serv_sock, 5)==-1)
      error_handling("listen() error");
   
   FD_ZERO(&reads);
   FD_SET(serv_sock, &reads);
   fd_max=serv_sock;

   while(1)
   {
      cpy_reads=reads;
      timeout.tv_sec=5;
      timeout.tv_usec=5000;
      if((fd_num=select(fd_max+1, &cpy_reads, 0, 0, &timeout))==-1)//multiplexing
         break;
      if(fd_num==0)//변화x
         continue;

      for(i=0; i<fd_max+1; i++)
      {
         if(FD_ISSET(i, &cpy_reads))
         {
            if(i==serv_sock) //클라이언트에서 연결 요청
            {
               adr_sz=sizeof(clnt_adr);
               clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
               FD_SET(clnt_sock, &reads);
               if(fd_max < clnt_sock)
                  fd_max=clnt_sock;
               memset(buf,0,BUF_SIZE);
                    memset(user,0,sizeof(user));//초기화 안하면 연결 끊어도 같은 이름만 들어감
                    str_len=read(clnt_sock,buf,BUF_SIZE);//get username
               printf("connected user: %s %d\n", buf,clnt_sock);
               user[cnt].fd=clnt_sock;
               strcpy(user[cnt].username,buf);
               cnt++;
               printf("usernum: %d\n",cnt);
            }
            else // read message
            {
                    fp2=fdopen(i,"w");
               memset(buf,0,BUF_SIZE);
                    str_len=read(i, buf, BUF_SIZE);
               //printf("%d\n",str_len);
                    if(str_len==0) // close request
               {
                  FD_CLR(i, &reads);
                  //close(i);
                        fclose(fp2);
                        printf("closed client: %d \n", i);
               }
               else//end game get score
               {
                        int j=0,k=1;
                  while(user[j].fd!=i)
                            j++;
                        user[j].score=atoi(buf);//buf에 점수 저장되어있음
                        FILE*fp;
                        fp=fopen("rank.txt","r");
                        //FILE*fp2=fdopen(i,"w");
                        while(1){//파일에서 점수와 이름 읽어옴
                            fscanf(fp,"%s %d",rank[k].username,&rank[k].score);
                            if(feof(fp)!=0)
                                break;
                            k++;
                        }
                        fclose(fp);//rank file close
                        fp=fopen("rank.txt","w");
                        //현재 클라이언트의 이름과 점수 저장(원래 저장되어 있던 점수와 비교 하려고)
                        strcpy(rank[k].username,user[j].username);
                        rank[k].score=user[j].score;
                        //printf("%s %d\n",rank[k].username,rank[k].score);
                        sort();
                    
                        if(k<NUM-1){
                            for(int l=1;l<=k;l++){
                                fprintf(fp,"%s %d\n",rank[l].username,rank[l].score);
                                //fprintf(fp2,"%s %d\n",rank[l].username,rank[l].score);
                            }
                        }
                        else{//10명 이상
                            for(int l=1;l<11;l++){
                                fprintf(fp,"%s %d\n",rank[l].username,rank[l].score);
                            }
                        }
			
                        memset(rank,0,sizeof(rank));
                        fclose(fp);
                        //fclose(fp2);
               }
            }
         }
      }
   }
   close(serv_sock);
   return 0;

}

void error_handling(char *buf)
{
   fputs(buf, stderr);
   fputc('\n', stderr);
   exit(1);
}

void sort()
{
   int i,j;
   USERRANK tmp;
   for(i=1;i<NUM;i++){
      for(j=i+1;j<NUM;j++){
         if(rank[i].score<rank[j].score){
            tmp=rank[i];
            rank[i]=rank[j];
            rank[j]=tmp;
      
         }            
      }
   }
   
}



