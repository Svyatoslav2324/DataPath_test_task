#ifndef TATLIN_SORTER_HPP
#define TATLIN_SORTER_HPP

#include "tape.hpp"
#include "file_tape.hpp"
#include <string>
#include <vector>

namespace tatlin {


class TapeSorter {
public:
    TapeSorter(ITape* input, ITape* output, size_t memoryLimitBytes, const TapeConfig& cfg, const std::string& tmpDir = "tmp");
    void sort();

private:
    ITape* input_;
    ITape* output_;
    size_t memoryLimitBytes_;
    TapeConfig tapeConfig_;
    std::string tmpDir_;
    std::vector<std::string> tempFiles_;

    void createRuns();
    void mergeRuns();
};

} // namespace tatlin

#endif // TATLIN_SORTER_HPP
