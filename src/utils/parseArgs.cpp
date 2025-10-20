#include "utils.hpp"

void parseArgs(int argc, char* argv[], Config& config) {
   
    if (argc < 2 || std::string(argv[1]) == "help" || argv[1][0] == '-') {
        config.mode = AppMode::HELP;
        return;
    }

    std::string mode = std::string(argv[1]);

    if (shortArgMap.count(mode)) { mode = shortArgMap.at(mode); }

    try {
        config.mode = appModeMap.at(mode);
    } catch (const std::out_of_range&) {
        throw std::invalid_argument("Invalid mode: " + mode);
    }

    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (shortArgMap.count(arg)) { arg = shortArgMap.at(arg); }

        if (arg == "--arq" && i + 1 < argc) {
            std::string arqTypeStr = argv[++i];
            try {
                config.arqType = arqModeMap.at(arqTypeStr);
            } catch (const std::out_of_range&) {
                throw std::invalid_argument("Invalid ARQ mode: " + arqTypeStr);
            }
        } else if (arg == "--drop-rate" && i + 1 < argc) {
            parseValue(arg, argv[++i], config.dropRate);
        } else if (arg == "--delay-min" && i + 1 < argc) {
            parseValue(arg, argv[++i], config.delayMinMs);
        } else if (arg == "--delay-max" && i + 1 < argc) {
            parseValue(arg, argv[++i], config.delayMaxMs);
        } else if (arg == "--mtu" && i + 1 < argc) {
            parseValue(arg, argv[++i], config.mtu);
        } else if (arg == "--help") {
            config.mode = AppMode::HELP;
            return;
        } else {
            throw std::invalid_argument("Unknown argument: " + arg);
        }
    }
}