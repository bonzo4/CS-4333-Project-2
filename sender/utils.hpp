#ifndef SENDER_UTILS_HPP
#define SENDER_UTILS_HPP

#include <unordered_map>
#include <string>
#include <stdexcept>
#include <cstdint>

#define MAX_DATA_SIZE 512

enum class ARQMode {
    STOP_AND_WAIT,
    SLIDING_WINDOW,
};

enum PacketType : uint8_t {
    DATA = 0,
    ACK = 1,
};

struct Packet {
    uint32_t packetIndex;
    uint16_t dataSize;
    PacketType type;
    uint8_t isLast;
    char data[MAX_DATA_SIZE];
};

struct Config {
    ARQMode arqType = ARQMode::STOP_AND_WAIT;
    float dropRate = 0.0f;
    int delayMinMs = 10;
    int delayMaxMs = 100;
    int mtu = 0;
};

const std::unordered_map<std::string, ARQMode> arqModeMap = {
    {"stop-and-wait", ARQMode::STOP_AND_WAIT},
    {"sliding-window", ARQMode::SLIDING_WINDOW}
};

const std::unordered_map<std::string, std::string> shortArgMap = {
    {"-a", "--arq"},
    {"-d", "--drop-rate"},
    {"-dmin", "--delay-min"},
    {"-dmax", "--delay-max"},
    {"-m", "--mtu"}
};

inline void parseArgs(int argc, char* argv[], Config& config) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (shortArgMap.count(arg)) { 
            arg = shortArgMap.at(arg); 
        }

        if (arg == "--arq" && i + 1 < argc) {
            std::string arqTypeStr = argv[++i];
            try {
                config.arqType = arqModeMap.at(arqTypeStr);
            } catch (const std::out_of_range&) {
                throw std::invalid_argument("Invalid ARQ mode: " + arqTypeStr);
            }
        } else if (arg == "--drop-rate" && i + 1 < argc) {
            config.dropRate = std::stof(argv[++i]);
        } else if (arg == "--delay-min" && i + 1 < argc) {
            config.delayMinMs = std::stoi(argv[++i]);
        } else if (arg == "--delay-max" && i + 1 < argc) {
            config.delayMaxMs = std::stoi(argv[++i]);
        } else if (arg == "--mtu" && i + 1 < argc) {
            config.mtu = std::stoi(argv[++i]);
        } else {
            throw std::invalid_argument("Unknown argument: " + arg);
        }
    }
}

#endif // SENDER_UTILS_HPP