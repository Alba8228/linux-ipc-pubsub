#include "server.h"

int main(int argc, char *argv[])
{
    config_t cfg;
    const char *config_file = (argc > 1) ? argv[1] : "server.conf";

    config_set_default(&cfg);
    config_load(config_file, &cfg);
    config_print(&cfg);

    server_init(&cfg);
    runloop();
    server_shutdown();

    return 0;
}
