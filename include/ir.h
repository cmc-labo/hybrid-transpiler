#ifndef HYBRID_IR_H
#define HYBRID_IR_H

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace hybrid {

/**
 * Type categories in the IR
 */
enum class TypeKind {
    Void,
    Bool,
    Integer,
    Float,
    Pointer,
    Reference,
    Array,
    Struct,
    Class,
    Enum,
    Function,
    Template,
    // STL Container types
    StdVector,
    StdList,
    StdDeque,
    StdMap,
    StdUnorderedMap,
    StdSet,
    StdUnorderedSet,
    StdString,
    StdPair,
    StdOptional,
    // Threading types
    StdThread,
    StdMutex,
    StdRecursiveMutex,
    StdSharedMutex,
    StdConditionVariable,
    StdAtomic,
    StdLockGuard,
    StdUniqueLock,
    StdSharedLock
};

/**
 * Type representation in IR
 */
class Type {
public:
    TypeKind kind;
    std::string name;
    bool is_const = false;
    bool is_mutable = true;

    // For composite types
    std::shared_ptr<Type> element_type;  // For pointers, arrays, references
    std::vector<std::shared_ptr<Type>> template_args;

    // Size information
    size_t size_bytes = 0;
    size_t alignment = 0;

    explicit Type(TypeKind k) : kind(k) {}
};

/**
 * Variable/field representation
 */
class Variable {
public:
    std::string name;
    std::shared_ptr<Type> type;
    bool is_static = false;
    bool is_const = false;
    std::string initializer;
};

/**
 * Function parameter
 */
class Parameter {
public:
    std::string name;
    std::shared_ptr<Type> type;
    bool has_default = false;
    std::string default_value;
};

/**
 * Exception specification
 */
class ExceptionSpec {
public:
    bool can_throw = false;
    std::vector<std::string> throw_types;  // Empty means can throw any
    bool is_noexcept = false;
};

/**
 * Try-Catch block representation
 */
class TryCatchBlock {
public:
    std::string try_body;

    struct CatchClause {
        std::string exception_type;
        std::string exception_var;
        std::string handler_body;
    };

    std::vector<CatchClause> catch_clauses;
};

/**
 * Template parameter representation
 */
class TemplateParameter {
public:
    enum Kind {
        Type,           // typename T or class T
        NonType,        // int N
        Template        // template<typename> class C
    };

    Kind kind;
    std::string name;
    std::string default_value;

    // For non-type parameters
    std::shared_ptr<Type> param_type;

    // Constraints (C++20 concepts)
    std::vector<std::string> constraints;
};

/**
 * Template specialization information
 */
class TemplateSpecialization {
public:
    bool is_partial = false;
    std::vector<std::string> specialized_args;
};

/**
 * Thread creation information
 */
class ThreadInfo {
public:
    std::string thread_var_name;
    std::string function_name;
    std::vector<std::string> arguments;
    bool detached = false;
    bool joinable = true;
};

/**
 * Mutex/Lock information
 */
class MutexInfo {
public:
    enum MutexType {
        Mutex,
        RecursiveMutex,
        SharedMutex,
        TimedMutex
    };

    MutexType type = Mutex;
    std::string mutex_var_name;
    std::shared_ptr<Type> protected_type;  // Type being protected by mutex
};

/**
 * Lock scope information
 */
class LockInfo {
public:
    enum LockType {
        LockGuard,
        UniqueLock,
        SharedLock,
        ScopedLock
    };

    LockType type = LockGuard;
    std::string lock_var_name;
    std::string mutex_name;
    std::string scope_body;
};

/**
 * Atomic operation information
 */
class AtomicInfo {
public:
    std::string atomic_var_name;
    std::shared_ptr<Type> value_type;
    std::vector<std::string> operations;  // load, store, fetch_add, etc.
};

/**
 * Condition variable information
 */
class ConditionVariableInfo {
public:
    std::string cv_var_name;
    std::string associated_mutex;
    std::vector<std::string> wait_conditions;
};

/**
 * Function representation
 */
class Function {
public:
    std::string name;
    std::shared_ptr<Type> return_type;
    std::vector<Parameter> parameters;
    std::string body;

    bool is_const = false;
    bool is_static = false;
    bool is_virtual = false;
    bool is_pure_virtual = false;
    bool is_constructor = false;
    bool is_destructor = false;

    // Ownership analysis results
    std::vector<std::string> moved_params;
    std::vector<std::string> borrowed_params;

    // Exception handling
    ExceptionSpec exception_spec;
    std::vector<TryCatchBlock> try_catch_blocks;
    bool may_throw = false;  // Analysis result: function may throw

    // Template information
    bool is_template = false;
    std::vector<TemplateParameter> template_parameters;
    TemplateSpecialization specialization;

    // Threading information
    std::vector<ThreadInfo> threads_created;
    std::vector<LockInfo> lock_scopes;
    std::vector<AtomicInfo> atomic_operations;
    std::vector<ConditionVariableInfo> condition_variables;
    bool uses_threading = false;
};

/**
 * Class/Struct representation
 */
class ClassDecl {
public:
    std::string name;
    bool is_struct = false;

    std::vector<Variable> fields;
    std::vector<Function> methods;
    std::vector<std::string> base_classes;

    // Template information
    bool is_template = false;
    std::vector<TemplateParameter> template_parameters;
    TemplateSpecialization specialization;

    // Access control
    struct AccessSection {
        enum Level { Public, Protected, Private };
        Level level;
        std::vector<std::string> members;
    };
    std::vector<AccessSection> access_sections;

    // Threading information
    std::vector<MutexInfo> mutexes;
    std::vector<AtomicInfo> atomic_fields;
    bool thread_safe = false;
};

/**
 * Intermediate Representation
 * Contains parsed and analyzed C++ code in a language-neutral format
 */
class IR {
public:
    IR() = default;

    void addClass(const ClassDecl& class_decl);
    void addFunction(const Function& func);
    void addGlobalVariable(const Variable& var);

    const std::vector<ClassDecl>& getClasses() const { return classes_; }
    const std::vector<Function>& getFunctions() const { return functions_; }
    const std::vector<Variable>& getGlobalVariables() const { return global_vars_; }

    // Type lookup
    std::shared_ptr<Type> findType(const std::string& name) const;
    void registerType(const std::string& name, std::shared_ptr<Type> type);

private:
    std::vector<ClassDecl> classes_;
    std::vector<Function> functions_;
    std::vector<Variable> global_vars_;
    std::map<std::string, std::shared_ptr<Type>> type_registry_;
};

} // namespace hybrid

#endif // HYBRID_IR_H
