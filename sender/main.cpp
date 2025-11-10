#include "utils.hpp"
#include "Sender.hpp"

#include <sys/socket.h>

int main (int argc, char* argv[]) {

    Config config;
    parseArgs(argc, argv, config);

    Sender sender;
    sender.setMode(static_cast<int>(config.arqType));

    if (config.arqType == ARQMode::STOP_AND_WAIT) {
        sender.setModeParameter(config.mtu);
    } else {
        sender.setModeParameter(config.modeParameter);
    }
    sender.setMTU(config.mtu);
    sender.setFilename("manuscript.txt");
    sender.setLocalPort(8080);
    sender.setRecvPort(8081);
    sender.setTimeoutMs(1000);

    sender.setDropRate(config.dropRate);

    std::cout << "[INFO] Configuration - ARQ Mode: " 
              << (config.arqType == ARQMode::STOP_AND_WAIT ? "Stop-and-Wait" : "Sliding Window")
              << ", Drop Rate: " << config.dropRate << "%"
              << std::endl;

    sender.sendFile();

    return 0;
}