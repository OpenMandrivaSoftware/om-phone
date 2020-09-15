/* BitField class that, unlike std::bitset, works with
 * C style input (memset/memcpy/...), and therefore with
 * ioctl output without conversions
 *
 * (C) 2020 Bernhard Rosenkränzer <bero@lindev.ch>
 *
 * Released under the GPLv3
 */

#pragma once

#include <cstdint>
#include <cstring>

template<size_t s> class BitField {
public:
	BitField() { memset(data, 0, sizeof(data)); }
	operator void*() { return static_cast<void*>(&data); }
	bool isSet(int bit) const {
		return (data[(bit/8)]>>(bit%8))&1;
	}
	void set(int bit) {
		data[(bit/8)] |= 1<<(bit%8);
	}
	void clear(int bit) {
		data[(bit/8)] &= ~(1<<(bit%8));
	}
	bool operator[](int bit) const {
		return isSet(bit);
	}
private:
	uint8_t data[1+s/8];
};
