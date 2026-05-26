#include "client.h"

static shm_fifo_t *g_shmfifo = NULL;
static int g_msgid = 0;
static int g_sockfd = -1;
static int g_client_id = 0;
static pthread_t g_recv_tid = 0;
static volatile int g_running = 1;
static config_t g_cfg;

static void *network_recv_thread(void *arg)
{
    packet_t packet;
    while (g_running && g_sockfd >= 0) {
        if (net_recv_packet(g_sockfd, &packet) == 0) {
            if (packet.mode == PUBLISH) {
                printf("[Received] topic: %s, content: %s\n", packet.topic, packet.content);
            }
        } else {
            break;
        }
    }
    return NULL;
}

static void *local_recv_thread(void *arg)
{
    char buffer[64] = {0};
    while (g_running) {
        memset(buffer, 0, 64);
        if (msg_recv(g_msgid, getpid(), buffer, 64) == 0) {
            printf("[Received] content: %s\n", buffer);
        }
    }
    return NULL;
}

void client_init(const config_t *cfg)
{
    memcpy(&g_cfg, cfg, sizeof(config_t));
    fprintf(stderr, "Initializing client...\n");

    if (cfg->use_network) {
        g_sockfd = net_client_connect(cfg);
        if (g_sockfd >= 0) {
            pthread_create(&g_recv_tid, NULL, network_recv_thread, NULL);
            pthread_detach(g_recv_tid);
            fprintf(stderr, "Client initialized in network mode\n");
        } else {
            fprintf(stderr, "Warning: Failed to connect to server, falling back to local mode\n");
            //cfg->use_network = 0;
        }
    }

    if (!cfg->use_network) {
        g_shmfifo = shmfifo_init(cfg->shm_blocks, sizeof(packet_t));
        g_msgid = msg_create();
        pthread_create(&g_recv_tid, NULL, local_recv_thread, NULL);
        pthread_detach(g_recv_tid);
        fprintf(stderr, "Client initialized in local mode\n");
    }
}

void client_shutdown()
{
    g_running = 0;

    if (g_sockfd >= 0) {
        packet_t packet;
        memset(&packet, 0, sizeof(packet));
        packet.magic = MAGIC_NUM;
        packet.mode = UNSUBSCRIBE;
        net_send_packet(g_sockfd, &packet);
        net_close(g_sockfd);
    }

    if (g_shmfifo != NULL) {
        shmfifo_destroy(g_shmfifo);
    }

    fprintf(stderr, "Client shutdown complete\n");
}

void subscribe(char *topic)
{
    packet_t packet;
    memset(&packet, 0, sizeof(packet));
    packet.magic = MAGIC_NUM;
    strncpy(packet.topic, topic, TOPIC_SZ);
    packet.pid = getpid();
    packet.mode = SUBSCRIBE;

    if (g_cfg.use_network && g_sockfd >= 0) {
        net_send_packet(g_sockfd, &packet);
    } else if (g_shmfifo != NULL) {
        shmfifo_put(g_shmfifo, &packet);
    }
}

void publish(char *topic, char *content)
{
    packet_t packet;
    memset(&packet, 0, sizeof(packet));
    packet.magic = MAGIC_NUM;
    strncpy(packet.topic, topic, TOPIC_SZ);
    strncpy(packet.content, content, CONTENT_SZ);
    packet.pid = getpid();
    packet.mode = PUBLISH;

    if (g_cfg.use_network && g_sockfd >= 0) {
        net_send_packet(g_sockfd, &packet);
    } else if (g_shmfifo != NULL) {
        shmfifo_put(g_shmfifo, &packet);
    }
}

void unsubscribe()
{
    packet_t packet;
    memset(&packet, 0, sizeof(packet));
    packet.magic = MAGIC_NUM;
    packet.mode = UNSUBSCRIBE;

    if (g_cfg.use_network && g_sockfd >= 0) {
        net_send_packet(g_sockfd, &packet);
    }
}
