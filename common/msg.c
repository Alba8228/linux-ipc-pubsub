#include "msg.h"

#define MSG_SZ 64

struct msgbuf
{
	long mtype;
	char mtext[MSG_SZ];
};

int msg_create()
{
	key_t key = ftok("../common", 102);
	
	int msgid = msgget(key, 0);
	if(msgid == -1)
	{
		msgid = msgget(key, IPC_CREAT | 0644);
		if(msgid == -1){perror("msgget():\n"); exit(1);}
	}
	return msgid;
}

int msg_send(int msgid, long mtype, char *buf)
{
	struct msgbuf msg;
	msg.mtype = mtype;
	strcpy(msg.mtext, buf);
	
	return msgsnd(msgid, &msg, strlen(msg.mtext) + 1, 0);
}

int msg_recv(int msgid, long mtype, char *buf, int len)
{
	struct msgbuf msg;
	
	int ret = msgrcv(msgid, &msg, len, mtype, 0);
	if(ret == -1){perror("msgrcv():\n"); return -1;}
	
	strcpy(buf, msg.mtext);
	
	return ret;
}

int msg_del(int msgid)
{
	return msgctl(msgid, IPC_RMID, NULL);
}
