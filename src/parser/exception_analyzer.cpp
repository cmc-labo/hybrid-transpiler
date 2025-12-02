/**
 * Exception Analyzer
 * Analyzes C++ exception handling and prepares for conversion to Result/error
 */

#include "ir.h"
#include <regex>
#include <algorithm>

namespace hybrid {

/**
 * Exception Analyzer
 * Detects and analyzes try-catch blocks, throw statements, and exception specifications
 */
class ExceptionAnalyzer {
public:
    /**
     * Analyze function body for exception handling patterns
     */
    void analyzeFunction(Function& func) {
        // Detect try-catch blocks
        detectTryCatchBlocks(func);

        // Detect throw statements
        detectThrowStatements(func);

        // Analyze exception specification
        analyzeExceptionSpec(func);

        // Determine if function may throw
        func.may_throw = func.exception_spec.can_throw ||
                         !func.try_catch_blocks.empty() ||
                         containsThrowStatement(func.body);
    }

private:
    /**
     * Detect try-catch blocks in function body
     */
    void detectTryCatchBlocks(Function& func) {
        std::string body = func.body;

        // Simple pattern matching for try-catch blocks
        // In real implementation, would use Clang AST
        std::regex try_catch_pattern(
            R"(try\s*\{([^}]*)\}\s*catch\s*\(([^)]+)\)\s*\{([^}]*)\})"
        );

        std::smatch match;
        std::string::const_iterator search_start(body.cbegin());

        while (std::regex_search(search_start, body.cend(), match, try_catch_pattern)) {
            TryCatchBlock block;
            block.try_body = match[1].str();

            // Parse catch clause
            TryCatchBlock::CatchClause clause;
            std::string catch_param = match[2].str();
            clause.handler_body = match[3].str();

            // Parse exception type and variable
            // Format: "const Type& var" or "Type var" or "..."
            parseCatchParameter(catch_param, clause.exception_type, clause.exception_var);

            block.catch_clauses.push_back(clause);
            func.try_catch_blocks.push_back(block);

            search_start = match.suffix().first;
        }
    }

    /**
     * Parse catch parameter to extract type and variable name
     */
    void parseCatchParameter(const std::string& param,
                            std::string& type,
                            std::string& var) {
        if (param == "...") {
            type = "...";  // catch-all
            var = "";
            return;
        }

        // Remove const, &, etc.
        std::string cleaned = param;
        cleaned = std::regex_replace(cleaned, std::regex(R"(\s*const\s*)"), " ");
        cleaned = std::regex_replace(cleaned, std::regex(R"(\s*&\s*)"), " ");
        cleaned = std::regex_replace(cleaned, std::regex(R"(\s+)"), " ");

        // Split into type and variable
        size_t last_space = cleaned.find_last_of(' ');
        if (last_space != std::string::npos) {
            type = cleaned.substr(0, last_space);
            var = cleaned.substr(last_space + 1);
        } else {
            type = cleaned;
            var = "e";  // default variable name
        }

        // Trim whitespace
        type.erase(0, type.find_first_not_of(" \t\n"));
        type.erase(type.find_last_not_of(" \t\n") + 1);
        var.erase(0, var.find_first_not_of(" \t\n"));
        var.erase(var.find_last_not_of(" \t\n") + 1);
    }

    /**
     * Detect throw statements in function body
     */
    void detectThrowStatements(Function& func) {
        std::regex throw_pattern(R"(throw\s+)");
        if (std::regex_search(func.body, throw_pattern)) {
            func.exception_spec.can_throw = true;
        }
    }

    /**
     * Check if body contains throw statement
     */
    bool containsThrowStatement(const std::string& body) {
        return body.find("throw ") != std::string::npos;
    }

    /**
     * Analyze exception specification (noexcept, throw())
     */
    void analyzeExceptionSpec(Function& func) {
        // This would be extracted from Clang AST in real implementation
        // For now, we'll detect common patterns in function signature

        // Check for noexcept
        if (func.name.find("noexcept") != std::string::npos) {
            func.exception_spec.is_noexcept = true;
            func.exception_spec.can_throw = false;
        }
    }
};

/**
 * Common exception type mappings
 */
class ExceptionTypeMapper {
public:
    /**
     * Convert C++ exception type to error description
     */
    static std::string getErrorDescription(const std::string& exception_type) {
        static const std::map<std::string, std::string> exception_map = {
            {"std::exception", "Standard exception"},
            {"std::runtime_error", "Runtime error"},
            {"std::logic_error", "Logic error"},
            {"std::invalid_argument", "Invalid argument"},
            {"std::out_of_range", "Out of range"},
            {"std::overflow_error", "Overflow error"},
            {"std::underflow_error", "Underflow error"},
            {"std::range_error", "Range error"},
            {"std::bad_alloc", "Memory allocation failed"},
            {"std::bad_cast", "Bad cast"},
            {"std::bad_typeid", "Bad typeid"},
            {"std::ios_base::failure", "I/O error"},
            {"...", "Unknown error"}
        };

        auto it = exception_map.find(exception_type);
        if (it != exception_map.end()) {
            return it->second;
        }
        return "Error: " + exception_type;
    }

    /**
     * Get Rust error type for C++ exception
     */
    static std::string getRustErrorType(const std::string& exception_type) {
        if (exception_type == "std::exception" ||
            exception_type == "std::runtime_error") {
            return "Box<dyn std::error::Error>";
        } else if (exception_type == "std::invalid_argument") {
            return "std::io::Error";
        } else if (exception_type == "...") {
            return "Box<dyn std::error::Error>";
        }
        return "String";  // Fallback to String for custom exceptions
    }

    /**
     * Get Go error type for C++ exception
     */
    static std::string getGoErrorType(const std::string& exception_type) {
        // Go uses 'error' interface for all errors
        return "error";
    }
};

/**
 * Exception conversion strategy
 */
enum class ExceptionStrategy {
    ResultType,      // Convert to Result<T, E> (Rust)
    ErrorReturn,     // Convert to (T, error) (Go)
    Panic,           // Convert to panic! (Rust) or panic() (Go)
    Ignore           // Remove exception handling
};

/**
 * Determine conversion strategy for exception handling
 */
class ExceptionStrategySelector {
public:
    static ExceptionStrategy selectStrategy(const Function& func,
                                           const std::string& target_lang) {
        // If function has no exception handling, no conversion needed
        if (!func.may_throw && func.try_catch_blocks.empty()) {
            return ExceptionStrategy::Ignore;
        }

        // If function is marked noexcept but contains try-catch,
        // convert to Result/error but handle internally
        if (func.exception_spec.is_noexcept && !func.try_catch_blocks.empty()) {
            if (target_lang == "rust") {
                return ExceptionStrategy::ResultType;
            } else {
                return ExceptionStrategy::ErrorReturn;
            }
        }

        // If function may throw and has try-catch blocks
        if (func.may_throw) {
            if (target_lang == "rust") {
                return ExceptionStrategy::ResultType;
            } else {
                return ExceptionStrategy::ErrorReturn;
            }
        }

        return ExceptionStrategy::Ignore;
    }
};

} // namespace hybrid
