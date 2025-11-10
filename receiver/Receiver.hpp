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
#include <map>
#include <vector>

enum PacketType {
    DATA = 0,
    ACK = 1
};

struct Packet {
    int packetIndex;
    int dataSize;
    PacketType type;
    bool isLast;
    char data[MAX_DATA_SIZE];
};

struct BufferedPacket {
    Packet packet;
    bool received;
};

class Receiver {
    public:
        Receiver() {};

        std::string getFilename() const { return filename_; }
        int getLocalPort() const { return localPort_; }

        void setFilename(const std::string& filename) { filename_ = filename; }
        void setLocalPort(int port) { localPort_ = port; }

        void receiveFile();

    private:
        std::string filename_;
        int localPort_;
        int senderPort_;
};

#endif // RECEIVER_HPP