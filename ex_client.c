#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SIZE 1024
void error_handling(char *message);
typedef struct userRank{
    char username[100];
    int score;
}userRank;
userRank rank[12];
int main(int argc, char* argv[])
{
	int sock;
	struct sockaddr_in serv_addr;
	char message[BUF_SIZE];
	int str_len;
	
	if(argc!=3){
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	sock=socket(PF_INET, SOCK_STREAM, 0);
	if(sock == -1)
		error_handling("socket() error");
	
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_addr.sin_port=htons(atoi(argv[2]));
		
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1) 
		error_handling("connect() error!");
    else 
        puts("Connected...........");

    for(int i=0;i<2;i++){
        fputs("input message(Q to quit):",stdout); 
        scanf("%s",message);
        if(!strcmp(message,"q")||!strcmp(message,"Q"))
            break;
        write(sock,message,strlen(message));
        }
        
    read(sock,rank,sizeof(rank));
    for(int i=1;i<11;i++){
        printf("%d %s %d\n",i,rank[i].username,rank[i].score);
    }

    close(sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
