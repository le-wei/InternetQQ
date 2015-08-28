#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<fcntl.h>
#include<stdlib.h>
#include<Message.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<netinet/in.h>
#include"wrap.h"
#define SERVE_PORT 8000
#define SERVE_IP "192.168.42.27"
#define SERVE_FIFO "serve_public"
void sys_err(char *str)
{
	perror(str);
	exit(1);
}
int main(int argc,char* argv[])
{
	int plic_id,pive_id,read_id;
	struct sockaddr_in servaddr;
	int sockfd;

	int fbg;
	char buf[1024];
	Mesg loin,mes;
	if(argc<2)//判断输入格式是否正确
	{
		printf("%s\n","please input like:./clent username");
		return 0;
	}

	/*	//打包登录数据包
		loin.id=1;
		strcpy(loin.username,argv[1]);
	 */	
	sockfd = Socket(AF_INET,SOCK_STREAM,0);

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVE_PORT);
	inet_pton(AF_INET,SERVE_IP,&servaddr.sin_addr.s_addr);
	Connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
	int flags;
	flags = fcntl(STDIN_FILENO,F_GETFL);
	flags |=O_NONBLOCK;
	if(fcntl(STDIN_FILENO,F_SETFL,flags) == -1)
            sys_err("fcntl stdin error");
	int flags1;
	flags1 = fcntl(sockfd,F_GETFL);
	flags1 |=O_NONBLOCK;
	if(fcntl(sockfd,F_SETFL,flags) == -1)
            sys_err("fcntl stdin error");
	//从公共管道传送到服务器，进行登录
	//   write(sockfd,&loin,sizeof(loin));
	char tui[]="T~";
	char de[]="D~";
	while(1)
	{
		strcpy(mes.username,argv[1]);
		int n =0;
		n = read(STDIN_FILENO,buf,1024);
		if(n > 0)
		{
			mes.id = 2;
			buf[n]='\0';
			strcpy(mes.destname,strtok(buf," "));
			strcpy(mes.message,strtok(NULL," "));
			if((strncmp(mes.message,tui,2)==0))	
				mes.id = 0;
			if((strncmp(mes.message,de,2)==0))	
				mes.id = 1;
			write(sockfd,&mes,sizeof(mes));

		}
		int n1 =0;
		n1 = read(sockfd,&mes,sizeof(mes));
		if(n1>0)
		{
			printf("%s",mes.username);
			printf(":\n");
			printf("%s\n",mes.message);
			fflush(stdout);
			if(mes.id==-1)
			{
				exit(1);
			}
		}

	}
	return 0;
}
