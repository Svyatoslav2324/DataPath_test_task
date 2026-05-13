
#ifndef TATLIN_FILE_TAPE_HPP
#define TATLIN_FILE_TAPE_HPP

#include "tape.hpp"
#include <fstream>
#include <string>
#include <istream>

namespace tatlin {

struct TapeConfig {
    int read_delay_ms = 0;
    int write_delay_ms = 0;
    int rewind_delay_ms = 0;
    int shift_delay_ms = 0;
};

class FileTape : public ITape {
public:
    enum class Mode { ReadOnly, ReadWrite, Create };

    FileTape(const std::string& path, Mode mode, const TapeConfig& cfg = TapeConfig());
    ~FileTape();

    bool read(int32_t &value) override;
    void write(int32_t value) override;
    void shift(Shift dir) override;
    void rewind() override;
    bool isAtEnd() override;
    int64_t getPosition() const override;
    int64_t getLength() const override;
    void close() override;

private:
    std::string path_;

    std::ifstream ifs_;
    std::ofstream ofs_;

    Mode mode_;
    TapeConfig cfg_;
    int64_t pos_ = 0; // current cell index
    int64_t length_ = 0; // total number of cells

    int64_t stream_read_index_ = 0;

    void loadLength();
    void sleep_ms(int ms);
};

} // namespace tatlin

#endif // TATLIN_FILE_TAPE_HPP

