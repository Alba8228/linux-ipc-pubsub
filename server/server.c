#include "server.h"

static shm_fifo_t *g_shmfifo = NULL;
static tpool_t *g_tpool = NULL;
static hashtable_t **g_hashtable = NULL;
static int g_msgid = 0;
static int g_server_fd = -1;
static client_conn_t g_clients[MAX_CLIENTS];
static int g_client_count = 0;
static volatile sig_atomic_t g_running = 1;
static config_t g_cfg;
static pthread_t g_shm_thread = 0;

static void signal_handler(int sig)
{
    fprintf(stderr, "Received signal %d, shutting down...\n", sig);
    g_running = 0;
}

static void *shm_reader_thread(void *arg)
{
    while (g_running && g_shmfifo != NULL) {
        packet_t *packet = (packet_t *)malloc(sizeof(packet_t));
        memset(packet, 0, sizeof(packet_t));
        shmfifo_get(g_shmfifo, packet);
        packet->magic = MAGIC_NUM;
        thread_pool_add_task(g_tpool, task_handler, packet);
    }
    return NULL;
}

void server_init(const config_t *cfg)
{
    memcpy(&g_cfg, cfg, sizeof(config_t));

    fprintf(stderr, "Initializing server...\n");

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    g_shmfifo = shmfifo_init(cfg->shm_blocks, sizeof(packet_t));
    if (g_shmfifo == NULL) {
        fprintf(stderr, "Error: Failed to init shared memory FIFO\n");
        exit(1);
    }

    g_tpool = thread_pool_init(cfg->thread_count, cfg->queue_size);
    if (g_tpool == NULL) {
        fprintf(stderr, "Error: Failed to init thread pool\n");
        exit(1);
    }

    g_hashtable = create_hashtable();
    if (g_hashtable == NULL) {
        fprintf(stderr, "Error: Failed to create hash table\n");
        exit(1);
    }

    g_msgid = msg_create();
    if (g_msgid < 0) {
        fprintf(stderr, "Error: Failed to create message queue\n");
        exit(1);
    }

    if (cfg->use_network) {
        g_server_fd = net_server_init(cfg);
        if (g_server_fd < 0) {
            fprintf(stderr, "Warning: Failed to init network server\n");
        }
    }

    memset(g_clients, 0, sizeof(g_clients));
    
    pthread_create(&g_shm_thread, NULL, shm_reader_thread, NULL);
    pthread_detach(g_shm_thread);
    
    fprintf(stderr, "Server initialized successfully\n");
}

void server_shutdown()
{
    fprintf(stderr, "Shutting down server...\n");

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (g_clients[i].sockfd > 0) {
            net_close(g_clients[i].sockfd);
        }
    }

    if (g_server_fd >= 0) {
        net_close(g_server_fd);
    }

    if (g_tpool != NULL) {
        thread_pool_destroy(g_tpool);
    }

    if (g_shmfifo != NULL) {
        shmfifo_destroy(g_shmfifo);
    }

    if (g_msgid >= 0) {
        msg_del(g_msgid);
    }

    if (g_hashtable != NULL) {
        destroy_hash_table(g_hashtable);
        g_hashtable = NULL;
    }

    fprintf(stderr, "Server shutdown complete\n");
}

void task_handler(void *arg)
{
	packet_t *packet = (packet_t *)arg;
	datatype_t subscribers[RESULT_SZ];
	
	if(packet->mode == PUBLISH) {
		int count = search_hash_table_ex(g_hashtable, packet->topic, subscribers, RESULT_SZ);

		for(int i = 0; i < count; i++) {
			if (subscribers[i].type == SUBSCRIBER_LOCAL) {
				msg_send(g_msgid, subscribers[i].pid, packet->content);
			} else if (subscribers[i].type == SUBSCRIBER_NETWORK) {
				packet_t reply;
				memset(&reply, 0, sizeof(reply));
				reply.magic = MAGIC_NUM;
				reply.mode = PUBLISH;
				strncpy(reply.topic, packet->topic, TOPIC_SZ);
				strncpy(reply.content, packet->content, CONTENT_SZ);
				net_send_packet(subscribers[i].sockfd, &reply);
			}
		}
	} else if(packet->mode == SUBSCRIBE) {
		datatype_t value;
		memset(&value, 0, sizeof(value));
		strncpy(value.topic, packet->topic, sizeof(value.topic) - 1);
		value.pid = packet->pid;
		value.client_id = packet->client_id;
		
		if (packet->client_id > 0) {
			value.type = SUBSCRIBER_NETWORK;
			for (int i = 0; i < MAX_CLIENTS; i++) {
				if (g_clients[i].client_id == packet->client_id) {
					value.sockfd = g_clients[i].sockfd;
					break;
				}
			}
		} else {
			value.type = SUBSCRIBER_LOCAL;
		}
		
		insert_data_hash(g_hashtable, packet->topic, value);
	} else if(packet->mode == UNSUBSCRIBE) {
		remove_subscriber(g_hashtable, packet->client_id, -1);
	} else if(packet->mode == HEARTBEAT) {
	} else {
	}
	
	printf_hash_table(g_hashtable);
	free(packet);
}

void runloop()
{
	while(g_running) {
		fd_set readfds;
		int max_fd = 0;
		struct timeval tv;
		int ret;

		FD_ZERO(&readfds);
		
		if (g_server_fd >= 0) {
			FD_SET(g_server_fd, &readfds);
			if (g_server_fd > max_fd) max_fd = g_server_fd;
		}

		for (int i = 0; i < MAX_CLIENTS; i++) {
			if (g_clients[i].sockfd > 0) {
				FD_SET(g_clients[i].sockfd, &readfds);
				if (g_clients[i].sockfd > max_fd) max_fd = g_clients[i].sockfd;
			}
		}

		tv.tv_sec = 1;
		tv.tv_usec = 0;

		ret = select(max_fd + 1, &readfds, NULL, NULL, &tv);

		if (ret < 0 && errno != EINTR) {
			fprintf(stderr, "Error: Select error\n");
			break;
		}

		if (g_server_fd >= 0 && FD_ISSET(g_server_fd, &readfds)) {
			if (g_client_count < MAX_CLIENTS) {
				client_conn_t new_conn;
				int client_id = net_server_accept(g_server_fd, &new_conn);
				if (client_id > 0) {
					for (int i = 0; i < MAX_CLIENTS; i++) {
						if (g_clients[i].sockfd <= 0) {
							g_clients[i] = new_conn;
							g_client_count++;
							break;
						}
					}
				}
			} else {
				fprintf(stderr, "Warning: Max clients reached\n");
			}
		}

		for (int i = 0; i < MAX_CLIENTS; i++) {
			if (g_clients[i].sockfd > 0 && FD_ISSET(g_clients[i].sockfd, &readfds)) {
				packet_t *packet = (packet_t *)malloc(sizeof(packet_t));
				if (net_recv_packet(g_clients[i].sockfd, packet) < 0) {
					remove_subscriber(g_hashtable, g_clients[i].client_id, g_clients[i].sockfd);
					net_close(g_clients[i].sockfd);
					memset(&g_clients[i], 0, sizeof(client_conn_t));
					g_client_count--;
					free(packet);
				} else {
					packet->client_id = g_clients[i].client_id;
					thread_pool_add_task(g_tpool, task_handler, packet);
				}
			}
		}
	}
}
