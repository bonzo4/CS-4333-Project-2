#include "Receiver.hpp"
#include "utils.hpp"

int Receiver::receivePacket_(ReceivePacketOptions& options) {
    memset(&options.packet, 0, sizeof(options.packet));

    ssize_t bytesReceived = recvfrom(
        options.sockfd,                          // Socket
        &options.packet,                         // Buffer to store received data
        sizeof(Packet),                  // Max size to receive
        0,                               // Flags (0 = blocking)
        (struct sockaddr*)&options.senderAddr,   // Sender's address (OUTPUT)
        &options.senderAddrLen                   // Size of sender address (INPUT/OUTPUT)
    );

    if (options.packetIndex == 1) {
        options.startTime = time(nullptr);
    }

    std::cout << "[INFO] Message " << options.packet.packetIndex
                << " received with " << options.packet.dataSize
                << " bytes" << std::endl;

    options.filesize += options.packet.dataSize;

    if (options.packet.type == DATA && options.packet.packetIndex == options.packetIndex) {
        options.outFile.write(options.packet.data, options.packet.dataSize);
        
        SendAckOptions ackOptions {
            .sockfd = options.sockfd,
            .senderAddr = options.senderAddr,
            .senderAddrLen = options.senderAddrLen,
            .packetIndex = options.packet.packetIndex
        };

        sendAck_(ackOptions);

        options.packetIndex += 1;
    }

    if (options.packet.isLast == 1) {
        return 0;
    }

    return 1;
}