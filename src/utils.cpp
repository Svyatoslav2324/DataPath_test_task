#include "utils.hpp"
#include <filesystem>

namespace tatlin {

bool writeVectorToTape(const std::string& path, const std::vector<int32_t>& data, const TapeConfig& cfg) {
    std::filesystem::path p(path);
    if (p.has_parent_path()) std::filesystem::create_directories(p.parent_path());
    FileTape tape(path, FileTape::Mode::Create, cfg);
    for (auto v : data) {
        tape.write(v);
        tape.shift(Shift::RIGHT);
    }
    tape.close();
    return true;
}

std::vector<int32_t> readAllFromTape(const std::string& path, const TapeConfig& cfg) {
    std::vector<int32_t> out;
    if (!std::filesystem::exists(path)) return out;
    FileTape tape(path, FileTape::Mode::ReadOnly, cfg);
    tape.rewind();
    int32_t v;
    while (tape.read(v)) {
        out.push_back(v);
        tape.shift(Shift::RIGHT);
    }
    tape.close();
    return out;
}

bool isSortedVector(const std::vector<int32_t>& v) {
    for (size_t i = 1; i < v.size(); ++i) if (v[i-1] > v[i]) return false;
    return true;
}

bool isTapeSorted(const std::string& path, const TapeConfig& cfg) {
    if (!std::filesystem::exists(path)) return true;
    FileTape tape(path, FileTape::Mode::ReadOnly, cfg);
    tape.rewind();
    int32_t prev = 0;
    bool first = true;
    int32_t v;
    while (tape.read(v)) {
        if (!first) {
            if (prev > v) {
                tape.close();
                return false;
            }
        } else {
            first = false;
        }
        prev = v;
        tape.shift(Shift::RIGHT);
    }
    tape.close();
    return true;
}

} // namespace tatlin
