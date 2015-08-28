#include<signal.h>
#include<pthread.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<fcntl.h>
#include<stdlib.h>
#include<Message.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include"wrap.h"
#define SERVE_PORT 8000
struct s_info
{
	struct sockaddr_in cliaddr;
	int connfd;
};
loing* head=NULL;
void sys_err(char *str)
{
	perror(str);
	exit(1);
}
pthread_mutex_t conter_mutex = PTHREAD_MUTEX_INITIALIZER;
void dong(int i)
{


	/* int ret = Deletename(head,mes.destname);
	   int	write_id = Selectname(head,mes.username);
	   char buf[]=":异常下线";
	   char name[50];
	   strcpy(name,mes.username);
	   strcat(name,buf);
	   strcpy(mes.username,mes.destname);
	   strcpy(mes.message,name);
	   write(write_id,&mes,sizeof(mes));*/
	while(waitpid(0,NULL,WNOHANG)>0)
		;

}
void *do_work(void *arg)
{
	Mesg mes;
	struct s_info *ts = (struct s_info*)arg;
	int cilenfd = ts->connfd;
    char str[INET_ADDRSTRLEN];
	printf("Mssage from IP %s at PORT %d \n ",inet_ntop(AF_INET,&(*ts).cliaddr.sin_addr,str,sizeof(str)),ntohs((*ts).cliaddr.sin_port));
	pthread_detach(pthread_self());
	while(1)
	{
		int n = 0;
		//从公共管道中读取数据
		n = read(cilenfd,&mes,sizeof(mes));

		if(n>0)
		{     //如果id是1则代表登录
			if(mes.id == 1)
			{

		//		if(head == NULL)
		//			printf("head is NULL\n");
				//登录时打开私有管道，并把pid写如链表
			//	pthread_mutex_lock(&conter_mutex);
				int cilenfd1= Inset(head,cilenfd,mes.username);
			//	pthread_mutex_unlock(&conter_mutex);

		//		printf("%s,%d,%d\n",mes.username,cilenfd1,cilenfd);
				if(cilenfd1>0)//此处说明此用户已经登录过
				{
					strcpy(mes.username,mes.username);
					char mms[]="请不要重复登录";
					strcpy(mes.message,mms);
					mes.id=-1;
					write(cilenfd1,&mes,sizeof(mes));
					

				}
				else
				{
					strcpy(mes.username,mes.username);
					char bbf[]="登录成功";
					strcpy(mes.message,bbf);
					write(cilenfd,&mes,sizeof(mes));
				
				}
			}
			if(mes.id == 2)
			{
				if(strcmp(mes.username,mes.destname)==0)
					continue;
				int cilenfd1 = Selectname(head,mes.destname);
			//	printf("%s , %d\n",mes.destname,cilenfd1);
				if(cilenfd1 == 0)
				{
					cilenfd1 = Selectname(head,mes.username);
					char mms[50];
					strcpy(mms,mes.destname);
					char mm[] = " :没有在线，请稍等片刻";
					strcat(mms,mm);
					strcpy(mes.username,mes.username);
					strcpy(mes.message,mms);
					write(cilenfd1,&mes,sizeof(mes));
				
				}
				else
					write(cilenfd1,&mes,sizeof(mes));
					
			}
			if(mes.id == 0)
			{
				int cilenfd1 = Deletename(head,mes.username);
				char tui[] ="退出成功！";
				strcpy(mes.username,mes.username);
				strcpy(mes.message,tui);
				mes.id = -1;
				write(cilenfd1,&mes,sizeof(mes));
			}
		}
	}
	Close(ts->connfd);
//	return (void*)0;

}

int main(void)
{
	head = Init();
	int servefd,cilenfd;
	char buf[1024];
	struct sockaddr_in serve,clien;
	char str[INET_ADDRSTRLEN];
	socklen_t cilen_len;
	//signal(SIGPIPE,dong);
	struct s_info ts[300];
	int t = 0;
	pthread_t tid;
	servefd = Socket(AF_INET,SOCK_STREAM,0);
	bzero(&serve,sizeof(serve));
	serve.sin_family = AF_INET;
	serve.sin_port = htons(SERVE_PORT);
	serve.sin_addr.s_addr = htonl(INADDR_ANY);

	int pot = 1;
	setsockopt(servefd,SOL_SOCKET,SO_REUSEADDR,&pot,sizeof(pot));
	Bind(servefd,(struct sockaddr*)&serve,sizeof(serve));

	Listen(servefd,20);
	printf("Accepting connections .....\n");

	while(1)
	{
		cilen_len = sizeof(clien);
		cilenfd = Accept(servefd,(struct sockaddr*)&clien,&cilen_len);
		ts[t].cliaddr = clien;
		ts[t].connfd = cilenfd;
		if(t>=300)
		{
			printf("cilen is too much\n");
			return 0;
		}
		pthread_create(&tid,NULL,do_work,(void*)&ts[t]);
		t++;
//		printf("t=%d\n",t);
	}
	return 0;
}
