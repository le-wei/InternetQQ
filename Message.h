#ifndef _MESSAGE_H_
#define _MESSAGE_H_
#include<string.h>
#include<stdlib.h>
typedef struct loing
{
	int fid;
	char username[20];
	struct loing* next;
} loing;

loing * Init()
{
	loing* head =(loing*)malloc(sizeof(loing));
	head->next = NULL;
	return head;
}
int Selectname(loing* head, char* name)
{
	loing* p1 = head->next;
	int ret = 0;
	while(p1!=NULL)
	{
		if(strcmp(name,p1->username)==0)
		{
			ret = p1->fid;
			break;
		}
		p1=p1->next;
	}

	return ret;

}

int  Inset(loing* head,int id,char *name)
{
	int ret = 0;
	if((ret=Selectname(head,name))>0)
	{
		return ret;
	}
	else{
		loing *p2 = (loing*)malloc(sizeof(loing));
		p2->fid = id;
		strcpy(p2->username,name);
		p2->next = NULL;
		p2->next = head->next;
		head->next = p2;
		return ret;
	}
}
int Deletename(loing* head, char* name)
{
	loing* p1 = head->next;
	loing* p2 = head;
	int ret = 0;
	while(p1!=NULL)
	{
		if(strcmp(name,p1->username)==0)
		{
			ret =p1->fid;
			p2->next = p1->next;
			free(p1);
			break;
		}
		p2 = p2->next;
		p1=p1->next;
	}

	return ret;
}
typedef struct Mesg
{
	int id;
	char username[20];
	char destname[20];
	char message[1024];
}Mesg;
#endif
