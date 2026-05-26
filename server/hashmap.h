#ifndef __HASH_MAP_H_
#define __HASH_MAP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define HTABLE_LEN 31  // 增大表长度，使用质数

// 订阅者类型
typedef enum {
    SUBSCRIBER_LOCAL = 0,
    SUBSCRIBER_NETWORK
} subscriber_type_t;

// hashmap 数据类型
typedef struct datatype{
	char topic[32];
	pid_t pid;
	int client_id;
	subscriber_type_t type;
	int sockfd;
}datatype_t;

// hashmap 链表节点
typedef struct node{
	datatype_t data;
	struct node *next;
}hashtable_t;


hashtable_t **create_hashtable();// 创建 hashmap

// 插入数据 : key - value
void insert_data_hash(hashtable_t **h,char * key,datatype_t value);

// 查询数据 - 返回订阅者信息
int search_hash_table_ex(hashtable_t **h,char * key,datatype_t result[], int max_result);

// 兼容旧接口
int search_hash_table(hashtable_t **h,char * key,pid_t result[]);

// 删除特定客户端的订阅
int remove_subscriber(hashtable_t **h, int client_id, int sockfd);

// 销毁整个哈希表
void destroy_hash_table(hashtable_t **h);

// 输出 hashmap 
void printf_hash_table(hashtable_t **h);

// 计算 key 的 hashcode
int hashcode(const char *string);

#endif
