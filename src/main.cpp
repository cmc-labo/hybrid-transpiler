#include "transpiler.h"
#include <iostream>
#include <string>
#include <vector>

void printUsage(const char* program_name) {
    std::cout << "Hybrid Transpiler - C++ to Modern Languages\n";
    std::cout << "Usage: " << program_name << " [options]\n\n";
    std::cout << "Options:\n";
    std::cout << "  -i, --input <file>      Input C++ source file\n";
    std::cout << "  -o, --output <file>     Output file path\n";
    std::cout << "  -t, --target <lang>     Target language (rust|go) [default: rust]\n";
    std::cout << "  -O, --opt-level <N>     Optimization level 0-3 [default: 0]\n";
    std::cout << "  --no-safety-checks      Disable safety checks\n";
    std::cout << "  --no-comments           Don't preserve comments\n";
    std::cout << "  --gen-tests             Generate test cases\n";
    std::cout << "  -h, --help              Show this help message\n";
    std::cout << "  -v, --version           Show version information\n";
    std::cout << "\nExamples:\n";
    std::cout << "  " << program_name << " -i main.cpp -o main.rs -t rust\n";
    std::cout << "  " << program_name << " -i class.cpp -o class.go -t go -O2\n";
}

void printVersion() {
    std::cout << "Hybrid Transpiler v0.1.0\n";
    std::cout << "Built with LLVM/Clang\n";
    std::cout << "Copyright (c) 2025\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    hybrid::TranspilerOptions options;
    std::string input_file;
    std::vector<std::string> input_files;

    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-v" || arg == "--version") {
            printVersion();
            return 0;
        } else if (arg == "-i" || arg == "--input") {
            if (i + 1 < argc) {
                input_file = argv[++i];
            } else {
                std::cerr << "Error: --input requires a file path\n";
                return 1;
            }
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                options.output_path = argv[++i];
            } else {
                std::cerr << "Error: --output requires a file path\n";
                return 1;
            }
        } else if (arg == "-t" || arg == "--target") {
            if (i + 1 < argc) {
                std::string target = argv[++i];
                if (target == "rust") {
                    options.target = hybrid::TargetLanguage::Rust;
                } else if (target == "go") {
                    options.target = hybrid::TargetLanguage::Go;
                } else {
                    std::cerr << "Error: Unknown target language: " << target << "\n";
                    return 1;
                }
            } else {
                std::cerr << "Error: --target requires a language (rust|go)\n";
                return 1;
            }
        } else if (arg == "-O" || arg == "--opt-level") {
            if (i + 1 < argc) {
                options.optimization_level = std::stoi(argv[++i]);
                if (options.optimization_level < 0 || options.optimization_level > 3) {
                    std::cerr << "Error: Optimization level must be 0-3\n";
                    return 1;
                }
            } else {
                std::cerr << "Error: --opt-level requires a number\n";
                return 1;
            }
        } else if (arg == "--no-safety-checks") {
            options.enable_safety_checks = false;
        } else if (arg == "--no-comments") {
            options.preserve_comments = false;
        } else if (arg == "--gen-tests") {
            options.generate_tests = true;
        } else {
            std::cerr << "Error: Unknown option: " << arg << "\n";
            printUsage(argv[0]);
            return 1;
        }
    }

    // Validate inputs
    if (input_file.empty()) {
        std::cerr << "Error: No input file specified\n";
        printUsage(argv[0]);
        return 1;
    }

    // Auto-generate output filename if not specified
    if (options.output_path.empty()) {
        std::string extension = (options.target == hybrid::TargetLanguage::Rust) ? ".rs" : ".go";
        size_t dot_pos = input_file.find_last_of('.');
        if (dot_pos != std::string::npos) {
            options.output_path = input_file.substr(0, dot_pos) + extension;
        } else {
            options.output_path = input_file + extension;
        }
    }

    // Create transpiler and run
    std::cout << "Transpiling " << input_file << " to "
              << (options.target == hybrid::TargetLanguage::Rust ? "Rust" : "Go")
              << "...\n";

    hybrid::Transpiler transpiler(options);

    if (!transpiler.transpile(input_file)) {
        std::cerr << "Error: Transpilation failed\n";
        std::cerr << transpiler.getLastError() << "\n";
        return 1;
    }

    std::cout << "Successfully transpiled to: " << options.output_path << "\n";

    return 0;
}
