#pragma once

#include <cstdint>

namespace huffman {

struct HuffmanNode {
    uint8_t data;
    uint64_t frequency;
    uint32_t id;
    HuffmanNode* left;
    HuffmanNode* right;

    // Leaf node constructor
    HuffmanNode(uint8_t d, uint64_t f)
        : data(d), frequency(f), id(0), left(nullptr), right(nullptr) {}

    // Internal node constructor
    HuffmanNode(uint64_t f, HuffmanNode* l, HuffmanNode* r)
        : data(0), frequency(f), id(0), left(l), right(r) {}

    // Destructor to recursively clean up child nodes
    ~HuffmanNode() {
        delete left;
        delete right;
    }

    // Disable copy constructor and assignment operator to avoid double deletion
    HuffmanNode(const HuffmanNode&) = delete;
    HuffmanNode& operator=(const HuffmanNode&) = delete;

    bool isLeaf() const {
        return left == nullptr && right == nullptr;
    }
};

} // namespace huffman
