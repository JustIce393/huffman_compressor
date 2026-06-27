cmake_minimum_required(VERSION 3.12)
project(huffman_compressor CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(huffman_compressor
    src/main.cpp
    src/HuffmanCoder.cpp
    src/HuffmanCoder.hpp
    src/HuffmanNode.hpp
    src/BitIO.hpp
)

target_include_directories(huffman_compressor PRIVATE src)
