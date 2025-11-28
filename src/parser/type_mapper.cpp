#include "ir.h"
#include <map>
#include <string>

namespace hybrid {

/**
 * Type Mapper - Maps C++ types to IR type representation
 */
class TypeMapper {
public:
    static std::shared_ptr<Type> mapBuiltinType(const std::string& cpp_type) {
        static const std::map<std::string, TypeKind> builtin_map = {
            {"void", TypeKind::Void},
            {"bool", TypeKind::Bool},
            {"char", TypeKind::Integer},
            {"short", TypeKind::Integer},
            {"int", TypeKind::Integer},
            {"long", TypeKind::Integer},
            {"long long", TypeKind::Integer},
            {"unsigned char", TypeKind::Integer},
            {"unsigned short", TypeKind::Integer},
            {"unsigned int", TypeKind::Integer},
            {"unsigned long", TypeKind::Integer},
            {"unsigned long long", TypeKind::Integer},
            {"int8_t", TypeKind::Integer},
            {"int16_t", TypeKind::Integer},
            {"int32_t", TypeKind::Integer},
            {"int64_t", TypeKind::Integer},
            {"uint8_t", TypeKind::Integer},
            {"uint16_t", TypeKind::Integer},
            {"uint32_t", TypeKind::Integer},
            {"uint64_t", TypeKind::Integer},
            {"size_t", TypeKind::Integer},
            {"float", TypeKind::Float},
            {"double", TypeKind::Float}
        };

        auto it = builtin_map.find(cpp_type);
        if (it != builtin_map.end()) {
            auto type = std::make_shared<Type>(it->second);
            type->name = cpp_type;
            type->size_bytes = getSizeForType(cpp_type);
            type->alignment = type->size_bytes;
            return type;
        }

        return nullptr;
    }

    static std::shared_ptr<Type> mapPointerType(std::shared_ptr<Type> pointee) {
        auto ptr_type = std::make_shared<Type>(TypeKind::Pointer);
        ptr_type->element_type = pointee;
        ptr_type->size_bytes = sizeof(void*); // Platform-dependent
        ptr_type->alignment = sizeof(void*);

        if (pointee) {
            ptr_type->name = pointee->name + "*";
        }

        return ptr_type;
    }

    static std::shared_ptr<Type> mapReferenceType(std::shared_ptr<Type> referred, bool is_const) {
        auto ref_type = std::make_shared<Type>(TypeKind::Reference);
        ref_type->element_type = referred;
        ref_type->is_const = is_const;
        ref_type->size_bytes = sizeof(void*);
        ref_type->alignment = sizeof(void*);

        if (referred) {
            ref_type->name = referred->name + "&";
        }

        return ref_type;
    }

    static std::shared_ptr<Type> mapArrayType(std::shared_ptr<Type> element, size_t count) {
        auto array_type = std::make_shared<Type>(TypeKind::Array);
        array_type->element_type = element;

        if (element) {
            array_type->size_bytes = element->size_bytes * count;
            array_type->alignment = element->alignment;
            array_type->name = element->name + "[" + std::to_string(count) + "]";
        }

        return array_type;
    }

    static std::shared_ptr<Type> mapSmartPointer(const std::string& smart_ptr_name,
                                                   std::shared_ptr<Type> element) {
        auto ptr_type = std::make_shared<Type>(TypeKind::Pointer);
        ptr_type->name = smart_ptr_name;
        ptr_type->element_type = element;
        ptr_type->size_bytes = sizeof(void*);
        ptr_type->alignment = sizeof(void*);
        return ptr_type;
    }

private:
    static size_t getSizeForType(const std::string& type_name) {
        static const std::map<std::string, size_t> size_map = {
            {"void", 0},
            {"bool", 1},
            {"char", 1},
            {"short", 2},
            {"int", 4},
            {"long", 8},
            {"long long", 8},
            {"unsigned char", 1},
            {"unsigned short", 2},
            {"unsigned int", 4},
            {"unsigned long", 8},
            {"unsigned long long", 8},
            {"int8_t", 1},
            {"int16_t", 2},
            {"int32_t", 4},
            {"int64_t", 8},
            {"uint8_t", 1},
            {"uint16_t", 2},
            {"uint32_t", 4},
            {"uint64_t", 8},
            {"size_t", 8},
            {"float", 4},
            {"double", 8}
        };

        auto it = size_map.find(type_name);
        return (it != size_map.end()) ? it->second : 0;
    }
};

/**
 * Memory Management Pattern Analyzer
 * Analyzes C++ memory patterns and determines appropriate conversions
 */
class MemoryPatternAnalyzer {
public:
    enum class OwnershipPattern {
        UniqueOwnership,    // unique_ptr -> Box in Rust
        SharedOwnership,    // shared_ptr -> Rc/Arc in Rust
        BorrowedReference,  // const& -> & in Rust
        MutableBorrow,      // & -> &mut in Rust
        RawPointer,         // * -> raw pointer (unsafe)
        ValueSemantics      // Direct value
    };

    static OwnershipPattern analyzePointerPattern(const std::shared_ptr<Type>& type) {
        if (!type) return OwnershipPattern::ValueSemantics;

        if (type->kind == TypeKind::Pointer) {
            if (type->name.find("unique_ptr") != std::string::npos) {
                return OwnershipPattern::UniqueOwnership;
            }
            if (type->name.find("shared_ptr") != std::string::npos) {
                return OwnershipPattern::SharedOwnership;
            }
            return OwnershipPattern::RawPointer;
        }

        if (type->kind == TypeKind::Reference) {
            if (type->is_const) {
                return OwnershipPattern::BorrowedReference;
            } else {
                return OwnershipPattern::MutableBorrow;
            }
        }

        return OwnershipPattern::ValueSemantics;
    }

    static std::string getRustEquivalent(OwnershipPattern pattern,
                                        const std::string& inner_type) {
        switch (pattern) {
            case OwnershipPattern::UniqueOwnership:
                return "Box<" + inner_type + ">";
            case OwnershipPattern::SharedOwnership:
                return "Rc<" + inner_type + ">"; // or Arc for thread-safe
            case OwnershipPattern::BorrowedReference:
                return "&" + inner_type;
            case OwnershipPattern::MutableBorrow:
                return "&mut " + inner_type;
            case OwnershipPattern::RawPointer:
                return "*const " + inner_type;
            case OwnershipPattern::ValueSemantics:
            default:
                return inner_type;
        }
    }

    static std::string getGoEquivalent(OwnershipPattern pattern,
                                      const std::string& inner_type) {
        switch (pattern) {
            case OwnershipPattern::UniqueOwnership:
            case OwnershipPattern::SharedOwnership:
            case OwnershipPattern::BorrowedReference:
            case OwnershipPattern::MutableBorrow:
            case OwnershipPattern::RawPointer:
                return "*" + inner_type; // Go uses GC, pointers are managed
            case OwnershipPattern::ValueSemantics:
            default:
                return inner_type;
        }
    }
};

} // namespace hybrid
