#include <iostream>
#include <string>
#include "config.hpp"
#include "file_tape.hpp"
#include "sorter.hpp"

using namespace tatlin;

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: sorter <input-file> <output-file>\n";
        return 1;
    }
    std::string inputPath = argv[1];
    std::string outputPath = argv[2];
    size_t memoryLimit = 0;
    std::string configPath = "config.txt";

    Config cfg = parseConfigFile(configPath);
    if (memoryLimit == 0) memoryLimit = cfg.memoryLimitBytes;

    try {
        FileTape inputTape(inputPath, FileTape::Mode::ReadOnly, cfg.tapeConfig);
        FileTape outputTape(outputPath, FileTape::Mode::Create, cfg.tapeConfig);

        TapeSorter sorter(&inputTape, &outputTape, memoryLimit, cfg.tapeConfig, "tmp");
        sorter.sort();

        inputTape.close();
        outputTape.close();

        std::cout << "Sorting finished." << std::endl;
    } catch (const std::exception &ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 2;
    }
    return 0;
}
