#ifndef SENDER_HPP
#define SENDER_HPP

#include "../../utils/utils.hpp"

struct SenderConfig {
    int fd;
    int localPort;
    ARQMode arqMode;
    long arqParam;
    int recvSocketFd; 
    long timeoutMs;
};

class Sender {
    public:
        Sender(const SenderConfig& config) 
            : fd_(config.fd), 
              localPort_(config.localPort), 
              arqMode_(config.arqMode), 
              arqParam_(config.arqParam), 
              recvSocketFd_(config.recvSocketFd), 
              timeoutMs_(config.timeoutMs) 
        {};

        int getFd() const { return fd_; }
        int getLocalPort() const { return localPort_; }
        ARQMode getARQMode() const { return arqMode_; }
        long getARQParam() const { return arqParam_; }
        int getRecvSocketFd() const { return recvSocketFd_; }
        long getTimeoutMs() const { return timeoutMs_; }
        
        void setFd(int fd) { fd_ = fd; }
        void setLocalPort(int port) { localPort_ = port; }
        void setARQMode(ARQMode mode) { arqMode_ = mode; }
        void setARQParam(long param) { arqParam_ = param; }
        void setRecvSocketFd(int socketFd) { recvSocketFd_= socketFd; }
        void setTimeoutMs(long timeout) { timeoutMs_ = timeout; }

        void sendFile();

    private:
        int fd_;
        int localPort_;
        ARQMode arqMode_;
        long arqParam_;
        int recvSocketFd_;
        long timeoutMs_;
};

#endif // SENDER_HPP