#include "client.h"

int main(int argc, char *argv[])
{
    char buffer1[64] = {0};
    char *argv_cmd[5];
    int i = 0;
    config_t cfg;
    const char *config_file = (argc > 1) ? argv[1] : "client.conf";

    config_set_default(&cfg);
    config_load(config_file, &cfg);
    config_print(&cfg);

    client_init(&cfg);

    printf("Pub/Sub Client\n");
    printf("Commands:\n");
    printf("  publish <topic> <content>  - Publish a message\n");
    printf("  subscribe <topic>          - Subscribe to a topic\n");
    printf("  exit                       - Exit the client\n");
    printf("> ");

    while(1) {
        memset(buffer1, 0, 64);
        if (fgets(buffer1, sizeof(buffer1), stdin) == NULL) break;

        char *newline = strchr(buffer1, '\n');
        if (newline) *newline = '\0';

        for(int j = 0; j < 5; j++) argv_cmd[j] = NULL;

        argv_cmd[0] = strtok(buffer1, " ");
        if(argv_cmd[0] == NULL) {
            printf("> ");
            continue;
        }

        if(strcmp(argv_cmd[0], "exit") == 0) break;

        i = 0;
        while(1) {
            argv_cmd[++i] = strtok(NULL, " ");
            if(argv_cmd[i] == NULL || i >= 4) break;
        }

        if(strncmp(argv_cmd[0], "publish", 7) == 0 && argv_cmd[1] && argv_cmd[2]) {
            publish(argv_cmd[1], argv_cmd[2]);
        } else if(strncmp(argv_cmd[0], "subscribe", 9) == 0 && argv_cmd[1]) {
            subscribe(argv_cmd[1]);
        } else {
            fprintf(stderr, "Invalid command or parameters!\n");
        }
        printf("> ");
    }

    client_shutdown();
    return 0;
}
