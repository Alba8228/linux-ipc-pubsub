#include "net.h"
#include <unistd.h>
#include <string.h>

int net_server_init(const config_t *cfg)
{
    int server_fd;
    struct sockaddr_in server_addr;
    int opt = 1;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        fprintf(stderr, "Error: Failed to create socket\n");
        return -1;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        fprintf(stderr, "Error: Failed to set socket options\n");
        close(server_fd);
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(cfg->server_port);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        fprintf(stderr, "Error: Failed to bind socket to port %d\n", cfg->server_port);
        close(server_fd);
        return -1;
    }

    if (listen(server_fd, MAX_CLIENTS) < 0) {
        fprintf(stderr, "Error: Failed to listen on socket\n");
        close(server_fd);
        return -1;
    }

    fprintf(stderr, "Server listening on port %d\n", cfg->server_port);
    return server_fd;
}

int net_server_accept(int server_fd, client_conn_t *conn)
{
    socklen_t addr_len = sizeof(conn->addr);
    static int next_client_id = 1;

    conn->sockfd = accept(server_fd, (struct sockaddr *)&conn->addr, &addr_len);
    if (conn->sockfd < 0) {
        fprintf(stderr, "Error: Failed to accept connection\n");
        return -1;
    }

    conn->client_id = next_client_id++;
    fprintf(stderr, "New client connected: %s:%d (client_id=%d)\n",
             inet_ntoa(conn->addr.sin_addr), ntohs(conn->addr.sin_port), conn->client_id);
    return conn->client_id;
}

int net_client_connect(const config_t *cfg)
{
    int sockfd;
    struct sockaddr_in server_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        fprintf(stderr, "Error: Failed to create client socket\n");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(cfg->server_port);
    if (inet_pton(AF_INET, cfg->server_ip, &server_addr.sin_addr) <= 0) {
        fprintf(stderr, "Error: Invalid server IP address: %s\n", cfg->server_ip);
        close(sockfd);
        return -1;
    }

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        fprintf(stderr, "Error: Failed to connect to server %s:%d\n", cfg->server_ip, cfg->server_port);
        close(sockfd);
        return -1;
    }

    fprintf(stderr, "Connected to server %s:%d\n", cfg->server_ip, cfg->server_port);
    return sockfd;
}

int net_send_packet(int sockfd, const packet_t *packet)
{
    int total = sizeof(packet_t);
    int sent = 0;
    const char *buf = (const char *)packet;

    while (sent < total) {
        int n = send(sockfd, buf + sent, total - sent, 0);
        if (n < 0) {
            fprintf(stderr, "Error: Failed to send packet\n");
            return -1;
        }
        sent += n;
    }
    return 0;
}

int net_recv_packet(int sockfd, packet_t *packet)
{
    int total = sizeof(packet_t);
    int received = 0;
    char *buf = (char *)packet;

    memset(packet, 0, sizeof(packet_t));

    while (received < total) {
        int n = recv(sockfd, buf + received, total - received, 0);
        if (n <= 0) {
            return -1;
        }
        received += n;
    }

    if (packet->magic != MAGIC_NUM) {
        fprintf(stderr, "Error: Invalid magic number in packet\n");
        return -1;
    }

    return 0;
}

void net_close(int sockfd)
{
    if (sockfd >= 0) {
        close(sockfd);
    }
}
