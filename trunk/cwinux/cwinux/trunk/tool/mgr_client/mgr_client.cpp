#include "CwxMgrClient.h"

int main(int argc, char** argv){
    if (argc != 3){
        printf("Miss addr/port.\n Using: %s addr  port\n", argv[0]);
        return 0;
    }
    CwxMgrClient client;
    if (0 != client.init(argv[1], strtoul(argv[2], NULL, 0))) return 0;
    client.run();
    return 0;
}

