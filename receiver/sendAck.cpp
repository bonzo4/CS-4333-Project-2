#include "Receiver.hpp"
#include "utils.hpp"

void Receiver::sendAck_(SendAckOptions& options) {
    Packet ackPacket;
    memset(&ackPacket, 0, sizeof(ackPacket));
    ackPacket.type = ACK;
    ackPacket.packetIndex = options.packetIndex;
    ackPacket.dataSize = 0;

    sendto(options.sockfd, &ackPacket, sizeof(ackPacket), 0,
            (struct sockaddr*)&options.senderAddr, options.senderAddrLen);
    std::cout << "[INFO] Sent acknowledgment for Message " << options.packetIndex << std::endl;
}