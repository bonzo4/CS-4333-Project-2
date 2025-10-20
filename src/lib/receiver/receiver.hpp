#ifndef RECEIVER_HPP
#define RECEIVER_HPP

#include "../../utils/utils.hpp"

struct ReceiverConfig {
    int fd;
    int localPort;
    ARQMode arqMode;
    long arqParam;
};

class Recevier {
    public:
        Recevier(const ReceiverConfig& config) 
            : fd_(config.fd), 
              localPort_(config.localPort), 
              arqMode_(config.arqMode), 
              arqParam_(config.arqParam) 
        {};

        int getFd() const { return fd_; }
        int getLocalPort() const { return localPort_; }
        ARQMode getARQMode() const { return arqMode_; }
        long getARQParam() const { return arqParam_; }

        void setFd(int fd) { fd_ = fd; }
        void setLocalPort(int port) { localPort_ = port; }
        void setARQMode(ARQMode mode) { arqMode_ = mode; }
        void setARQParam(long param) { arqParam_ = param; }

        void receiveFile();

    private:
        int fd_;
        int localPort_;
        ARQMode arqMode_;
        long arqParam_;
};

#endif // RECEIVER_HPP