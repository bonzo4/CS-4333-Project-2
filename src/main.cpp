#include <cstdlib>
#include <iostream>
#include <exception>

#include "./utils/utils.hpp"
#include "./handlers/handlers.hpp"

void run_app(int argc, char* argv[]) {
    Config config;
    parseArgs(argc, argv, config);

    switch (config.mode) {
        case AppMode::RECEIVER:
            handleReceiver(config);
            break;
        case AppMode::SENDER:
            handleSender(config);
            break;
        case AppMode::AUTO:
            handleAuto(config);
            break;
        case AppMode::HELP:
            handleHelp();
            break;
    }
}

int main(int argc, char* argv[]) {
    try {
        run_app(argc, argv);
    } 
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}