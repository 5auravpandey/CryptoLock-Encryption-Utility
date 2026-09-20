/* @author Saurav Pandey | CryptoLock Utility */

#include "CryptoEngine.h"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <cstdio>

std::vector<uint8_t> CryptoEngine::stretchKey(const std::string& key,
                                              size_t length) {
    if (key.empty() || length == 0)
        return std::vector<uint8_t>(length, 0);

    std::vector<uint8_t> stretched(length);
    for (size_t i = 0; i < length; ++i)
        stretched[i] = static_cast<uint8_t>(key[i % key.size()]);

    for (int round = 0; round < STRETCH_ROUNDS; ++round) {
        uint8_t feedback = static_cast<uint8_t>(round * 0x9E + 0x37);

        for (size_t i = 0; i < length; ++i) {
            uint8_t prev = stretched[(i + length - 1) % length];
            uint8_t salt = static_cast<uint8_t>((i * 0x6D) ^ (round * 0x1F));

            stretched[i] ^= ((prev << 3) | (prev >> 5)) ^ salt ^ feedback;
            feedback       = static_cast<uint8_t>(feedback ^ stretched[i]);
        }
    }

    return stretched;
}

bool CryptoEngine::encryptFile(const std::string& inputPath,
                               const std::string& outputPath,
                               const std::string& key) {
    try {
        if (key.size() < 4)
            throw std::runtime_error("Key must be at least 4 characters.");

        std::ifstream inFile(inputPath, std::ios::binary);
        if (!inFile.is_open())
            throw std::runtime_error("Cannot open input file: '" + inputPath + "'");

        std::ofstream outFile(outputPath, std::ios::binary);
        if (!outFile.is_open())
            throw std::runtime_error("Cannot open output file: '" + outputPath + "'");

        outFile.write(MAGIC, MAGIC_SIZE);
        if (!outFile)
            throw std::runtime_error("Failed to write header to: '" + outputPath + "'");

        std::vector<uint8_t> stretched = stretchKey(key, CHUNK_SIZE);
        std::vector<uint8_t> buffer(CHUNK_SIZE);
        size_t globalOffset = 0;

        while (inFile) {
            inFile.read(reinterpret_cast<char*>(buffer.data()), CHUNK_SIZE);
            std::streamsize bytesRead = inFile.gcount();
            if (bytesRead <= 0) break;

            for (std::streamsize i = 0; i < bytesRead; ++i)
                buffer[static_cast<size_t>(i)] ^=
                    stretched[(globalOffset + static_cast<size_t>(i)) % stretched.size()];

            globalOffset += static_cast<size_t>(bytesRead);
            outFile.write(reinterpret_cast<const char*>(buffer.data()), bytesRead);

            if (!outFile)
                throw std::runtime_error("Write error on: '" + outputPath + "'");
        }

        return true;

    } catch (const std::exception& e) {
        std::cerr << "  [encrypt] " << e.what() << "\n";
        std::remove(outputPath.c_str());
        return false;
    }
}

bool CryptoEngine::decryptFile(const std::string& inputPath,
                               const std::string& outputPath,
                               const std::string& key) {
    try {
        if (key.size() < 4)
            throw std::runtime_error("Key must be at least 4 characters.");

        std::ifstream inFile(inputPath, std::ios::binary);
        if (!inFile.is_open())
            throw std::runtime_error("Cannot open input file: '" + inputPath + "'");

        char header[MAGIC_SIZE];
        inFile.read(header, MAGIC_SIZE);

        if (static_cast<size_t>(inFile.gcount()) < MAGIC_SIZE ||
            std::memcmp(header, MAGIC, MAGIC_SIZE) != 0) {
            throw std::runtime_error(
                "Error: File is not a valid CryptoLock encrypted file.");
        }

        std::ofstream outFile(outputPath, std::ios::binary);
        if (!outFile.is_open())
            throw std::runtime_error("Cannot open output file: '" + outputPath + "'");

        std::vector<uint8_t> stretched = stretchKey(key, CHUNK_SIZE);
        std::vector<uint8_t> buffer(CHUNK_SIZE);
        size_t globalOffset = 0;

        while (inFile) {
            inFile.read(reinterpret_cast<char*>(buffer.data()), CHUNK_SIZE);
            std::streamsize bytesRead = inFile.gcount();
            if (bytesRead <= 0) break;

            for (std::streamsize i = 0; i < bytesRead; ++i)
                buffer[static_cast<size_t>(i)] ^=
                    stretched[(globalOffset + static_cast<size_t>(i)) % stretched.size()];

            globalOffset += static_cast<size_t>(bytesRead);
            outFile.write(reinterpret_cast<const char*>(buffer.data()), bytesRead);

            if (!outFile)
                throw std::runtime_error("Write error on: '" + outputPath + "'");
        }

        return true;

    } catch (const std::exception& e) {
        std::cerr << "  [decrypt] " << e.what() << "\n";
        std::remove(outputPath.c_str());
        return false;
    }
}
