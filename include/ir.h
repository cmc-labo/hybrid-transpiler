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
    Template
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
    std::vector<std::string> template_params;

    // Access control
    struct AccessSection {
        enum Level { Public, Protected, Private };
        Level level;
        std::vector<std::string> members;
    };
    std::vector<AccessSection> access_sections;
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
