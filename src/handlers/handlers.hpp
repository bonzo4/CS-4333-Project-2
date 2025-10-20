#ifndef HANLDERS_HPP
#define HANLDERS_HPP

#include "../lib/receiver/receiver.hpp"
#include "../lib/sender/sender.hpp"

using namespace std;

void handleReceiver(const Config& config);
void handleSender(const Config& config);
void handleAuto(const Config& config);
void handleHelp();

#endif // HANLDERS_HPP