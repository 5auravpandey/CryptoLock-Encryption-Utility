/* @author Saurav Pandey | CryptoLock Utility */

#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>

#include "CryptoEngine.h"

namespace clr {
    const char* B   = "\033[1m";
    const char* R   = "\033[31m";
    const char* G   = "\033[32m";
    const char* C   = "\033[36m";
    const char* DIM = "\033[2m";
    const char* RST = "\033[0m";
}

static void printBanner() {
    std::cout
        << clr::C << clr::B
        << "\n"
        << "    ================================================\n"
        << "    |                                              |\n"
        << "    |        C R Y P T O   L O C K   v1.0         |\n"
        << "    |          XOR Cipher Utility                  |\n"
        << "    |                                              |\n"
        << "    ================================================\n"
        << clr::RST << "\n";
}

static void printUsage(const char* prog) {
    printBanner();
    std::cout
        << clr::B << "USAGE" << clr::RST << "\n"
        << "  " << prog << " -e <input_file> <key>   "
        << clr::DIM << "Encrypt a file" << clr::RST << "\n"
        << "  " << prog << " -d <input_file> <key>   "
        << clr::DIM << "Decrypt a file" << clr::RST << "\n"
        << "  " << prog << " -h                      "
        << clr::DIM << "Show this help" << clr::RST << "\n\n"
        << clr::B << "EXAMPLES" << clr::RST << "\n"
        << "  " << prog << " -e report.pdf S3cur3!Key\n"
        << "  " << clr::DIM << "  -> report.pdf.enc" << clr::RST << "\n\n"
        << "  " << prog << " -d report.pdf.enc S3cur3!Key\n"
        << "  " << clr::DIM << "  -> report.pdf.enc.dec" << clr::RST << "\n\n"
        << clr::B << "NOTES" << clr::RST << "\n"
        << "  - Key must be at least 4 characters.\n"
        << "  - Encrypted files carry a 'CLCK' magic header for validation.\n"
        << "  - Educational XOR cipher; not for production security.\n\n";
}

static void printError(const std::string& msg) {
    std::cerr << clr::R << clr::B << "[ERROR] "
              << clr::RST << clr::R << msg << clr::RST << "\n";
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    std::string flag = argv[1];

    if (flag == "-h" || flag == "--help") {
        printUsage(argv[0]);
        return 0;
    }

    if (flag != "-e" && flag != "-d") {
        printError("Unknown option '" + flag + "'.");
        std::cerr << "Run  " << argv[0] << " -h  for usage.\n";
        return 1;
    }

    if (argc < 4) {
        printError("Missing arguments. Both <input_file> and <key> are required.");
        std::cerr << "Run  " << argv[0] << " -h  for usage.\n";
        return 1;
    }

    if (argc > 4) {
        printError("Too many arguments.");
        std::cerr << "Run  " << argv[0] << " -h  for usage.\n";
        return 1;
    }

    std::string inputPath = argv[2];
    std::string key       = argv[3];

    if (key.length() < 4) {
        printError("Key is too short. Please use at least 4 characters.");
        return 1;
    }

    bool encrypting        = (flag == "-e");
    std::string outputPath = inputPath + (encrypting ? ".enc" : ".dec");

    if (inputPath == outputPath) {
        printError("Input and output paths must be different.");
        return 1;
    }

    std::string actionWord = encrypting ? "Encrypted" : "Decrypted";
    CryptoEngine engine;

    std::cout << clr::B
              << (encrypting ? "[*] Encrypting" : "[*] Decrypting")
              << clr::RST
              << " '" << inputPath << "' ...\n";

    auto t0 = std::chrono::high_resolution_clock::now();

    bool success = encrypting
        ? engine.encryptFile(inputPath, outputPath, key)
        : engine.decryptFile(inputPath, outputPath, key);

    auto t1 = std::chrono::high_resolution_clock::now();

    if (!success) {
        printError("Operation failed. Check the error above.");
        return 1;
    }

    double elapsedMs = std::chrono::duration<double, std::milli>(t1 - t0).count();

    std::cout
        << "\n"
        << clr::G << clr::B
        << "  --- Operation Complete ---\n"
        << clr::RST
        << "    Action  : " << clr::B << actionWord << clr::RST << "\n"
        << "    Output  : " << clr::B << outputPath << clr::RST << "\n"
        << "    Elapsed : " << clr::B
        << std::fixed << std::setprecision(2) << elapsedMs << " ms"
        << clr::RST << "\n\n";

    return 0;
}
