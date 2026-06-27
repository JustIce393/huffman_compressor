#include "HuffmanCoder.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <algorithm>

namespace huffman {

namespace {

struct DictEntry {
    uint16_t parent;
    uint8_t character;
};

} // namespace

bool HuffmanCoder::compress(const std::string& inputPath, const std::string& outputPath) {
    std::ifstream in(inputPath, std::ios::binary);
    if (!in) {
        std::cerr << "Error: Could not open input file " << inputPath << std::endl;
        return false;
    }

    // Read entire file into buffer
    std::vector<uint8_t> inputBytes((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    in.close();

    std::ofstream out(outputPath, std::ios::binary);
    if (!out) {
        std::cerr << "Error: Could not open output file " << outputPath << std::endl;
        return false;
    }

    // Write LZW Header: Magic "LZW" + Version 1
    out.put('L');
    out.put('Z');
    out.put('W');
    out.put(0x01);

    if (inputBytes.empty()) {
        return true;
    }

    // Dictionary mapping (parent_code << 8 | byte) -> code
    std::unordered_map<uint32_t, uint16_t> dict;
    
    uint16_t nextCode = 257; // 256 is RESET_CODE
    const uint16_t RESET_CODE = 256;
    const uint32_t MAX_CODES = 65536;

    uint16_t currentCode = inputBytes[0];

    for (size_t i = 1; i < inputBytes.size(); ++i) {
        uint8_t nextChar = inputBytes[i];
        uint32_t key = (static_cast<uint32_t>(currentCode) << 8) | nextChar;

        auto it = dict.find(key);
        if (it != dict.end()) {
            currentCode = it->second;
        } else {
            // Output currentCode (2 bytes, little-endian)
            out.put(static_cast<char>(currentCode & 0xFF));
            out.put(static_cast<char>((currentCode >> 8) & 0xFF));

            // Add to dictionary
            if (nextCode < MAX_CODES) {
                dict[key] = nextCode++;
            }

            // Check if dictionary is full, reset if so
            if (nextCode >= MAX_CODES) {
                // Output RESET_CODE
                out.put(static_cast<char>(RESET_CODE & 0xFF));
                out.put(static_cast<char>((RESET_CODE >> 8) & 0xFF));

                dict.clear();
                nextCode = 257;
            }

            currentCode = nextChar;
        }
    }

    // Output final code
    out.put(static_cast<char>(currentCode & 0xFF));
    out.put(static_cast<char>((currentCode >> 8) & 0xFF));

    return true;
}

bool HuffmanCoder::decompress(const std::string& inputPath, const std::string& outputPath) {
    std::ifstream in(inputPath, std::ios::binary);
    if (!in) {
        std::cerr << "Error: Could not open compressed file " << inputPath << std::endl;
        return false;
    }

    // Verify LZW Header
    char magic[3];
    in.read(magic, 3);
    int version = in.get();
    if (!in || magic[0] != 'L' || magic[1] != 'Z' || magic[2] != 'W' || version != 0x01) {
        std::cerr << "Error: Invalid LZW compressed file format." << std::endl;
        return false;
    }

    std::ofstream out(outputPath, std::ios::binary);
    if (!out) {
        std::cerr << "Error: Could not open output file " << outputPath << std::endl;
        return false;
    }

    // Dictionary size 65536, mapping code -> prefix+char
    std::vector<DictEntry> dict(65536);
    uint16_t nextCode = 257;
    const uint16_t RESET_CODE = 256;
    const uint32_t MAX_CODES = 65536;

    auto read_code = [&]() -> int {
        int byte1 = in.get();
        if (byte1 == EOF) return -1;
        int byte2 = in.get();
        if (byte2 == EOF) return -1;
        return byte1 | (byte2 << 8);
    };

    int firstCode = read_code();
    if (firstCode == -1) {
        return true; // Empty file
    }

    uint16_t oldCode = static_cast<uint16_t>(firstCode);
    uint8_t finChar = static_cast<uint8_t>(firstCode);
    out.put(static_cast<char>(finChar));

    auto get_string = [&](uint16_t code, std::vector<uint8_t>& str) {
        str.clear();
        while (code >= 257) {
            str.push_back(dict[code].character);
            code = dict[code].parent;
        }
        str.push_back(static_cast<uint8_t>(code));
        std::reverse(str.begin(), str.end());
    };

    std::vector<uint8_t> strBytes;
    int codeVal;

    while ((codeVal = read_code()) != -1) {
        uint16_t code = static_cast<uint16_t>(codeVal);

        if (code == RESET_CODE) {
            nextCode = 257;
            codeVal = read_code();
            if (codeVal == -1) break;
            code = static_cast<uint16_t>(codeVal);
            oldCode = code;
            finChar = static_cast<uint8_t>(code);
            out.put(static_cast<char>(finChar));
            continue;
        }

        bool specialCase = (code >= nextCode);

        if (specialCase) {
            get_string(oldCode, strBytes);
            strBytes.push_back(finChar);
        } else {
            get_string(code, strBytes);
        }

        // Output decoded string
        out.write(reinterpret_cast<const char*>(strBytes.data()), strBytes.size());

        finChar = strBytes[0];

        // Add to dictionary
        if (nextCode < MAX_CODES) {
            dict[nextCode].parent = oldCode;
            dict[nextCode].character = finChar;
            nextCode++;
        }

        oldCode = code;
    }

    return true;
}

// Keep stubs for buildTree and generateCodes to avoid compilation issues if declared in header
HuffmanNode* HuffmanCoder::buildTree(const std::unordered_map<uint8_t, uint64_t>&) {
    return nullptr;
}

void HuffmanCoder::generateCodes(const HuffmanNode*, const std::string&, std::unordered_map<uint8_t, std::string>&) {
}

} // namespace huffman
