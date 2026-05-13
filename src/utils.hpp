#ifndef TATLIN_UTILS_HPP
#define TATLIN_UTILS_HPP

#include "file_tape.hpp"
#include <vector>
#include <string>

namespace tatlin {

bool writeVectorToTape(const std::string& path, const std::vector<int32_t>& data, const TapeConfig& cfg = TapeConfig());
std::vector<int32_t> readAllFromTape(const std::string& path, const TapeConfig& cfg = TapeConfig());
bool isSortedVector(const std::vector<int32_t>& v);
bool isTapeSorted(const std::string& path, const TapeConfig& cfg = TapeConfig());

} // namespace tatlin

#endif // TATLIN_UTILS_HPP
