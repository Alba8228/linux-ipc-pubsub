#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static char *trim(char *str)
{
    char *end;

    while (isspace((unsigned char)*str)) str++;

    if (*str == 0)
        return str;

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    end[1] = '\0';
    return str;
}

void config_set_default(config_t *cfg)
{
    cfg->server_port = 8888;
    strcpy(cfg->server_ip, "127.0.0.1");
    cfg->thread_count = 10;
    cfg->queue_size = 10;
    cfg->shm_blocks = 10;
    cfg->use_network = 0;
}

int config_load(const char *filename, config_t *cfg)
{
    FILE *fp;
    char line[CONFIG_MAX_LINE];
    char key[CONFIG_MAX_KEY];
    char value[CONFIG_MAX_VALUE];
    char *p;

    config_set_default(cfg);

    fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Warning: Config file %s not found, using defaults\n", filename);
        return 0;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        char *trim_line = trim(line);

        if (*trim_line == '\0' || *trim_line == '#') {
            continue;
        }

        p = strchr(trim_line, '=');
        if (p == NULL) {
            continue;
        }

        *p = '\0';
        strncpy(key, trim(trim_line), CONFIG_MAX_KEY - 1);
        strncpy(value, trim(p + 1), CONFIG_MAX_VALUE - 1);

        if (strcmp(key, "server_port") == 0) {
            cfg->server_port = atoi(value);
        } else if (strcmp(key, "server_ip") == 0) {
            strncpy(cfg->server_ip, value, sizeof(cfg->server_ip) - 1);
        } else if (strcmp(key, "thread_count") == 0) {
            cfg->thread_count = atoi(value);
        } else if (strcmp(key, "queue_size") == 0) {
            cfg->queue_size = atoi(value);
        } else if (strcmp(key, "shm_blocks") == 0) {
            cfg->shm_blocks = atoi(value);
        } else if (strcmp(key, "use_network") == 0) {
            cfg->use_network = atoi(value);
        }
    }

    fclose(fp);
    return 0;
}

void config_print(const config_t *cfg)
{
    printf("=== Configuration ===\n");
    printf("server_ip: %s\n", cfg->server_ip);
    printf("server_port: %d\n", cfg->server_port);
    printf("thread_count: %d\n", cfg->thread_count);
    printf("queue_size: %d\n", cfg->queue_size);
    printf("shm_blocks: %d\n", cfg->shm_blocks);
    printf("use_network: %d\n", cfg->use_network);
}
