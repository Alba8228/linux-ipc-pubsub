#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/select.h>
#include <errno.h>
#include <poll.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define TOPIC_SZ 64
#define CONTENT_SZ 64
#define BLOCKS 10
#define THREAD_CNT 10
#define TASK_CNT 10
#define RESULT_SZ 10
#define MAGIC_NUM 0x12345678

enum work_mode
{
	PUBLISH = 0,
	SUBSCRIBE,
	UNSUBSCRIBE,
	HEARTBEAT,
	ACK
};

typedef struct packet
{
	unsigned int magic;
	char topic[TOPIC_SZ];
	pid_t pid;
	int client_id;
	enum work_mode mode;
	char content[CONTENT_SZ];
}packet_t;

#endif
