#include "utils.hpp"
#include "Receiver.hpp"

int main (int argc, char* argv[]) {
    
    Config config;
    parseARQType(argc, argv, config);

    Receiver receiver;

    receiver.setFilename("received_manuscript.txt");
    receiver.setLocalPort(8081);

    receiver.receiveFile();

    return 0;
}

