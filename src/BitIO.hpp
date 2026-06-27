#pragma once

#include <iostream>
#include <cstdint>
#include <string>

namespace huffman {

class BitWriter {
private:
    std::ostream& out;
    uint8_t buffer;
    int bitCount;

public:
    explicit BitWriter(std::ostream& os) : out(os), buffer(0), bitCount(0) {}

    ~BitWriter() {
        flush();
    }

    void writeBit(bool bit) {
        buffer = (buffer << 1) | (bit ? 1 : 0);
        bitCount++;
        if (bitCount == 8) {
            out.put(static_cast<char>(buffer));
            buffer = 0;
            bitCount = 0;
        }
    }

    void writeBits(const std::string& bits) {
        for (char c : bits) {
            writeBit(c == '1');
        }
    }

    void flush() {
        if (bitCount > 0) {
            buffer <<= (8 - bitCount);
            out.put(static_cast<char>(buffer));
            buffer = 0;
            bitCount = 0;
        }
    }
};

class BitReader {
private:
    std::istream& in;
    uint8_t buffer;
    int bitCount;

public:
    explicit BitReader(std::istream& is) : in(is), buffer(0), bitCount(0) {}

    bool readBit(bool& bit) {
        if (bitCount == 0) {
            int val = in.get();
            if (val == EOF) {
                return false;
            }
            buffer = static_cast<uint8_t>(val);
            bitCount = 8;
        }
        bit = (buffer & 0x80) != 0;
        buffer <<= 1;
        bitCount--;
        return true;
    }
};

} // namespace huffman
