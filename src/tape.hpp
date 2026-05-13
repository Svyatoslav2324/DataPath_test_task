#ifndef TATLIN_TAPE_HPP
#define TATLIN_TAPE_HPP

#include <cstdint>
#include <string>

namespace tatlin {

enum class Shift { LEFT, RIGHT };

class ITape {
public:
	virtual ~ITape() = default;
	// Read current cell into value. Returns false on EOF.
	virtual bool read(int32_t &value) = 0;
	// Write value to current cell (may extend tape).
	virtual void write(int32_t value) = 0;
	// Move tape by one cell under the head
	virtual void shift(Shift dir) = 0;
	// Rewind tape to start
	virtual void rewind() = 0;
	// True if current position is at or past end
	virtual bool isAtEnd() = 0;
	virtual int64_t getPosition() const = 0;
	virtual int64_t getLength() const = 0;
	virtual void close() = 0;
};

} // namespace tatlin

#endif // TATLIN_TAPE_HPP
