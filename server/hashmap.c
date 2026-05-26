#include "hashmap.h"


hashtable_t **create_hashtable()//放的都是hashtable_t类型的指针
{
	int i;
	hashtable_t **h = NULL;
	h = (hashtable_t **)malloc(HTABLE_LEN * sizeof(hashtable_t *));

	memset(h,0,sizeof(HTABLE_LEN * sizeof(hashtable_t *)));

	for (i = 0;i < HTABLE_LEN;i++){
		h[i] = NULL;
	}

	return h;
}

// hashmap : key(字符串)<----> value(结构体)
void insert_data_hash(hashtable_t **h,char * key,datatype_t value)
{
	hashtable_t *temp = NULL;
	hashtable_t **p = NULL;

	int index = 0,i = 0;
	int code = 0;	

	code = hashcode(key);

	index = code % HTABLE_LEN;
	
	for (p = &h[index];*p != NULL;p = &((*p)->next)){//  **h[]一下相当于解了一下
		if ((*p)->data.pid > value.pid)
			break;
	}

	temp = (hashtable_t *)malloc(sizeof(hashtable_t));
	temp->data = value;
	temp->next = *p;
	*p = temp;
	return;
}

void printf_hash_table(hashtable_t **h)
{
	return;	
}

int hashcode(const char *string)//把topic转化为数字
{
	unsigned int hash = 5381;  // 使用经典的 DJB2 哈希算法
	int c;
	
	while ((c = *string++))
	{
		hash = ((hash << 5) + hash) + c; // hash * 33 + c
	}
	
	return (int)hash;
}

int search_hash_table(hashtable_t **h,char *key,pid_t result[])//根据topic（key）再转化来的数字做index
{
	int code = hashcode(key);
	int cnt = 0;
	int index = code % HTABLE_LEN;
	hashtable_t **p = NULL;
	
	for (p = &h[index];*p != NULL;p = &((*p)->next)){//在hashtable中寻找有几个订阅了该主题的pid
		if (strcmp(key,(*p)->data.topic) == 0){
			result[cnt++] = (*p)->data.pid;
		}
	}
	return cnt;
}

int search_hash_table_ex(hashtable_t **h, char *key, datatype_t result[], int max_result)
{
	int code = hashcode(key);
	int cnt = 0;
	int index = code % HTABLE_LEN;
	hashtable_t **p = NULL;
	
	for (p = &h[index];*p != NULL && cnt < max_result;p = &((*p)->next)){
		if (strcmp(key, (*p)->data.topic) == 0){
			result[cnt++] = (*p)->data;
		}
	}
	return cnt;
}

int remove_subscriber(hashtable_t **h, int client_id, int sockfd)
{
	int removed = 0;
	for (int i = 0; i < HTABLE_LEN; i++) {
		hashtable_t **p = &h[i];
		while (*p != NULL) {
			hashtable_t *curr = *p;
			int match = 0;
			
			if (client_id > 0 && curr->data.client_id == client_id) {
				match = 1;
			} else if (sockfd > 0 && curr->data.sockfd == sockfd) {
				match = 1;
			}
			
			if (match) {
				*p = curr->next;
				free(curr);
				removed++;
			} else {
				p = &curr->next;
			}
		}
	}
	return removed;
}

void destroy_hash_table(hashtable_t **h)
{
	if(h == NULL) return;
	
	for(int i = 0; i < HTABLE_LEN; i++)
	{
		hashtable_t *curr = h[i];
		while(curr != NULL)
		{
			hashtable_t *next = curr->next;
			free(curr);
			curr = next;
		}
	}
	
	free(h);
}
