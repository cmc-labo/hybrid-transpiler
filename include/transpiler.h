#ifndef HYBRID_TRANSPILER_H
#define HYBRID_TRANSPILER_H

#include <string>
#include <memory>
#include <vector>

namespace hybrid {

// Forward declarations
class IR;
class CodeGenerator;

/**
 * Target language for transpilation
 */
enum class TargetLanguage {
    Rust,
    Go
};

/**
 * Transpilation options
 */
struct TranspilerOptions {
    TargetLanguage target = TargetLanguage::Rust;
    int optimization_level = 0;
    bool enable_safety_checks = true;
    bool preserve_comments = true;
    bool generate_tests = false;
    bool verbose = false;           // Verbose output
    bool quiet = false;             // Minimal output
    std::string output_path;
};

/**
 * Main transpiler class
 */
class Transpiler {
public:
    explicit Transpiler(const TranspilerOptions& options);
    ~Transpiler();

    /**
     * Transpile a single C++ source file
     * @param input_path Path to C++ source file
     * @return true if successful, false otherwise
     */
    bool transpile(const std::string& input_path);

    /**
     * Transpile multiple C++ source files
     * @param input_paths Vector of paths to C++ source files
     * @return true if all successful, false otherwise
     */
    bool transpileBatch(const std::vector<std::string>& input_paths);

    /**
     * Get the last error message
     */
    const std::string& getLastError() const { return last_error_; }

private:
    TranspilerOptions options_;
    std::unique_ptr<IR> ir_;
    std::unique_ptr<CodeGenerator> codegen_;
    std::string last_error_;

    bool parseSourceFile(const std::string& input_path);
    bool generateCode(const std::string& output_path);
};

} // namespace hybrid

#endif // HYBRID_TRANSPILER_H
