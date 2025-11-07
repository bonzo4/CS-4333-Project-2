#include "Sender.hpp"
#include "utils.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <cstring>
#include <sys/select.h>


void Sender::sendFile() {
    std::ifstream file(getFilename(), std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "[ERROR]Could not open file " << getFilename() << std::endl;
        return;
    }
    
    struct sockaddr_in recvAddr;
    recvAddr.sin_family = AF_INET;
    recvAddr.sin_port = htons(getRecvPort());
    inet_pton(AF_INET, "127.0.0.1", &recvAddr.sin_addr);

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0) {
        std::cerr << "[ERROR]Could not create socket" << std::endl;
        file.close();
        return;
    }

    uint32_t packetIndex = 1;
    bool fileComplete = false;

    int filesize = file.seekg(0, std::ios::end).tellg();
    file.seekg(0, std::ios::beg);

    std::cout << "[INFO]Sending " << getFilename() 
              << " from 127.0.0.1:" << getLocalPort() 
              << " to 127.0.0.1:" << getRecvPort() 
              << " with " << filesize << " bytes"
              << std::endl;

    time_t startTime = time(nullptr);
    
    while (!fileComplete) {
        Packet dataPacket;
        memset(&dataPacket, 0, sizeof(dataPacket));
        
        file.read(dataPacket.data, getModeParameter() > 0 ? getModeParameter() : MAX_DATA_SIZE);
        dataPacket.dataSize = file.gcount();
        dataPacket.packetIndex = packetIndex;
        dataPacket.type = DATA;
        dataPacket.isLast = 0;
        
        if (file.eof() || dataPacket.dataSize == 0) {
            fileComplete = true;
            if (dataPacket.dataSize == 0) {
                break;
            }
            dataPacket.isLast = 1;
        }
        
        bool ackReceived = false;
        
        while (!ackReceived) {
            ssize_t bytesSent = sendto(sockfd, &dataPacket, sizeof(dataPacket), 0,
                                       (struct sockaddr*)&recvAddr, sizeof(recvAddr));
            
            if (bytesSent < 0) {
                std::cerr << "[ERROR]Error sending packet" << std::endl;
                file.close();
                return;
            }

            std::cout << "[INFO]Message " << packetIndex 
                      << " sent with " << dataPacket.dataSize 
                      << " bytes of actual data" << std::endl;
            

            struct sockaddr_in ackAddr;
            socklen_t ackAddrLen = sizeof(ackAddr);
            Packet ackPacket;
            
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(sockfd, &readfds);
            
            struct timeval timeout;
            timeout.tv_sec = getTimeoutMs() / 1000;
            timeout.tv_usec = (getTimeoutMs() % 1000) * 1000;
            
            int selectResult = select(sockfd + 1, &readfds, NULL, NULL, &timeout);
            
            if (selectResult > 0) {
                ssize_t bytesReceived = recvfrom(sockfd, &ackPacket, sizeof(ackPacket), 0,
                                                  (struct sockaddr*)&ackAddr, &ackAddrLen);
                
                if (bytesReceived > 0 && ackPacket.type == ACK && ackPacket.packetIndex == packetIndex) {
                    std::cout << "[INFO]Message " << packetIndex << " acknowledged" << std::endl;
                    ackReceived = true;
                    packetIndex += 1;
                }
            } else if (selectResult == 0) {
                std::cout << "[INFO]Timeout waiting for acknowledgment, resending..." << std::endl;
            }
        }
    }

    file.close();

    time_t endTime = time(nullptr);

    std::cout << "[INFO]Successfully transferred " << getFilename() 
              << " (" << filesize << " bytes) in " 
              << (endTime - startTime) << " seconds" << std::endl;
}