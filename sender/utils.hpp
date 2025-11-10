#ifndef SENDER_UTILS_HPP
#define SENDER_UTILS_HPP

#define MAX_DATA_SIZE 64

#include <unordered_map>
#include <string>
#include <stdexcept>
#include <cstdint>

enum class ARQMode {
    STOP_AND_WAIT,
    SLIDING_WINDOW,
};

struct Config {
    ARQMode arqType = ARQMode::STOP_AND_WAIT;
    float dropRate = 0.0f;
    long modeParameter = 256;
    long mtu = MAX_DATA_SIZE;
};

const std::unordered_map<std::string, ARQMode> arqModeMap = {
    {"stop-and-wait", ARQMode::STOP_AND_WAIT},
    {"sliding-window", ARQMode::SLIDING_WINDOW}
};

const std::unordered_map<std::string, std::string> shortArgMap = {
    {"-a", "--arq"},
    {"-d", "--drop-rate"},
    {"-m", "--mtu"},
    {"-mp", "--mode-parameter"}
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
        } else if (arg == "--mode-parameter" && i + 1 < argc) {
            config.modeParameter = std::stoi(argv[++i]);
        } else if (arg == "--mtu" && i + 1 < argc) {
            config.mtu = std::stol(argv[++i]);
        } else {
            throw std::invalid_argument("Unknown argument: " + arg);
        }
    }
}

#endif // SENDER_UTILS_HPP