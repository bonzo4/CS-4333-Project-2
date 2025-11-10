#include "Receiver.hpp"
#include "utils.hpp"
#include <random>
#include <thread>
#include <chrono>

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

    std::cout << "[INFO] Waiting to receive file..." << std::endl;

    time_t startTime;
    long filesize = 0;

    std::map<int, Packet> receiveBuffer;
    int totalPackets = 0;

    while (true) {
        // receive packets
        ssize_t bytesReceived = recvfrom(sockfd, &packet, sizeof(packet), 0,
                                        (struct sockaddr*)&senderAddr, &senderAddrLen);

        if (bytesReceived <= 0 || packet.type != DATA) {
            continue;
        }

        std::cout << "[INFO] Message " << packet.packetIndex << " received with " 
                  << packet.dataSize << " bytes of actual data" << std::endl;

        receiveBuffer[packet.packetIndex] = packet;

        // send ACK
        Packet ackPacket;
        ackPacket.type = ACK;
        ackPacket.packetIndex = packet.packetIndex;
        sendto(sockfd, &ackPacket, sizeof(ackPacket), 0,
               (struct sockaddr*)&senderAddr, senderAddrLen);
        std::cout << "[INFO] Sending acknowledgment for message " << packet.packetIndex << std::endl;

        if (packet.isLast) {
            totalPackets = packet.packetIndex;
        }

        if (totalPackets > 0 && receiveBuffer.size() == totalPackets) {
            break;
        }
    }

    // write received data to file in order
    for (int i = 1; i <= totalPackets; i++) {
        if (receiveBuffer.find(i) != receiveBuffer.end()) {
            Packet& pkt = receiveBuffer[i];
            outFile.write(pkt.data, pkt.dataSize);
            filesize += pkt.dataSize;
        } else {
            std::cerr << "[ERROR] Missing packet " << i << std::endl;
        }
    }

    outFile.close();
    close(sockfd);
    
    time_t endTime = time(nullptr);
    
    std::cout << "[INFO] Successfully received " << getFilename() 
              << " (" << filesize << " bytes)" << std::endl;
}