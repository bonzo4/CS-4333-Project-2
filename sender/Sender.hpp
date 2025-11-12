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
#include <unordered_map>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>

struct AckPacket {
    int packetIndex;
};

struct Packet {
    int packetIndex;
    int dataSize;
    bool isLast;
    long offset;
    char data[MAX_DATA_SIZE];
};


struct PacketInfo {
    Packet packet;
    bool hasBeenSent;
};

class Sender {
    public:
        Sender() {};

        std::string getFilename() const { return filename_; }
        int getLocalPort() const { return localPort_; }
        int getRecvPort() const { return recvPort_; }
        int getMode() const { return mode_; }
        long getModeParameter() const { return modeParameter_; }
        long getMTU() const { return mtu_; }
        long getTimeoutMs() const { return timeoutMs_; }
        float getDropRate() const { return dropRate_; }

        void setFilename(const std::string& filename) { filename_ = filename; }
        void setLocalPort(int port) { localPort_ = port; }
        void setRecvPort(int port) { recvPort_ = port; }
        void setMode(int mode) { mode_ = mode; }
        void setModeParameter(long param) { modeParameter_ = param; }
        void setMTU(long mtu) { mtu_ = mtu; }
        void setTimeoutMs(long timeout) { timeoutMs_ = timeout; }
        void setDropRate(float dropRate) { dropRate_ = dropRate; }

        void sendFile();

    private:
        std::string filename_;
        int localPort_;
        int recvPort_;
        int mode_;
        long modeParameter_;
        long mtu_;
        long timeoutMs_;
        float dropRate_;
};

#endif // SENDER_HPP