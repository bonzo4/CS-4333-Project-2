#include "Receiver.hpp"
#include "utils.hpp"

void Receiver::receiveFile() {

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    
    if (sockfd < 0) {
        std::cerr << "[ERROR] Could not create socket" << std::endl;
        return;
    }
    
    struct sockaddr_in recvAddr;
    recvAddr.sin_family = AF_INET;
    recvAddr.sin_port = htons(getLocalPort());
    recvAddr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(sockfd, (struct sockaddr*)&recvAddr, sizeof(recvAddr)) < 0) {
        std::cerr << "[ERROR] Could not bind socket to port " << getLocalPort() << std::endl;
        close(sockfd);
        return;
    }
    
    std::cout << "[INFO] Receiving " << getFilename() 
              << " listening on 127.0.0.1:" << getLocalPort() 
              << std::endl;

    std::ofstream outFile(getFilename(), std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "[ERROR] Could not open output file " << getFilename() << std::endl;
        close(sockfd);
        return;
    }
    
    struct sockaddr_in senderAddr;
    socklen_t senderAddrLen = sizeof(senderAddr);
    
    Packet packet;
    uint32_t packetIndex = 1;

    std::cout << "[INFO] Waiting to receive file..." << std::endl;

    time_t startTime;
    int filesize = 0;

    ReceivePacketOptions options {
        .packet = packet,
        .sockfd = sockfd,
        .senderAddr = senderAddr,
        .senderAddrLen = senderAddrLen,
        .packetIndex = packetIndex,
        .outFile = outFile,
        .filesize = filesize,
        .startTime = startTime
    };

    while (receivePacket_(options)) { }

    outFile.close();
    close(sockfd);
    
    time_t endTime = time(nullptr);
    
    std::cout << "[INFO] Successfully received " << getFilename() 
              << " (" << filesize << " bytes) in " 
              << (endTime - startTime) << " seconds" << std::endl;
}