#ifndef RECEIVER_HPP
#define RECEIVER_HPP

#include <string>

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
};

#endif // RECEIVER_HPP