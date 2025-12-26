#include "transpiler.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

void printUsage(const char* program_name) {
    std::cout << "Hybrid Transpiler - Convert C++ code to modern, safe languages (Rust/Go)\n\n";

    std::cout << "Features:\n";
    std::cout << "  • Classes → Structs + Traits/Interfaces\n";
    std::cout << "  • Smart pointers → Ownership models\n";
    std::cout << "  • Templates → Generics\n";
    std::cout << "  • Exceptions → Result types\n";
    std::cout << "  • Threading → Safe concurrency\n";
    std::cout << "  • Async/Coroutines → async/await\n\n";

    std::cout << "Usage: " << program_name << " [options]\n\n";

    std::cout << "Options:\n";
    std::cout << "  -i, --input <file>      Input C++ source file (required)\n";
    std::cout << "  -o, --output <file>     Output file path (auto-generated if omitted)\n";
    std::cout << "  -t, --target <lang>     Target language: rust, go [default: rust]\n";
    std::cout << "  -O, --opt-level <N>     Optimization level 0-3 [default: 0]\n";
    std::cout << "                          0 = readable, 1 = balanced,\n";
    std::cout << "                          2 = optimized, 3 = aggressive\n";
    std::cout << "  --no-safety-checks      Disable safety checks\n";
    std::cout << "  --no-comments           Don't preserve comments\n";
    std::cout << "  --gen-tests             Generate test cases\n";
    std::cout << "  --verbose               Enable verbose output\n";
    std::cout << "  --quiet                 Minimal output (errors only)\n";
    std::cout << "  -h, --help              Show this help message\n";
    std::cout << "  -v, --version           Show version information\n\n";

    std::cout << "Examples:\n";
    std::cout << "  # Basic conversion to Rust\n";
    std::cout << "  " << program_name << " -i example.cpp -o example.rs\n\n";
    std::cout << "  # Convert to Go with optimization\n";
    std::cout << "  " << program_name << " -i myclass.cpp -t go -O2\n\n";
    std::cout << "  # Auto-generate output filename\n";
    std::cout << "  " << program_name << " -i point.cpp -t rust\n";
    std::cout << "  # Output: point.rs\n\n";
    std::cout << "  # Verbose mode (detailed output)\n";
    std::cout << "  " << program_name << " -i example.cpp --verbose\n\n";
    std::cout << "  # Quiet mode (errors only)\n";
    std::cout << "  " << program_name << " -i example.cpp --quiet\n\n";
    std::cout << "  # Generate with test cases\n";
    std::cout << "  " << program_name << " -i vector.cpp --gen-tests\n\n";

    std::cout << "Supported C++ Features:\n";
    std::cout << "  • Classes, methods, constructors\n";
    std::cout << "  • Templates and generics\n";
    std::cout << "  • Smart pointers (unique_ptr, shared_ptr)\n";
    std::cout << "  • STL containers (vector, map, etc.)\n";
    std::cout << "  • Exception handling\n";
    std::cout << "  • Multithreading (std::thread, mutex)\n";
    std::cout << "  • Async/coroutines (C++20)\n\n";

    std::cout << "For more information and documentation:\n";
    std::cout << "  https://github.com/yourusername/hybrid-transpiler\n";
}

void printVersion() {
    std::cout << "Hybrid Transpiler v0.1.0\n";
    std::cout << "Built with LLVM/Clang support\n\n";
    std::cout << "Supported targets:\n";
    std::cout << "  • Rust (edition 2021)\n";
    std::cout << "  • Go (1.18+)\n\n";
    std::cout << "Copyright (c) 2025\n";
    std::cout << "License: MIT\n";
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
                std::cerr << "Usage: " << argv[0] << " --input <file.cpp> --output <file.rs> --target rust\n";
                std::cerr << "See '" << argv[0] << " --help' for more information.\n";
                return 1;
            }
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                options.output_path = argv[++i];
            } else {
                std::cerr << "Error: --output requires a file path\n";
                std::cerr << "Usage: " << argv[0] << " --output <file.rs>\n";
                std::cerr << "See '" << argv[0] << " --help' for more information.\n";
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
                    std::cerr << "Error: Unknown target language '" << target << "'\n";
                    std::cerr << "Supported languages: rust, go\n";

                    // Suggest corrections for common typos
                    if (target == "rs" || target == "r") {
                        std::cerr << "Did you mean 'rust'?\n";
                    } else if (target == "golang") {
                        std::cerr << "Use 'go' instead of 'golang'\n";
                    }

                    return 1;
                }
            } else {
                std::cerr << "Error: --target requires a language (rust|go)\n";
                std::cerr << "See '" << argv[0] << " --help' for more information.\n";
                return 1;
            }
        } else if (arg == "-O" || arg == "--opt-level") {
            if (i + 1 < argc) {
                try {
                    options.optimization_level = std::stoi(argv[++i]);
                    if (options.optimization_level < 0 || options.optimization_level > 3) {
                        std::cerr << "Error: Optimization level must be between 0 and 3\n";
                        std::cerr << "You specified: " << options.optimization_level << "\n";
                        std::cerr << "Valid values: 0 (readable), 1 (balanced), 2 (optimized), 3 (aggressive)\n";
                        return 1;
                    }
                } catch (const std::invalid_argument&) {
                    std::cerr << "Error: Invalid optimization level '" << argv[i] << "'\n";
                    std::cerr << "Expected a number between 0 and 3\n";
                    return 1;
                } catch (const std::out_of_range&) {
                    std::cerr << "Error: Optimization level out of range\n";
                    std::cerr << "Expected a number between 0 and 3\n";
                    return 1;
                }
            } else {
                std::cerr << "Error: --opt-level requires a number\n";
                std::cerr << "Usage: " << argv[0] << " --opt-level <0-3>\n";
                std::cerr << "See '" << argv[0] << " --help' for more information.\n";
                return 1;
            }
        } else if (arg == "--no-safety-checks") {
            options.enable_safety_checks = false;
        } else if (arg == "--no-comments") {
            options.preserve_comments = false;
        } else if (arg == "--gen-tests") {
            options.generate_tests = true;
        } else if (arg == "--verbose") {
            options.verbose = true;
        } else if (arg == "--quiet") {
            options.quiet = true;
        } else {
            std::cerr << "Error: Unknown option '" << arg << "'\n";

            // Suggest similar options
            if (arg.find("--in") == 0) {
                std::cerr << "Did you mean '--input'?\n";
            } else if (arg.find("--out") == 0) {
                std::cerr << "Did you mean '--output'?\n";
            } else if (arg.find("--tar") == 0) {
                std::cerr << "Did you mean '--target'?\n";
            } else if (arg.find("-O") == 0 && arg.length() > 2) {
                std::cerr << "Did you mean '-O " << arg.substr(2) << "'?\n";
            }

            std::cerr << "Run '" << argv[0] << " --help' for usage information.\n";
            return 1;
        }
    }

    // Validate inputs
    if (input_file.empty()) {
        std::cerr << "Error: No input file specified\n";
        std::cerr << "You must provide an input file with -i or --input\n";
        std::cerr << "Example: " << argv[0] << " -i example.cpp\n";
        std::cerr << "Run '" << argv[0] << " --help' for usage information.\n";
        return 1;
    }

    // Check if input file exists
    std::ifstream test_file(input_file);
    if (!test_file.good()) {
        std::cerr << "Error: Input file not found: " << input_file << "\n";
        std::cerr << "Please check the file path and try again.\n";
        return 1;
    }
    test_file.close();

    // Auto-generate output filename if not specified
    if (options.output_path.empty()) {
        std::string extension = (options.target == hybrid::TargetLanguage::Rust) ? ".rs" : ".go";
        size_t dot_pos = input_file.find_last_of('.');
        if (dot_pos != std::string::npos) {
            options.output_path = input_file.substr(0, dot_pos) + extension;
        } else {
            options.output_path = input_file + extension;
        }

        if (options.verbose) {
            std::cout << "Auto-generated output path: " << options.output_path << "\n";
        }
    }

    // Display verbose information
    if (options.verbose) {
        std::cout << "Configuration:\n";
        std::cout << "  Input:  " << input_file << "\n";
        std::cout << "  Output: " << options.output_path << "\n";
        std::cout << "  Target: " << (options.target == hybrid::TargetLanguage::Rust ? "Rust" : "Go") << "\n";
        std::cout << "  Optimization level: " << options.optimization_level << "\n";
        std::cout << "  Safety checks: " << (options.enable_safety_checks ? "enabled" : "disabled") << "\n";
        std::cout << "  Preserve comments: " << (options.preserve_comments ? "yes" : "no") << "\n";
        std::cout << "  Generate tests: " << (options.generate_tests ? "yes" : "no") << "\n";
    }

    // Create transpiler and run
    if (!options.quiet) {
        std::cout << "Transpiling " << input_file << " to "
                  << (options.target == hybrid::TargetLanguage::Rust ? "Rust" : "Go")
                  << "...\n";
    }

    hybrid::Transpiler transpiler(options);

    if (!transpiler.transpile(input_file)) {
        std::cerr << "Error: Transpilation failed\n";
        std::cerr << transpiler.getLastError() << "\n";
        return 1;
    }

    if (!options.quiet) {
        std::cout << "Successfully transpiled to: " << options.output_path << "\n";
    }

    return 0;
}
