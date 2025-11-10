#ifndef RECEIVER_UTILS_HPP
#define RECEIVER_UTILS_HPP

#include <unordered_map>
#include <string>
#include <stdexcept>
#include <cstdint>

#define MAX_DATA_SIZE 64

enum class ARQMode {
    STOP_AND_WAIT,
    SLIDING_WINDOW,
};

struct Config {
    ARQMode arqType = ARQMode::STOP_AND_WAIT;
};

const std::unordered_map<std::string, ARQMode> arqModeMap = {
    {"stop-and-wait", ARQMode::STOP_AND_WAIT},
    {"sliding-window", ARQMode::SLIDING_WINDOW}
};

const std::unordered_map<std::string, std::string> shortArgMap = {
    {"-a", "--arq"},
};

inline void parseARQType(int argc, char* argv[], Config& config) {
    if (argc > 1 && std::string(argv[1]) == "-a") {
        std::string arg = argv[1];
        if (shortArgMap.count(arg)) { 
            arg = shortArgMap.at(arg); 
        }
        std::string arqTypeStr = argv[2];
        try {
            config.arqType = arqModeMap.at(arqTypeStr);
        } catch (const std::out_of_range&) {
            throw std::invalid_argument("Invalid ARQ mode: " + arqTypeStr);
        }
    }
}

#endif // RECEIVER_UTILS_HPP