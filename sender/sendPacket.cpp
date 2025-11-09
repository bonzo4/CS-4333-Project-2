#include "Sender.hpp"

int Sender::sendPacket_(SendPacketOptions& options) {
    Packet dataPacket;
    memset(&dataPacket, 0, sizeof(dataPacket));

    options.file.read(dataPacket.data, getModeParameter() > 0 ? getModeParameter() : MAX_DATA_SIZE);
    dataPacket.dataSize = options.file.gcount();
    dataPacket.packetIndex = options.packetIndex;
    dataPacket.type = DATA;
    dataPacket.isLast = 0;

    if (options.file.eof()) {
        options.fileComplete = true;
        dataPacket.isLast = 1;
    } else if (dataPacket.dataSize == 0) {
        return 0;
    }

    WaitForAckOptions ackOptions {
        .socketfd = options.socketfd,
        .recvAddr = options.recvAddr,
        .packetIndex = options.packetIndex,
        .fileComplete = options.fileComplete,
        .ackReceived = false
    };

    while (!ackOptions.ackReceived) {
        ssize_t bytesSent = sendto(
            options.socketfd, 
            &dataPacket,
            sizeof(dataPacket), 
            0,
            (struct sockaddr*)&options.recvAddr, 
            sizeof(options.recvAddr));
            
        if (bytesSent < 0) {
            std::cerr << "[ERROR] Error sending packet" << std::endl;
            options.file.close();
            return 0;
        }

        std::cout << "[INFO] Message " << options.packetIndex 
                    << " sent with " << dataPacket.dataSize 
                    << " bytes of actual data" << std::endl;
        
        waitForAck_(ackOptions);
    }

    return 1;
}