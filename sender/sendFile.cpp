#include "Sender.hpp"
#include "utils.hpp"

void Sender::sendFile() {

    std::ifstream file(getFilename(), std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "[ERROR] Could not open file " << getFilename() << std::endl;
        return;
    }
    
    struct sockaddr_in recvAddr;
    recvAddr.sin_family = AF_INET;
    recvAddr.sin_port = htons(getRecvPort());
    inet_pton(AF_INET, "127.0.0.1", &recvAddr.sin_addr);

    int socketfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (socketfd < 0) {
        std::cerr << "[ERROR] Could not create socket" << std::endl;
        file.close();
        return;
    }

    uint32_t packetIndex = 1;
    bool fileComplete = false;

    int filesize = file.seekg(0, std::ios::end).tellg();
    file.seekg(0, std::ios::beg);

    std::cout << "[INFO] Sending " << getFilename() 
              << " from 127.0.0.1:" << getLocalPort() 
              << " to 127.0.0.1:" << getRecvPort() 
              << " with " << filesize << " bytes"
              << std::endl;

    time_t startTime = time(nullptr);

    SendPacketOptions options {
        .file = file,
        .socketfd = socketfd,
        .recvAddr = recvAddr,
        .packetIndex = packetIndex,
        .fileComplete = fileComplete
    };

    while (!options.fileComplete && sendPacket_(options)) { }
    
    close(socketfd);
    file.close();

    time_t endTime = time(nullptr);

    std::cout << "[INFO] Successfully transferred " << getFilename() 
              << " (" << filesize << " bytes) in " 
              << (endTime - startTime) << " seconds" << std::endl;
}