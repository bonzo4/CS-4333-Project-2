#include "Receiver.hpp"
#include "utils.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <cstring>


void Receiver::receiveFile() {

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    
    if (sockfd < 0) {
        std::cerr << "Error: Could not create socket" << std::endl;
        return;
    }
    
    struct sockaddr_in recvAddr;
    recvAddr.sin_family = AF_INET;
    recvAddr.sin_port = htons(getLocalPort());
    recvAddr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(sockfd, (struct sockaddr*)&recvAddr, sizeof(recvAddr)) < 0) {
        std::cerr << "[ERROR]Could not bind socket to port " << getLocalPort() << std::endl;
        close(sockfd);
        return;
    }
    
    std::cout << "[INFO]Receiving " << getFilename() 
              << " listening on 127.0.0.1:" << getLocalPort() 
              << std::endl;

    std::ofstream outFile(getFilename(), std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "[ERROR]Could not open output file " << getFilename() << std::endl;
        close(sockfd);
        return;
    }
    
    struct sockaddr_in senderAddr;
    socklen_t senderAddrLen = sizeof(senderAddr);
    
    Packet packet;
    uint32_t packetIndex = 1;

    std::cout << "[INFO]Waiting to receive file..." << std::endl;

    time_t startTime = time(nullptr);
    int filesize = 0;

    while (true) {
        memset(&packet, 0, sizeof(packet));
        
        ssize_t bytesReceived = recvfrom(
            sockfd,                          // Socket
            &packet,                         // Buffer to store received data
            sizeof(Packet),                  // Max size to receive
            0,                               // Flags (0 = blocking)
            (struct sockaddr*)&senderAddr,   // Sender's address (OUTPUT)
            &senderAddrLen                   // Size of sender address (INPUT/OUTPUT)
        );
        
        if (bytesReceived < 0) {
            perror("recvfrom failed");
            continue;
        }

        std::cout << "[INFO]Message " << packet.packetIndex
                  << " received with " << packet.dataSize
                  << " bytes" << std::endl;

        filesize += packet.dataSize;
                  
        
        if (packet.type == DATA && packet.packetIndex == packetIndex) {
            outFile.write(packet.data, packet.dataSize);

            Packet ackPacket;
            memset(&ackPacket, 0, sizeof(ackPacket));
            ackPacket.type = ACK;
            ackPacket.packetIndex = packetIndex;
            ackPacket.dataSize = 0;
            
            sendto(sockfd, &ackPacket, sizeof(ackPacket), 0,
                   (struct sockaddr*)&senderAddr, senderAddrLen);

            std::cout << "[INFO]Sent acknowledgment for Message " << packetIndex << std::endl;

            if (packet.isLast == 1) {
                break;
            }

            packetIndex += 1;
        } else if (packet.type == DATA) {
            std::cout << "[INFO]Duplicate packet received (expected index=" << packetIndex 
                      << ", got index=" << packet.packetIndex << ")" << std::endl;

            Packet ackPacket;
            memset(&ackPacket, 0, sizeof(ackPacket));
            ackPacket.type = ACK;
            ackPacket.packetIndex = 1 - packetIndex;
            ackPacket.dataSize = 0;
            
            sendto(sockfd, &ackPacket, sizeof(ackPacket), 0,
                   (struct sockaddr*)&senderAddr, senderAddrLen);

            std::cout << "[INFO]Sent duplicate ACK for index=" << (1 - packetIndex) << std::endl;
        }
    }
    
    outFile.close();
    close(sockfd);
    
    time_t endTime = time(nullptr);
    
    std::cout << "[INFO]Successfully received " << getFilename() 
              << " (" << filesize << " bytes) in " 
              << (endTime - startTime) << " seconds" << std::endl;
}