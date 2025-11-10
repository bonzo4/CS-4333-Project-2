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

    long filesize = file.seekg(0, std::ios::end).tellg();
    file.seekg(0, std::ios::beg);

    std::cout << "[INFO] Sending " << getFilename() 
              << " from 127.0.0.1:" << getLocalPort() 
              << " to 127.0.0.1:" << getRecvPort() 
              << " with " << filesize << " bytes"
              << std::endl;

    auto startTime = std::chrono::steady_clock::now();

    int packetIndex = 1;
    long windowSize = static_cast<long>(getModeParameter() / getMTU());
    std::map<int, PacketInfo> packetsToSend;
    bool allDataRead = false;

    while (!allDataRead || !packetsToSend.empty()) {
        // create packets and add packets to window
        while (!allDataRead && packetsToSend.size() < windowSize) {
            Packet dataPacket;
            memset(&dataPacket, 0, sizeof(dataPacket));

            file.read(dataPacket.data, getMTU());
            dataPacket.dataSize = file.gcount();
            dataPacket.packetIndex = packetIndex;
            dataPacket.isLast = false;

            if (file.eof()) {
                allDataRead = true;
                dataPacket.isLast = true;
            }
    
            PacketInfo packetInfo;
            packetInfo.packet = dataPacket;
            packetInfo.sentTime = std::chrono::steady_clock::now() - getTimeoutMs() * std::chrono::milliseconds(1);
            packetInfo.hasBeenSent = false;
            packetsToSend[packetIndex] = packetInfo;

            packetIndex++;
        }

        auto currentTime = std::chrono::steady_clock::now();

        // send packets
        for (auto& pair : packetsToSend) {
            int packetKey = pair.first;
            PacketInfo& packetInfo = pair.second;
            
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                currentTime - packetInfo.sentTime).count();
            
            if (elapsed > getTimeoutMs()) {
                if (packetInfo.hasBeenSent) {
                    std::cout << "[INFO] Message " << packetKey << " timed out" << std::endl;
                }
                
                static std::random_device rd;
                static std::mt19937 gen(rd());
                static std::uniform_real_distribution<> dis(0.0, 1.0);
                float randomValue = dis(gen);

                packetInfo.hasBeenSent = true;
                packetInfo.sentTime = currentTime;
                
                if (randomValue < getDropRate()) {
                    std::cout << "[INFO] Simulating packet loss for message " << packetKey << std::endl;
                } else {
                    ssize_t bytesSent = sendto(
                        socketfd,                       // socket file descriptor
                        &packetInfo.packet,             // buffer to send
                        sizeof(packetInfo.packet),      // size of the packet
                        0,                              // flags
                        (struct sockaddr*)&recvAddr,    // address of the receiver
                        sizeof(recvAddr)                // length of the receiver address
                    );
                    std::cout << "[INFO] Message " << packetKey 
                              << " sent with " << packetInfo.packet.dataSize 
                              << " bytes of actual data" << std::endl;
                }
            }
        }

        // receive ACKs
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(socketfd, &readfds);

        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 10000;

        int selectResult = select(socketfd + 1, &readfds, NULL, NULL, &timeout);
                
        if (selectResult > 0) {
            struct sockaddr_in ackAddr;
            socklen_t ackAddrLen = sizeof(ackAddr);
            AckPacket ackPacket;
            
            ssize_t bytesReceived = recvfrom(
                socketfd,                    // socket file descriptor
                &ackPacket,                  // buffer to store the ACK packet
                sizeof(ackPacket),           // size of the ACK packet
                MSG_DONTWAIT,                // flags
                (struct sockaddr*)&ackAddr,  // address of the sender
                &ackAddrLen                  // length of the sender address
            );

            int ackIndex = ackPacket.packetIndex;

            if (packetsToSend.find(ackIndex) != packetsToSend.end()) {
                std::cout << "[INFO] Message " << ackIndex << " acknowledged" << std::endl;

                packetsToSend.erase(ackIndex);
            } 
        }
    }
    
    close(socketfd);
    file.close();

    auto endTime = std::chrono::steady_clock::now();
    auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

    std::cout << "[INFO] Successfully transferred " << getFilename() 
              << " (" << filesize << " bytes) in " 
              << elapsedMs << " ms" << std::endl;
}