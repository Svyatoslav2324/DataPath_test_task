#include "file_tape.hpp"
#include <filesystem>
#include <thread>
#include <stdexcept>
#include <sstream>

namespace tatlin {

FileTape::FileTape(const std::string &path, Mode mode, const TapeConfig &cfg)
    : path_(path), mode_(mode), cfg_(cfg), pos_(0), length_(0), stream_read_index_(0) {
    using std::ios;
    std::filesystem::path p(path_);

    if (mode_ == Mode::Create) {
        if (p.has_parent_path()) std::filesystem::create_directories(p.parent_path());
        ofs_.open(path_, ios::out | ios::trunc);
        if (!ofs_) throw std::runtime_error("Failed to open tape file for writing: " + path_);
        length_ = 0;
    } else if (mode_ == Mode::ReadOnly) {
        if (!std::filesystem::exists(p)) throw std::runtime_error("tape file for reading does not exists: " + path_);
        ifs_.open(path_, ios::in);
        if (!ifs_) throw std::runtime_error("Failed to open tape file for reading: " + path_);
        loadLength();
        ifs_.clear(); ifs_.seekg(0);
        stream_read_index_ = 0;
    } else { // ReadWrite
        if (p.has_parent_path()) std::filesystem::create_directories(p.parent_path());
        ofs_.open(path_, ios::out | ios::app);
        ifs_.open(path_, ios::in);
        if (!ofs_ && !ifs_) throw std::runtime_error("Failed to open tape file: " + path_);
        loadLength();
        ifs_.clear(); ifs_.seekg(0);
        stream_read_index_ = 0;
    }
}

FileTape::~FileTape() {
    if (ifs_.is_open()) ifs_.close();
    if (ofs_.is_open()) ofs_.close();
}

void FileTape::loadLength() {
    length_ = 0;
    std::ifstream in(path_);
    if (!in) { length_ = 0; return; }
    std::string line;
    while (std::getline(in, line)) ++length_;
}

void FileTape::sleep_ms(int ms) {
    if (ms > 0) std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

bool FileTape::read(int32_t &value) {
    value = 0;
    if (pos_ >= length_) return false;
    if (!ifs_.is_open()) { ifs_.open(path_, std::ios::in); if (!ifs_) return false; }
    if (stream_read_index_ > pos_) { ifs_.clear(); ifs_.seekg(0); stream_read_index_ = 0; }
    std::string line;
    while (stream_read_index_ <= pos_) {
        if (!std::getline(ifs_, line)) return false;
        if (stream_read_index_ == pos_) {
            std::istringstream iss(line);
            iss >> value;
            ++stream_read_index_;
            sleep_ms(cfg_.read_delay_ms);
            return true;
        }
        ++stream_read_index_;
    }
    return false;
}

void FileTape::write(int32_t value) {
    if (!ofs_.is_open()) { ofs_.open(path_, std::ios::out | std::ios::app); if (!ofs_) throw std::runtime_error("Failed to open tape for writing: " + path_); }
    ofs_ << value << '\n'; ofs_.flush();
    if (pos_ >= length_) length_ = pos_ + 1;
    sleep_ms(cfg_.write_delay_ms);
}

void FileTape::shift(Shift dir) {
    if (dir == Shift::RIGHT) ++pos_; else if (pos_ > 0) --pos_;
    sleep_ms(cfg_.shift_delay_ms);
}

void FileTape::rewind() {
    pos_ = 0;
    if (ifs_.is_open()) { ifs_.clear(); ifs_.seekg(0); }
    stream_read_index_ = 0;
    sleep_ms(cfg_.rewind_delay_ms);
}

bool FileTape::isAtEnd() { return pos_ >= length_; }

int64_t FileTape::getPosition() const { return pos_; }
int64_t FileTape::getLength() const { return length_; }

void FileTape::close() {
    if (ifs_.is_open()) ifs_.close();
    if (ofs_.is_open()) ofs_.close();
}

} // namespace tatlin
