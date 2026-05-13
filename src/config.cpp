#include "config.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

namespace tatlin {

static inline std::string trim(const std::string &s) {
    std::size_t left = s.find_first_not_of(" \t\r\n");
    if (left == std::string::npos) return std::string();
    std::size_t right = s.find_last_not_of(" \t\r\n");
    return s.substr(left, right - left + 1);
}

Config parseConfigFile(const std::string& path) {
    Config cfg;
    std::ifstream in(path);
    if (!in) {
        std::cout << "uses default config\n"; 
        return cfg;
    }
    std::string line;
    while (std::getline(in, line)) {
        auto posc = line.find('#');
        if (posc != std::string::npos) line = line.substr(0, posc);
        auto eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string key = trim(line.substr(0, eq));
        std::string val = trim(line.substr(eq + 1));
        if (key.empty() || val.empty()) continue;
        try {
            if (key == "read_delay_ms") cfg.tapeConfig.read_delay_ms = std::stoi(val);
            else if (key == "write_delay_ms") cfg.tapeConfig.write_delay_ms = std::stoi(val);
            else if (key == "rewind_delay_ms") cfg.tapeConfig.rewind_delay_ms = std::stoi(val);
            else if (key == "shift_delay_ms") cfg.tapeConfig.shift_delay_ms = std::stoi(val);
            else if (key == "memory_limit_bytes") cfg.memoryLimitBytes = std::stoull(val);
        } catch(...) {
        }
    }
    return cfg;
}

} // namespace tatlin
