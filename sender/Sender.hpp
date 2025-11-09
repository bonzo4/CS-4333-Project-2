#ifndef SENDER_HPP
#define SENDER_HPP

#include "utils.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <cstring>
#include <sys/select.h>

struct SendPacketOptions {
    std::ifstream& file;
    int socketfd;
    struct sockaddr_in recvAddr;
    uint32_t packetIndex;
    bool fileComplete;
};

struct WaitForAckOptions {
    int socketfd;
    struct sockaddr_in recvAddr;
    uint32_t& packetIndex;
    bool& fileComplete;
    bool ackReceived = false;
};

class Sender {
    public:
        Sender() {};

        std::string getFilename() const { return filename_; }
        int getLocalPort() const { return localPort_; }
        int getRecvPort() const { return recvPort_; }
        int getMode() const { return mode_; }
        long getModeParameter() const { return modeParameter_; }
        long getTimeoutMs() const { return timeoutMs_; }

        void setFilename(const std::string& filename) { filename_ = filename; }
        void setLocalPort(int port) { localPort_ = port; }
        void setRecvPort(int port) { recvPort_ = port; }
        void setMode(int mode) { mode_ = mode; }
        void setModeParameter(long param) { modeParameter_ = param; }
        void setTimeoutMs(long timeout) { timeoutMs_ = timeout; }

        void sendFile();

    private:
        std::string filename_;
        int localPort_;
        int recvPort_;
        int mode_;
        long modeParameter_;
        long timeoutMs_;

        int sendPacket_(SendPacketOptions& options);
        void waitForAck_(WaitForAckOptions& options);
};

#endif // SENDER_HPP