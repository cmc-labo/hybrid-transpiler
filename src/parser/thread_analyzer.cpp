/**
 * Thread Analyzer
 * Analyzes C++ threading code and prepares for conversion to Rust/Go concurrency
 */

#include "ir.h"
#include <regex>
#include <algorithm>

namespace hybrid {

/**
 * Thread Analyzer
 * Detects and analyzes std::thread, mutexes, atomics, and condition variables
 */
class ThreadAnalyzer {
public:
    /**
     * Analyze function body for threading patterns
     */
    void analyzeFunction(Function& func) {
        // Detect thread creation
        detectThreadCreation(func);

        // Detect mutex and lock usage
        detectMutexUsage(func);

        // Detect atomic operations
        detectAtomicOperations(func);

        // Detect condition variables
        detectConditionVariables(func);

        // Mark function as using threading
        func.uses_threading = !func.threads_created.empty() ||
                             !func.lock_scopes.empty() ||
                             !func.atomic_operations.empty() ||
                             !func.condition_variables.empty();
    }

    /**
     * Analyze class for thread-safe patterns
     */
    void analyzeClass(ClassDecl& class_decl) {
        // Detect mutex members
        detectMutexMembers(class_decl);

        // Detect atomic members
        detectAtomicMembers(class_decl);

        // Determine if class is thread-safe
        class_decl.thread_safe = !class_decl.mutexes.empty() ||
                                 !class_decl.atomic_fields.empty();
    }

private:
    /**
     * Detect std::thread creation patterns
     */
    void detectThreadCreation(Function& func) {
        std::string body = func.body;

        // Pattern 1: std::thread t(func, args...)
        std::regex thread_pattern1(
            R"(std::thread\s+(\w+)\s*\(\s*(\w+)\s*(?:,\s*([^)]*))?\))"
        );

        // Pattern 2: std::thread t{func, args...}
        std::regex thread_pattern2(
            R"(std::thread\s+(\w+)\s*\{\s*(\w+)\s*(?:,\s*([^}]*))?\})"
        );

        // Pattern 3: auto t = std::thread(...)
        std::regex thread_pattern3(
            R"((?:auto|std::thread)\s+(\w+)\s*=\s*std::thread\s*\(\s*(\w+)\s*(?:,\s*([^)]*))?\))"
        );

        std::smatch match;
        std::string::const_iterator search_start(body.cbegin());

        // Search for pattern 1
        while (std::regex_search(search_start, body.cend(), match, thread_pattern1)) {
            ThreadInfo thread_info;
            thread_info.thread_var_name = match[1].str();
            thread_info.function_name = match[2].str();

            // Parse arguments
            if (match.size() > 3 && match[3].matched) {
                parseArguments(match[3].str(), thread_info.arguments);
            }

            func.threads_created.push_back(thread_info);
            search_start = match.suffix().first;
        }

        // Search for pattern 2
        search_start = body.cbegin();
        while (std::regex_search(search_start, body.cend(), match, thread_pattern2)) {
            ThreadInfo thread_info;
            thread_info.thread_var_name = match[1].str();
            thread_info.function_name = match[2].str();

            if (match.size() > 3 && match[3].matched) {
                parseArguments(match[3].str(), thread_info.arguments);
            }

            func.threads_created.push_back(thread_info);
            search_start = match.suffix().first;
        }

        // Search for pattern 3
        search_start = body.cbegin();
        while (std::regex_search(search_start, body.cend(), match, thread_pattern3)) {
            ThreadInfo thread_info;
            thread_info.thread_var_name = match[1].str();
            thread_info.function_name = match[2].str();

            if (match.size() > 3 && match[3].matched) {
                parseArguments(match[3].str(), thread_info.arguments);
            }

            func.threads_created.push_back(thread_info);
            search_start = match.suffix().first;
        }

        // Detect thread.detach()
        std::regex detach_pattern(R"((\w+)\.detach\s*\(\))");
        search_start = body.cbegin();
        while (std::regex_search(search_start, body.cend(), match, detach_pattern)) {
            std::string thread_var = match[1].str();

            // Find the thread in threads_created and mark as detached
            for (auto& thread : func.threads_created) {
                if (thread.thread_var_name == thread_var) {
                    thread.detached = true;
                    thread.joinable = false;
                    break;
                }
            }

            search_start = match.suffix().first;
        }
    }

    /**
     * Parse function arguments
     */
    void parseArguments(const std::string& args_str, std::vector<std::string>& arguments) {
        if (args_str.empty()) return;

        std::string current_arg;
        int paren_depth = 0;
        int bracket_depth = 0;

        for (char c : args_str) {
            if (c == '(' || c == '{') {
                paren_depth++;
                current_arg += c;
            } else if (c == ')' || c == '}') {
                paren_depth--;
                current_arg += c;
            } else if (c == '[') {
                bracket_depth++;
                current_arg += c;
            } else if (c == ']') {
                bracket_depth--;
                current_arg += c;
            } else if (c == ',' && paren_depth == 0 && bracket_depth == 0) {
                // End of current argument
                std::string trimmed = trim(current_arg);
                if (!trimmed.empty()) {
                    arguments.push_back(trimmed);
                }
                current_arg.clear();
            } else {
                current_arg += c;
            }
        }

        // Add last argument
        std::string trimmed = trim(current_arg);
        if (!trimmed.empty()) {
            arguments.push_back(trimmed);
        }
    }

    /**
     * Detect mutex usage and lock scopes
     */
    void detectMutexUsage(Function& func) {
        std::string body = func.body;

        // Pattern: std::lock_guard<std::mutex> lock(mutex_name)
        std::regex lock_guard_pattern(
            R"(std::lock_guard<[^>]+>\s+(\w+)\s*\(\s*(\w+)\s*\))"
        );

        // Pattern: std::unique_lock<std::mutex> lock(mutex_name)
        std::regex unique_lock_pattern(
            R"(std::unique_lock<[^>]+>\s+(\w+)\s*\(\s*(\w+)\s*\))"
        );

        // Pattern: std::shared_lock<std::shared_mutex> lock(mutex_name)
        std::regex shared_lock_pattern(
            R"(std::shared_lock<[^>]+>\s+(\w+)\s*\(\s*(\w+)\s*\))"
        );

        std::smatch match;
        std::string::const_iterator search_start(body.cbegin());

        // Detect lock_guard
        while (std::regex_search(search_start, body.cend(), match, lock_guard_pattern)) {
            LockInfo lock_info;
            lock_info.type = LockInfo::LockGuard;
            lock_info.lock_var_name = match[1].str();
            lock_info.mutex_name = match[2].str();

            func.lock_scopes.push_back(lock_info);
            search_start = match.suffix().first;
        }

        // Detect unique_lock
        search_start = body.cbegin();
        while (std::regex_search(search_start, body.cend(), match, unique_lock_pattern)) {
            LockInfo lock_info;
            lock_info.type = LockInfo::UniqueLock;
            lock_info.lock_var_name = match[1].str();
            lock_info.mutex_name = match[2].str();

            func.lock_scopes.push_back(lock_info);
            search_start = match.suffix().first;
        }

        // Detect shared_lock
        search_start = body.cbegin();
        while (std::regex_search(search_start, body.cend(), match, shared_lock_pattern)) {
            LockInfo lock_info;
            lock_info.type = LockInfo::SharedLock;
            lock_info.lock_var_name = match[1].str();
            lock_info.mutex_name = match[2].str();

            func.lock_scopes.push_back(lock_info);
            search_start = match.suffix().first;
        }
    }

    /**
     * Detect atomic operations
     */
    void detectAtomicOperations(Function& func) {
        std::string body = func.body;

        // Pattern: std::atomic<T> var_name
        std::regex atomic_pattern(
            R"(std::atomic<([^>]+)>\s+(\w+))"
        );

        // Pattern: var.load(), var.store(), var.fetch_add(), etc.
        std::regex atomic_op_pattern(
            R"((\w+)\.(load|store|fetch_add|fetch_sub|exchange|compare_exchange_weak|compare_exchange_strong)\s*\()"
        );

        std::smatch match;
        std::string::const_iterator search_start(body.cbegin());

        // Detect atomic variable declarations
        while (std::regex_search(search_start, body.cend(), match, atomic_pattern)) {
            AtomicInfo atomic_info;
            atomic_info.atomic_var_name = match[2].str();

            // Create type for atomic value
            std::string type_name = match[1].str();
            auto value_type = std::make_shared<Type>(TypeKind::Integer); // Simplified
            value_type->name = type_name;
            atomic_info.value_type = value_type;

            func.atomic_operations.push_back(atomic_info);
            search_start = match.suffix().first;
        }

        // Detect atomic operations
        search_start = body.cbegin();
        while (std::regex_search(search_start, body.cend(), match, atomic_op_pattern)) {
            std::string var_name = match[1].str();
            std::string operation = match[2].str();

            // Find or create atomic info
            bool found = false;
            for (auto& atomic : func.atomic_operations) {
                if (atomic.atomic_var_name == var_name) {
                    atomic.operations.push_back(operation);
                    found = true;
                    break;
                }
            }

            if (!found) {
                // Variable might be a member, create entry anyway
                AtomicInfo atomic_info;
                atomic_info.atomic_var_name = var_name;
                atomic_info.operations.push_back(operation);
                func.atomic_operations.push_back(atomic_info);
            }

            search_start = match.suffix().first;
        }
    }

    /**
     * Detect condition variables
     */
    void detectConditionVariables(Function& func) {
        std::string body = func.body;

        // Pattern: std::condition_variable cv_name
        std::regex cv_pattern(
            R"(std::condition_variable\s+(\w+))"
        );

        // Pattern: cv.wait(lock), cv.notify_one(), cv.notify_all()
        std::regex cv_op_pattern(
            R"((\w+)\.(wait|notify_one|notify_all|wait_for|wait_until)\s*\()"
        );

        std::smatch match;
        std::string::const_iterator search_start(body.cbegin());

        // Detect condition variable declarations
        while (std::regex_search(search_start, body.cend(), match, cv_pattern)) {
            ConditionVariableInfo cv_info;
            cv_info.cv_var_name = match[1].str();

            func.condition_variables.push_back(cv_info);
            search_start = match.suffix().first;
        }

        // Detect condition variable operations
        search_start = body.cbegin();
        while (std::regex_search(search_start, body.cend(), match, cv_op_pattern)) {
            std::string cv_var = match[1].str();
            std::string operation = match[2].str();

            // Find or create CV info
            bool found = false;
            for (auto& cv : func.condition_variables) {
                if (cv.cv_var_name == cv_var) {
                    cv.wait_conditions.push_back(operation);
                    found = true;
                    break;
                }
            }

            if (!found) {
                ConditionVariableInfo cv_info;
                cv_info.cv_var_name = cv_var;
                cv_info.wait_conditions.push_back(operation);
                func.condition_variables.push_back(cv_info);
            }

            search_start = match.suffix().first;
        }
    }

    /**
     * Detect mutex members in class
     */
    void detectMutexMembers(ClassDecl& class_decl) {
        for (const auto& field : class_decl.fields) {
            if (field.type->kind == TypeKind::StdMutex) {
                MutexInfo mutex_info;
                mutex_info.type = MutexInfo::Mutex;
                mutex_info.mutex_var_name = field.name;
                class_decl.mutexes.push_back(mutex_info);
            } else if (field.type->kind == TypeKind::StdRecursiveMutex) {
                MutexInfo mutex_info;
                mutex_info.type = MutexInfo::RecursiveMutex;
                mutex_info.mutex_var_name = field.name;
                class_decl.mutexes.push_back(mutex_info);
            } else if (field.type->kind == TypeKind::StdSharedMutex) {
                MutexInfo mutex_info;
                mutex_info.type = MutexInfo::SharedMutex;
                mutex_info.mutex_var_name = field.name;
                class_decl.mutexes.push_back(mutex_info);
            }
        }
    }

    /**
     * Detect atomic members in class
     */
    void detectAtomicMembers(ClassDecl& class_decl) {
        for (const auto& field : class_decl.fields) {
            if (field.type->kind == TypeKind::StdAtomic) {
                AtomicInfo atomic_info;
                atomic_info.atomic_var_name = field.name;
                atomic_info.value_type = field.type->element_type;
                class_decl.atomic_fields.push_back(atomic_info);
            }
        }
    }

    /**
     * Trim whitespace from string
     */
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == std::string::npos) return "";

        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, last - first + 1);
    }
};

/**
 * Threading type mapper
 * Maps C++ threading types to Rust and Go equivalents
 */
class ThreadingTypeMapper {
public:
    /**
     * Get Rust equivalent for C++ threading type
     */
    static std::string getRustThreadingType(TypeKind kind) {
        switch (kind) {
            case TypeKind::StdThread:
                return "std::thread::JoinHandle";
            case TypeKind::StdMutex:
                return "std::sync::Mutex";
            case TypeKind::StdRecursiveMutex:
                return "parking_lot::ReentrantMutex"; // Requires parking_lot crate
            case TypeKind::StdSharedMutex:
                return "std::sync::RwLock";
            case TypeKind::StdConditionVariable:
                return "std::sync::Condvar";
            case TypeKind::StdAtomic:
                return "std::sync::atomic::Atomic"; // Needs type suffix
            case TypeKind::StdLockGuard:
                return "MutexGuard"; // RAII type from Mutex::lock()
            case TypeKind::StdUniqueLock:
                return "MutexGuard";
            case TypeKind::StdSharedLock:
                return "RwLockReadGuard";
            default:
                return "/* Unknown threading type */";
        }
    }

    /**
     * Get Go equivalent for C++ threading type
     */
    static std::string getGoThreadingType(TypeKind kind) {
        switch (kind) {
            case TypeKind::StdThread:
                return "goroutine"; // Not a type, but a concept
            case TypeKind::StdMutex:
                return "sync.Mutex";
            case TypeKind::StdRecursiveMutex:
                return "sync.Mutex"; // Go doesn't have recursive mutex
            case TypeKind::StdSharedMutex:
                return "sync.RWMutex";
            case TypeKind::StdConditionVariable:
                return "sync.Cond";
            case TypeKind::StdAtomic:
                return "atomic"; // sync/atomic package
            case TypeKind::StdLockGuard:
                return "defer unlock"; // Pattern, not a type
            case TypeKind::StdUniqueLock:
                return "defer unlock";
            case TypeKind::StdSharedLock:
                return "defer RUnlock";
            default:
                return "/* Unknown threading type */";
        }
    }

    /**
     * Get Rust atomic type suffix
     */
    static std::string getRustAtomicType(const std::string& cpp_type) {
        if (cpp_type == "int" || cpp_type == "int32_t") return "AtomicI32";
        if (cpp_type == "long" || cpp_type == "int64_t") return "AtomicI64";
        if (cpp_type == "unsigned int" || cpp_type == "uint32_t") return "AtomicU32";
        if (cpp_type == "unsigned long" || cpp_type == "uint64_t") return "AtomicU64";
        if (cpp_type == "bool") return "AtomicBool";
        if (cpp_type == "size_t" || cpp_type == "usize") return "AtomicUsize";
        if (cpp_type == "ptrdiff_t" || cpp_type == "isize") return "AtomicIsize";
        return "AtomicUsize"; // Default
    }

    /**
     * Get Go atomic type
     */
    static std::string getGoAtomicType(const std::string& cpp_type) {
        if (cpp_type == "int" || cpp_type == "int32_t") return "atomic.Int32";
        if (cpp_type == "long" || cpp_type == "int64_t") return "atomic.Int64";
        if (cpp_type == "unsigned int" || cpp_type == "uint32_t") return "atomic.Uint32";
        if (cpp_type == "unsigned long" || cpp_type == "uint64_t") return "atomic.Uint64";
        if (cpp_type == "bool") return "atomic.Bool";
        if (cpp_type.find("*") != std::string::npos) return "atomic.Pointer";
        return "atomic.Value"; // Default
    }
};

} // namespace hybrid
