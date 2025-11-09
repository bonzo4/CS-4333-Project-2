#ifndef RECEIVER_HPP
#define RECEIVER_HPP

#include "utils.hpp"

#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <cstring>

struct ReceivePacketOptions {
    Packet& packet;
    int sockfd;
    struct sockaddr_in& senderAddr;
    socklen_t& senderAddrLen;
    uint32_t& packetIndex;
    std::ofstream& outFile;
    int& filesize;
    time_t& startTime;
};

struct SendAckOptions {
    int sockfd;
    struct sockaddr_in senderAddr;
    socklen_t senderAddrLen;
    uint32_t packetIndex;
};

class Receiver {
    public:
        Receiver() {};

        std::string getFilename() const { return filename_; }
        int getLocalPort() const { return localPort_; }
        int getMode() const { return mode_; }
        long getModeParameter() const { return modeParameter_; }

        void setFilename(const std::string& filename) { filename_ = filename; }
        void setLocalPort(int port) { localPort_ = port; }
        void setMode(int mode) { mode_ = mode; }
        void setModeParameter(long param) { modeParameter_ = param; }

        void receiveFile();

    private:
        std::string filename_;
        int localPort_;
        int senderPort_;
        int mode_;
        long modeParameter_;

        int receivePacket_(ReceivePacketOptions& options);
        void sendAck_(SendAckOptions& options);
};

#endif // RECEIVER_HPP