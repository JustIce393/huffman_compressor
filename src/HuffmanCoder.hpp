#pragma once

#include <string>
#include <unordered_map>
#include "HuffmanNode.hpp"

namespace huffman {

class HuffmanCoder {
public:
    // Compresses the input file and writes to the output file.
    // Returns true on success, false on failure.
    static bool compress(const std::string& inputPath, const std::string& outputPath);

    // Decompresses the input file and writes to the output file.
    // Returns true on success, false on failure.
    static bool decompress(const std::string& inputPath, const std::string& outputPath);

private:
    // Helper to build the Huffman tree from frequency map.
    // Caller takes ownership of the returned root node.
    static HuffmanNode* buildTree(const std::unordered_map<uint8_t, uint64_t>& frequencies);

    // Helper to generate codes by traversing the Huffman tree.
    static void generateCodes(const HuffmanNode* node, const std::string& currentPath,
                              std::unordered_map<uint8_t, std::string>& codes);
};

} // namespace huffman
