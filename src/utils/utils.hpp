#ifndef UTILS_HPP
#define UTILS_HPP

#include <unordered_map>
#include <string>
#include <stdexcept>
#include <type_traits>

// Helper for static_assert in templates
template<typename T>
struct always_false : std::false_type {};

enum class AppMode {
    RECEIVER,
    SENDER,
    AUTO,
    HELP
};

enum class ARQMode {
    STOP_AND_WAIT,
    SLIDING_WINDOW,
};

struct Config {
    AppMode mode = AppMode::HELP;
    ARQMode arqType = ARQMode::STOP_AND_WAIT;
    float dropRate = 0.0f;
    int delayMinMs = 10;
    int delayMaxMs = 100;
    int mtu = -1;
};

const std::unordered_map<std::string, AppMode> appModeMap = {
    {"recveiver", AppMode::RECEIVER},
    {"sender", AppMode::SENDER},
    {"auto", AppMode::AUTO},
    {"help", AppMode::HELP}
};

const std::unordered_map<std::string, ARQMode> arqModeMap = {
    {"stop-and-wait", ARQMode::STOP_AND_WAIT},
    {"sliding-window", ARQMode::SLIDING_WINDOW}
};

const std::unordered_map<std::string, std::string> shortArgMap = {
    {"r", "receiver"},
    {"s", "sender"},
    {"a", "auto"},
    {"h", "help"},
    {"-a", "--arq"},
    {"-d", "--drop-rate"},
    {"-dmin", "--delay-min"},
    {"-dmax", "--delay-max"},
    {"-m", "--mtu"},
    {"-h", "--help"}
};

void parseArgs(int argc, char* argv[], Config& config);

inline void parseValue(const std::string& argName, const std::string& valueStr, auto& outValue) {
    using T = std::decay_t<decltype(outValue)>;
    try {
        if constexpr (std::is_same_v<T, float>) {
            outValue = std::stof(valueStr);
        } else if constexpr (std::is_same_v<T, int>) {
            outValue = std::stoi(valueStr);
        } else {
            static_assert(always_false<T>::value, "Unsupported type");
        }
    } catch (const std::invalid_argument&) {
        throw std::invalid_argument("Invalid value for " + argName + ": '" + valueStr + "'");
    } catch (const std::out_of_range&) {
        throw std::out_of_range("Value for " + argName + " is out of range: '" + valueStr + "'");
    }
}

#endif // UTILS_HPP