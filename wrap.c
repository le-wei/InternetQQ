#include<unistd.h>
#include<errno.h>
#include<stdlib.h>
#include<sys/socket.h>

void sys_err_exit(const char* str)
{
	perror(str);
	exit(1);
}
//对accept函数的从新封装
int Accept(int fd,struct sockaddr *sa,socklen_t *salenptr)
{
	int n;
again:
	if((n = accept(fd,sa,salenptr))<0)
	{
		if((errno == ECONNABORTED) || (errno == EINTR))
			//如果是网络重链接错误，或者时信号打断则从新接受链接
			goto again;
		else
			sys_err_exit("accept error");
	}
	return n;

}

//对bind函数的封装
void Bind(int fd, struct sockaddr* sa,socklen_t salen)
{
	if(bind(fd,sa,salen)<0)
		sys_err_exit("bind error");
}
//对connect函数的封装
void Connect(int fd,const struct sockaddr *sa,socklen_t salen)
{
	if(connect(fd,sa,salen) < 0)
		sys_err_exit("connect error");
}

//对listen函数的封装
void Listen(int fd,int backlog)
{
	if(listen(fd,backlog) < 0)
		sys_err_exit("listen error");
}

//对socket函数的封装

int Socket(int family,int type,int protocol)
{
	int n;
	if((n = socket(family,type,protocol)) < 0)
		sys_err_exit("socket error");
	return n;
}

//对read函数的封装

ssize_t Read(int fd,void *ptr,size_t nbytes)
{
	ssize_t n;
again:
	if((n = read(fd,ptr,nbytes)) == -1)
	{
		if(errno == EINTR)
			//如果时被信号打断从新去读
			goto again;
		else 
			return -1;
	}

	return n;

}

//对write函数的封装
ssize_t Write(int fd,const void *ptr,size_t nbytes)
{
	ssize_t n;
again:
	if((n=write(fd,ptr,nbytes))==-1)
	{
		if(errno == EINTR)
			goto again;
		else
			return -1;
	}

	return n;
}
//对close函数的封装
void Close(int fd)
{
	if(close(fd)==-1)
		sys_err_exit("close error");
}

//对读n个字节的封装
ssize_t Readn(int fd,void *vptr,size_t n)
{
	size_t nleft;//剩余的字节
	ssize_t nread; //已经读的字节
	char *ptr;

	ptr = vptr;
	nleft = n;
	while(nleft > 0)
	{
		if((nread = read(fd, ptr, nleft))<0)
		{
			if(errno == EINTR)
				//如果时被信号打断已经读的清除
				nread = 0;
		else 
			return -1;
		}
		else if(nread == 0)//另一端已经关闭
			break;

		nleft -= nread;//计算还剩余多少字节没有读
		ptr += nread;//指针后移到已经读的字节，以便下次从此处继续保存


	}
	return n-nleft;//返回实际读到的字节数
}

//对读n个字节的封装
ssize_t Writen(int fd,const void *vptr,size_t n)
{
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;

	ptr = vptr;
	nleft = n;
	while(nleft > 0)
	{
		if((nwritten = write(fd,ptr,nleft)) < 0)
		{
			if(nwritten < 0 && errno == EINTR)
				nwritten = 0;
			else 
				return -1;
		}
		nleft -= nwritten;
		ptr += nwritten;
	}

	return n;
}


//预读取函数
static ssize_t my_read(int fd,char *ptr)
{
	static int read_cnt;
	static char *read_ptr;
	static char read_buf[100];

	if(read_cnt <= 0)
	{
again:
		if((read_cnt = read(fd,read_buf,sizeof(read_buf)))<0)
		{
			if(errno == EINTR)
				goto again;
		return -1;
	}
	else if(read_cnt == 0)
		return 0;
	read_ptr = read_buf;
	}

	read_cnt-- ;
	*ptr = *read_ptr++;
	return 1;

	
}

//读一行函数封装
ssize_t ReadLine(int fd,void *vptr,size_t maxlen)
{
	ssize_t n,rc;
	char c,*ptr;

	ptr = vptr;
	for(n=1;n < maxlen;n++)
	{
		if((rc = my_read(fd,&c)) ==1 )
		{
			*ptr++ = c;
		if(c == '\n')
			break;
		}
		else if(rc == 0)
		{
			*ptr = 0;
			return n-1;
		}
		else 
		{
			return -1;
		}
	}
	*ptr = 0;
	return n;
}
