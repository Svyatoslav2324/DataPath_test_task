#include <iostream>
#include <vector>
#include <random>
#include <cassert>
#include <filesystem>
#include "utils.hpp"
#include "file_tape.hpp"
#include "sorter.hpp"
#include "config.hpp"

using namespace tatlin;

#define ASSERT_TRUE(x) do { if (!(x)) { std::cerr << "ASSERT_TRUE failed: " #x << std::endl; return 1; } } while(0)
#define ASSERT_EQ(a,b) do { if (!((a) == (b))) { std::cerr << "ASSERT_EQ failed: " #a " != " #b << std::endl; return 1; } } while(0)

int test_file_tape_basic() {
    TapeConfig cfg;
    std::string path = "tmp/test_tape.bin";
    std::vector<int32_t> src = {5, 1, 3, 7};
    writeVectorToTape(path, src, cfg);
    auto out = readAllFromTape(path, cfg);
    ASSERT_EQ(out.size(), src.size());
    for (size_t i = 0; i < src.size(); ++i) ASSERT_EQ(out[i], src[i]);
    return 0;
}

int test_file_tape_text() {
    TapeConfig cfg;
    std::string path = "tmp/test_tape.txt";
    std::vector<int32_t> src = {5, 1, 3, 7};
    writeVectorToTape(path, src, cfg);
    auto out = readAllFromTape(path, cfg);
    ASSERT_EQ(out.size(), src.size());
    for (size_t i = 0; i < src.size(); ++i) ASSERT_EQ(out[i], src[i]);
    return 0;
}

int test_sort_small() {
    TapeConfig cfg;
    std::string inPath = "tmp/input.bin";
    std::string outPath = "tmp/output.bin";
    // generate random data
    std::mt19937 mt(123);
    std::uniform_int_distribution<int> dist(0, 1000);
    std::vector<int32_t> data(1000);
    for (auto &v : data) v = dist(mt);
    writeVectorToTape(inPath, data, cfg);

    FileTape input(inPath, FileTape::Mode::ReadOnly, cfg);
    FileTape output(outPath, FileTape::Mode::Create, cfg);
    TapeSorter sorter(&input, &output, 64 * 1024, cfg, "tmp");
    sorter.sort();
    input.close(); output.close();
    ASSERT_TRUE(isTapeSorted(outPath, cfg));
    FileTape outTape(outPath, FileTape::Mode::ReadOnly, cfg);
    ASSERT_EQ(static_cast<size_t>(outTape.getLength()), data.size());
    outTape.close();
    return 0;
}

int main() {
    std::filesystem::create_directories("tmp");
    std::cout << "Running tests..." << std::endl;
    if (test_file_tape_basic() != 0) return 1;
    std::cout << "test_file_tape_basic OK" << std::endl;
    if (test_file_tape_text() != 0) return 1;
    std::cout << "test_file_tape_text OK" << std::endl;
    if (test_sort_small() != 0) return 1;
    std::cout << "test_sort_small OK" << std::endl;
    std::cout << "All tests passed." << std::endl;
    return 0;
}
