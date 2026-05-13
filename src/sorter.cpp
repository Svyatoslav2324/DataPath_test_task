#include "sorter.hpp"
#include "file_tape.hpp"
#include <algorithm>
#include <queue>
#include <filesystem>
#include <cstdio>
#include <iostream>
#include <memory>

namespace tatlin {

TapeSorter::TapeSorter(ITape* input, ITape* output, size_t memoryLimitBytes, const TapeConfig& cfg, const std::string& tmpDir)
    : input_(input), output_(output), memoryLimitBytes_(memoryLimitBytes), tapeConfig_(cfg), tmpDir_(tmpDir) {
}

void TapeSorter::sort() {
    // create temp dir
    std::filesystem::create_directories(tmpDir_);
    createRuns();
    mergeRuns();
}

void TapeSorter::createRuns() {
    input_->rewind();
    std::vector<int32_t> buffer;
    size_t runSize = std::max<size_t>(1, memoryLimitBytes_ / sizeof(int32_t));
    int runIndex = 0;
    int32_t v;
    while (input_->read(v)) {
        buffer.push_back(v);
        input_->shift(Shift::RIGHT);
        if (buffer.size() >= runSize) {
            std::sort(buffer.begin(), buffer.end());
            std::string tmpPath = tmpDir_ + "/run_" + std::to_string(runIndex++) + ".tmp";
            {
                FileTape tmpTape(tmpPath, FileTape::Mode::Create, tapeConfig_);
                for (auto &x : buffer) {
                    tmpTape.write(x);
                    tmpTape.shift(Shift::RIGHT);
                }
                tmpTape.close();
            }
            tempFiles_.push_back(tmpPath);
            buffer.clear();
        }
    }
    if (!buffer.empty()) {
        std::sort(buffer.begin(), buffer.end());
        std::string tmpPath = tmpDir_ + "/run_" + std::to_string(runIndex++) + ".tmp";
        {
            FileTape tmpTape(tmpPath, FileTape::Mode::Create, tapeConfig_);
            for (auto &x : buffer) {
                tmpTape.write(x);
                tmpTape.shift(Shift::RIGHT);
            }
            tmpTape.close();
        }
        tempFiles_.push_back(tmpPath);
        buffer.clear();
    }
}

void TapeSorter::mergeRuns() {
    if (tempFiles_.empty()) {
        // nothing to merge
        return;
    }
    std::vector<std::string> curFiles = tempFiles_;

    struct Node { int32_t value; size_t idx; };

    // estimate how many runs we can merge at once based on memory limit
    size_t nodeSize = sizeof(Node);
    size_t maxRuns = 0;
    if (memoryLimitBytes_ > nodeSize * 4)
        maxRuns = memoryLimitBytes_ / (nodeSize * 4);
    if (maxRuns < 2) maxRuns = 2; // need at least 2-way merge

    size_t passNum = 0;
    while (curFiles.size() > 1) {
        std::vector<std::string> nextFiles;
        for (size_t i = 0; i < curFiles.size(); i += maxRuns) {
            size_t j = std::min(curFiles.size(), i + maxRuns);
            std::string outPath = tmpDir_ + "/merge_" + std::to_string(passNum) + "_" + std::to_string(i / maxRuns) + ".tmp";

            // open group runs
            std::vector<std::unique_ptr<FileTape>> runs;
            for (size_t k = i; k < j; ++k) {
                runs.emplace_back(std::make_unique<FileTape>(curFiles[k], FileTape::Mode::ReadOnly, tapeConfig_));
            }

            auto cmp = [](const Node &a, const Node &b) { return a.value > b.value; };
            std::priority_queue<Node, std::vector<Node>, decltype(cmp)> pq(cmp);

            for (size_t r = 0; r < runs.size(); ++r) {
                runs[r]->rewind();
                int32_t val;
                if (runs[r]->read(val)) {
                    pq.push({val, r});
                    runs[r]->shift(Shift::RIGHT);
                }
            }

            // merge group into outPath
            FileTape outTape(outPath, FileTape::Mode::Create, tapeConfig_);
            while (!pq.empty()) {
                Node n = pq.top(); pq.pop();
                outTape.write(n.value);
                outTape.shift(Shift::RIGHT);
                size_t ridx = n.idx;
                if (!runs[ridx]->isAtEnd()) {
                    int32_t nextv;
                    if (runs[ridx]->read(nextv)) {
                        pq.push({nextv, ridx});
                        runs[ridx]->shift(Shift::RIGHT);
                    }
                }
            }

            for (auto &r : runs) r->close();
            outTape.close();

            nextFiles.push_back(outPath);
        }

        // remove previous-level temp files
        for (auto &p : curFiles) {
            std::error_code ec;
            std::filesystem::remove(p, ec);
        }

        curFiles.swap(nextFiles);
        ++passNum;
    }

    // Now curFiles.size() == 1, stream it to the output tape
    output_->rewind();
    FileTape finalRun(curFiles[0], FileTape::Mode::ReadOnly, tapeConfig_);
    finalRun.rewind();
    int32_t v;
    while (finalRun.read(v)) {
        output_->write(v);
        output_->shift(Shift::RIGHT);
        finalRun.shift(Shift::RIGHT);
    }
    finalRun.close();

    // remove final temp file
    std::error_code ec;
    std::filesystem::remove(curFiles[0], ec);
    tempFiles_.clear();
}

} // namespace tatlin
