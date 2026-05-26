#ifndef __CONFIG_H__
#define __CONFIG_H__

#define CONFIG_MAX_LINE 256
#define CONFIG_MAX_KEY 64
#define CONFIG_MAX_VALUE 256

typedef struct {
    int server_port;
    char server_ip[64];
    int thread_count;
    int queue_size;
    int shm_blocks;
    int use_network;
} config_t;

int config_load(const char *filename, config_t *cfg);
void config_set_default(config_t *cfg);
void config_print(const config_t *cfg);

#endif
