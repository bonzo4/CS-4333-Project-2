#include "Receiver.hpp"
#include <set>

void Receiver::receiveFile() {

    int socketFd = socket(AF_INET, SOCK_DGRAM, 0);

    if (socketFd < 0) {
        std::cerr << "[ERROR] Could not create socket" << std::endl;
        return;
    }
    
    struct sockaddr_in recvAddr;
    recvAddr.sin_family = AF_INET;
    recvAddr.sin_port = htons(getLocalPort());
    recvAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(socketFd, (struct sockaddr*)&recvAddr, sizeof(recvAddr)) < 0) {
        std::cerr << "[ERROR] Could not bind socket to port " << getLocalPort() << std::endl;
        close(socketFd);
        return;
    }
    
    std::cout << "[INFO] Receiving " << getFilename() 
              << " listening on 127.0.0.1:" << getLocalPort() 
              << std::endl;

    std::ofstream outFile(getFilename(), std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "[ERROR] Could not open output file " << getFilename() << std::endl;
        close(socketFd);
        return;
    }
    
    struct sockaddr_in senderAddr;
    socklen_t senderAddrLen = sizeof(senderAddr);
    
    Packet packet;

    std::cout << "[INFO] Waiting to receive file..." << std::endl;

    std::set<int> receivedPackets;
    long filesize = 0;
    int totalPackets = 0;

    while (totalPackets == 0 || receivedPackets.size() < totalPackets) {
        // receive packets
        ssize_t bytesReceived = recvfrom(
            socketFd,                        // socket file descriptor
            &packet,                         // buffer to store received packet
            sizeof(packet),                  // size of the buffer
            0,                               // flags
            (struct sockaddr*)&senderAddr,   // address of the sender
            &senderAddrLen                   // length of the sender address
        );                 

        if (bytesReceived <= 0) {
            continue;
        }

        if (packet.isLast) {
            totalPackets = packet.packetIndex + 1;
        }

        std::cout << "[INFO] Message " << packet.packetIndex << " received with " 
                  << packet.dataSize << " bytes of actual data" << std::endl;

        // write to file
        if (receivedPackets.find(packet.packetIndex) != receivedPackets.end()) {
            std::cout << "[INFO] Duplicate packet " << packet.packetIndex << " ignored" << std::endl;
        } else {
            outFile.seekp(packet.offset);
            outFile.write(packet.data, packet.dataSize);
            outFile.flush();
            
            receivedPackets.insert(packet.packetIndex);
            filesize += packet.dataSize;
        }

        // send ACK
        Packet ackPacket;
        ackPacket.packetIndex = packet.packetIndex;
        size_t bytesSent = sendto(
            socketFd,                       // socket file descriptor 
            &ackPacket,                     // buffer to send
            sizeof(ackPacket),              // size of the ACK packet
            0,                              // flags
            (struct sockaddr*)&senderAddr,  // address of the sender
            senderAddrLen                   // length of the sender address
        );

        if (bytesSent <= 0) {
            std::cerr << "[ERROR] Could not send acknowledgment for message " 
                      << packet.packetIndex << std::endl;
        } else {
            std::cout << "[INFO] Sending acknowledgment for message " << packet.packetIndex << std::endl;
        }
    }

    outFile.close();
    close(socketFd);
    
    time_t endTime = time(nullptr);
    
    std::cout << "[INFO] Successfully received " << getFilename() 
              << " (" << filesize << " bytes)" << std::endl;
}