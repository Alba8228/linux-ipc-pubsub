#ifndef __NET_H__
#define __NET_H__

#include "protocol.h"
#include "config.h"

#define MAX_CLIENTS 32

typedef struct {
    int sockfd;
    int client_id;
    struct sockaddr_in addr;
} client_conn_t;

int net_server_init(const config_t *cfg);
int net_server_accept(int server_fd, client_conn_t *conn);
int net_client_connect(const config_t *cfg);
int net_send_packet(int sockfd, const packet_t *packet);
int net_recv_packet(int sockfd, packet_t *packet);
void net_close(int sockfd);

#endif
