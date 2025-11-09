#include "Sender.hpp"

void Sender::waitForAck_(WaitForAckOptions& options) {
    struct sockaddr_in ackAddr;
    socklen_t ackAddrLen = sizeof(ackAddr);
    Packet ackPacket;
    
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(options.socketfd, &readfds);

    struct timeval timeout;
    timeout.tv_sec = getTimeoutMs() / 1000;
    timeout.tv_usec = (getTimeoutMs() % 1000) * 1000;

    int selectResult = select(options.socketfd + 1, &readfds, NULL, NULL, &timeout);
    
    if (selectResult > 0) {
        ssize_t bytesReceived = recvfrom(options.socketfd, &ackPacket, sizeof(ackPacket), 0,
                                            (struct sockaddr*)&ackAddr, &ackAddrLen);

        if (bytesReceived > 0 && ackPacket.type == ACK && ackPacket.packetIndex == options.packetIndex) {
            std::cout << "[INFO] Message " << options.packetIndex << " acknowledged" << std::endl;
            options.ackReceived = true;
            options.packetIndex += 1;
        }
    } else if (selectResult == 0) {
        std::cout << "[INFO] Timeout waiting for acknowledgment, resending..." << std::endl;
    }
}