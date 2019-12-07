#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 1024
#define NUM 11
typedef struct userInfo{
	
	int fd;
	char username[100];
	int score;
}USER;

USER user[10];

typedef struct userRank{
	int rank;
	char username[100];
	int score;
}USERRANK;

USERRANK rank[NUM];
int cnt=0;
void error_handling(char *buf);

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	struct timeval timeout;	
	fd_set reads, cpy_reads;
	socklen_t adr_sz;
	int fd_max, str_len, fd_num, i;
	char buf[BUF_SIZE];
	

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
		if((fd_num=select(fd_max+1, &cpy_reads, 0, 0, &timeout))==-1)
			break;
		if(fd_num==0)
			continue;

		for(i=0; i<fd_max+1; i++)
		{
			if(FD_ISSET(i, &cpy_reads))
			{
				if(i==serv_sock) // connection request!
				{
					adr_sz=sizeof(clnt_adr);
					clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
					FD_SET(clnt_sock, &reads);
					if(fd_max < clnt_sock)
						fd_max=clnt_sock;
					str_len=read(clnt_sock,buf,BUF_SIZE);

					printf("connected user: %s %d\n", buf,clnt_sock);
					user[cnt].fd=clnt_sock;
					strcpy(user[cnt].username,buf);
					cnt++;
					printf("usernum: %d\n",cnt);
					
				}
				else // read message!
				{
					str_len=read(i, buf, BUF_SIZE);
					if(str_len==0) // close request!
					{
						FD_CLR(i, &reads);
						close(i);
						printf("closed client: %d \n", i);
					}
					else
					{
						int j=0;
						while(user[j].fd!=i)
							j++;
						user[j].score=atoi(buf);

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

void sort(){

	int i,j;
	USERRANK tmp;
	for(i=0;i<NUM:i++){
		for(j=i+1;j<NUM;j++){
			if(rank[i].score<rank[j].score){
				tmp=rank[i];
				rank[i]=rank[j];
				rank[j]=tmp;
		
			}				
		}
	}
	
	
}
	



