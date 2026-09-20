/* @author Saurav Pandey | CryptoLock Utility */

#ifndef CRYPTOENGINE_H
#define CRYPTOENGINE_H

#include <string>
#include <vector>
#include <cstdint>

class CryptoEngine {
public:
    bool encryptFile(const std::string& inputPath,
                     const std::string& outputPath,
                     const std::string& key);

    bool decryptFile(const std::string& inputPath,
                     const std::string& outputPath,
                     const std::string& key);

private:
    static constexpr char     MAGIC[4]       = { 'C', 'L', 'C', 'K' };
    static constexpr size_t   MAGIC_SIZE     = sizeof(MAGIC);
    static constexpr size_t   CHUNK_SIZE     = 4096;
    static constexpr int      STRETCH_ROUNDS = 64;

    std::vector<uint8_t> stretchKey(const std::string& key, size_t length);
};

#endif
