#ifndef TATLIN_CONFIG_HPP
#define TATLIN_CONFIG_HPP

#include "file_tape.hpp"
#include <string>

namespace tatlin {

struct Config {
    TapeConfig tapeConfig;
    size_t memoryLimitBytes = 8 * 1024 * 1024; // default 8MB
};

Config parseConfigFile(const std::string& path);

} // namespace tatlin

#endif // TATLIN_CONFIG_HPP
