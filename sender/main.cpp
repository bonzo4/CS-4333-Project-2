#include "utils.hpp"
#include "Sender.hpp"

#include <sys/socket.h>

int main (int argc, char* argv[]) {

    Config config;
    parseArgs(argc, argv, config);

    Sender sender;
    sender.setMode(static_cast<int>(config.arqType));
    sender.setModeParameter(config.mtu);
    sender.setFilename("manuscript.txt");
    sender.setLocalPort(8080);
    sender.setRecvPort(8081);
    sender.setTimeoutMs(10000);

    sender.sendFile();

    return 0;
}