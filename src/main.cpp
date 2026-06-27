#include "HuffmanCoder.hpp"
#include <iostream>
#include <chrono>
#include <filesystem>
#include <iomanip>

namespace fs = std::filesystem;

void printUsage() {
    std::cout << "Huffman Coding Lossless File Compressor CLI\n\n";
    std::cout << "Usage:\n";
    std::cout << "  huffman_compressor compress <input_file> <output_file>\n";
    std::cout << "  huffman_compressor decompress <input_file> <output_file>\n\n";
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printUsage();
        return 1;
    }

    std::string mode = argv[1];
    std::string inputPath = argv[2];
    std::string outputPath = argv[3];

    if (mode != "compress" && mode != "decompress") {
        std::cerr << "Error: Unknown mode '" << mode << "'\n\n";
        printUsage();
        return 1;
    }

    if (!fs::exists(inputPath)) {
        std::cerr << "Error: Input file does not exist: " << inputPath << "\n";
        return 1;
    }

    try {
        auto startTime = std::chrono::high_resolution_clock::now();
        bool success = false;

        if (mode == "compress") {
            std::cout << "Compressing " << inputPath << " to " << outputPath << "...\n";
            success = huffman::HuffmanCoder::compress(inputPath, outputPath);
        } else {
            std::cout << "Decompressing " << inputPath << " to " << outputPath << "...\n";
            success = huffman::HuffmanCoder::decompress(inputPath, outputPath);
        }

        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = endTime - startTime;

        if (!success) {
            std::cerr << "Operation failed!\n";
            return 1;
        }

        std::cout << "Success!\n";
        std::cout << "Time elapsed: " << std::fixed << std::setprecision(3) << elapsed.count() << " seconds\n";

        if (mode == "compress") {
            uintmax_t inputSize = fs::file_size(inputPath);
            uintmax_t outputSize = fs::file_size(outputPath);
            std::cout << "Original Size:   " << inputSize << " bytes\n";
            std::cout << "Compressed Size: " << outputSize << " bytes\n";
            if (inputSize > 0) {
                double ratio = 100.0 * (1.0 - (double)outputSize / inputSize);
                std::cout << "Space Saved:     " << std::fixed << std::setprecision(2) << ratio << "%\n";
            }
        } else {
            uintmax_t inputSize = fs::file_size(inputPath);
            uintmax_t outputSize = fs::file_size(outputPath);
            std::cout << "Compressed Size: " << inputSize << " bytes\n";
            std::cout << "Restored Size:   " << outputSize << " bytes\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "An exception occurred: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
