#include "ir.h"
#include <map>
#include <string>
#include <regex>

namespace hybrid {

/**
 * STL Container Mapper
 * Maps C++ STL containers to IR type representation with template arguments
 */
class STLContainerMapper {
public:
    /**
     * Check if a type name is an STL container
     */
    static bool isSTLContainer(const std::string& type_name) {
        static const std::vector<std::string> containers = {
            "std::vector", "vector",
            "std::list", "list",
            "std::deque", "deque",
            "std::map", "map",
            "std::unordered_map", "unordered_map",
            "std::set", "set",
            "std::unordered_set", "unordered_set",
            "std::string", "string",
            "std::pair", "pair",
            "std::optional", "optional"
        };

        for (const auto& container : containers) {
            if (type_name.find(container) != std::string::npos) {
                return true;
            }
        }
        return false;
    }

    /**
     * Map STL container to IR type
     */
    static std::shared_ptr<Type> mapSTLContainer(const std::string& cpp_type) {
        // Parse container type and template arguments
        std::string container_name = extractContainerName(cpp_type);
        std::vector<std::string> template_args = extractTemplateArgs(cpp_type);

        TypeKind kind = getSTLContainerKind(container_name);
        if (kind == TypeKind::Template) {
            return nullptr; // Unknown container
        }

        auto type = std::make_shared<Type>(kind);
        type->name = container_name;

        // Map template arguments
        for (const auto& arg : template_args) {
            // Recursively map template arguments
            auto arg_type = mapTypeRecursive(arg);
            if (arg_type) {
                type->template_args.push_back(arg_type);
            }
        }

        return type;
    }

    /**
     * Get Rust equivalent for STL container
     */
    static std::string getRustEquivalent(const std::shared_ptr<Type>& type) {
        if (!type) return "()";

        switch (type->kind) {
            case TypeKind::StdVector:
                if (!type->template_args.empty()) {
                    return "Vec<" + getRustEquivalent(type->template_args[0]) + ">";
                }
                return "Vec<()>";

            case TypeKind::StdList:
                // Rust LinkedList is rarely used, prefer VecDeque
                if (!type->template_args.empty()) {
                    return "std::collections::LinkedList<" +
                           getRustEquivalent(type->template_args[0]) + ">";
                }
                return "std::collections::LinkedList<()>";

            case TypeKind::StdDeque:
                if (!type->template_args.empty()) {
                    return "std::collections::VecDeque<" +
                           getRustEquivalent(type->template_args[0]) + ">";
                }
                return "std::collections::VecDeque<()>";

            case TypeKind::StdMap:
                if (type->template_args.size() >= 2) {
                    return "std::collections::BTreeMap<" +
                           getRustEquivalent(type->template_args[0]) + ", " +
                           getRustEquivalent(type->template_args[1]) + ">";
                }
                return "std::collections::BTreeMap<(), ()>";

            case TypeKind::StdUnorderedMap:
                if (type->template_args.size() >= 2) {
                    return "std::collections::HashMap<" +
                           getRustEquivalent(type->template_args[0]) + ", " +
                           getRustEquivalent(type->template_args[1]) + ">";
                }
                return "std::collections::HashMap<(), ()>";

            case TypeKind::StdSet:
                if (!type->template_args.empty()) {
                    return "std::collections::BTreeSet<" +
                           getRustEquivalent(type->template_args[0]) + ">";
                }
                return "std::collections::BTreeSet<()>";

            case TypeKind::StdUnorderedSet:
                if (!type->template_args.empty()) {
                    return "std::collections::HashSet<" +
                           getRustEquivalent(type->template_args[0]) + ">";
                }
                return "std::collections::HashSet<()>";

            case TypeKind::StdString:
                return "String";

            case TypeKind::StdPair:
                if (type->template_args.size() >= 2) {
                    return "(" + getRustEquivalent(type->template_args[0]) + ", " +
                           getRustEquivalent(type->template_args[1]) + ")";
                }
                return "((), ())";

            case TypeKind::StdOptional:
                if (!type->template_args.empty()) {
                    return "Option<" + getRustEquivalent(type->template_args[0]) + ">";
                }
                return "Option<()>";

            // Builtin types
            case TypeKind::Integer:
                if (type->name == "int") return "i32";
                if (type->name == "unsigned int") return "u32";
                if (type->name == "long") return "i64";
                if (type->name == "unsigned long") return "u64";
                if (type->name == "short") return "i16";
                if (type->name == "unsigned short") return "u16";
                if (type->name == "char") return "i8";
                if (type->name == "unsigned char") return "u8";
                if (type->name.find("int8_t") != std::string::npos) return "i8";
                if (type->name.find("int16_t") != std::string::npos) return "i16";
                if (type->name.find("int32_t") != std::string::npos) return "i32";
                if (type->name.find("int64_t") != std::string::npos) return "i64";
                if (type->name.find("uint8_t") != std::string::npos) return "u8";
                if (type->name.find("uint16_t") != std::string::npos) return "u16";
                if (type->name.find("uint32_t") != std::string::npos) return "u32";
                if (type->name.find("uint64_t") != std::string::npos) return "u64";
                if (type->name.find("size_t") != std::string::npos) return "usize";
                return "i32";

            case TypeKind::Float:
                if (type->name == "float") return "f32";
                if (type->name == "double") return "f64";
                return "f64";

            case TypeKind::Bool:
                return "bool";

            case TypeKind::Void:
                return "()";

            default:
                return type->name;
        }
    }

    /**
     * Get Go equivalent for STL container
     */
    static std::string getGoEquivalent(const std::shared_ptr<Type>& type) {
        if (!type) return "interface{}";

        switch (type->kind) {
            case TypeKind::StdVector:
            case TypeKind::StdList:
            case TypeKind::StdDeque:
                if (!type->template_args.empty()) {
                    return "[]" + getGoEquivalent(type->template_args[0]);
                }
                return "[]interface{}";

            case TypeKind::StdMap:
            case TypeKind::StdUnorderedMap:
                if (type->template_args.size() >= 2) {
                    return "map[" + getGoEquivalent(type->template_args[0]) + "]" +
                           getGoEquivalent(type->template_args[1]);
                }
                return "map[interface{}]interface{}";

            case TypeKind::StdSet:
            case TypeKind::StdUnorderedSet:
                // Go doesn't have built-in set, use map[T]bool
                if (!type->template_args.empty()) {
                    return "map[" + getGoEquivalent(type->template_args[0]) + "]bool";
                }
                return "map[interface{}]bool";

            case TypeKind::StdString:
                return "string";

            case TypeKind::StdPair:
                // Go doesn't have tuples, use struct
                if (type->template_args.size() >= 2) {
                    return "struct { First " + getGoEquivalent(type->template_args[0]) +
                           "; Second " + getGoEquivalent(type->template_args[1]) + " }";
                }
                return "struct { First interface{}; Second interface{} }";

            case TypeKind::StdOptional:
                // Go uses pointer for optional, or explicit Optional pattern
                if (!type->template_args.empty()) {
                    return "*" + getGoEquivalent(type->template_args[0]);
                }
                return "*interface{}";

            // Builtin types
            case TypeKind::Integer:
                if (type->name == "int") return "int32";
                if (type->name == "unsigned int") return "uint32";
                if (type->name == "long") return "int64";
                if (type->name == "unsigned long") return "uint64";
                if (type->name == "short") return "int16";
                if (type->name == "unsigned short") return "uint16";
                if (type->name == "char") return "int8";
                if (type->name == "unsigned char") return "uint8";
                if (type->name.find("size_t") != std::string::npos) return "uint64";
                return "int32";

            case TypeKind::Float:
                if (type->name == "float") return "float32";
                if (type->name == "double") return "float64";
                return "float64";

            case TypeKind::Bool:
                return "bool";

            case TypeKind::Void:
                return "interface{}";

            default:
                return type->name;
        }
    }

private:
    /**
     * Extract container name from full type (e.g., "std::vector" from "std::vector<int>")
     */
    static std::string extractContainerName(const std::string& type_str) {
        size_t pos = type_str.find('<');
        if (pos != std::string::npos) {
            std::string name = type_str.substr(0, pos);
            // Remove std:: prefix if present
            if (name.find("std::") == 0) {
                name = name.substr(5);
            }
            return name;
        }
        return type_str;
    }

    /**
     * Extract template arguments
     */
    static std::vector<std::string> extractTemplateArgs(const std::string& type_str) {
        std::vector<std::string> args;
        size_t start = type_str.find('<');
        size_t end = type_str.rfind('>');

        if (start == std::string::npos || end == std::string::npos || start >= end) {
            return args;
        }

        std::string args_str = type_str.substr(start + 1, end - start - 1);

        // Split by comma, accounting for nested templates
        int depth = 0;
        size_t last = 0;
        for (size_t i = 0; i < args_str.length(); ++i) {
            if (args_str[i] == '<') depth++;
            else if (args_str[i] == '>') depth--;
            else if (args_str[i] == ',' && depth == 0) {
                std::string arg = args_str.substr(last, i - last);
                arg = trim(arg);
                if (!arg.empty()) args.push_back(arg);
                last = i + 1;
            }
        }

        // Last argument
        std::string arg = args_str.substr(last);
        arg = trim(arg);
        if (!arg.empty()) args.push_back(arg);

        return args;
    }

    /**
     * Get TypeKind for STL container name
     */
    static TypeKind getSTLContainerKind(const std::string& name) {
        static const std::map<std::string, TypeKind> kind_map = {
            {"vector", TypeKind::StdVector},
            {"list", TypeKind::StdList},
            {"deque", TypeKind::StdDeque},
            {"map", TypeKind::StdMap},
            {"unordered_map", TypeKind::StdUnorderedMap},
            {"set", TypeKind::StdSet},
            {"unordered_set", TypeKind::StdUnorderedSet},
            {"string", TypeKind::StdString},
            {"pair", TypeKind::StdPair},
            {"optional", TypeKind::StdOptional}
        };

        auto it = kind_map.find(name);
        return (it != kind_map.end()) ? it->second : TypeKind::Template;
    }

    /**
     * Recursively map type (handles nested templates)
     */
    static std::shared_ptr<Type> mapTypeRecursive(const std::string& type_str) {
        std::string trimmed = trim(type_str);

        // Check if it's an STL container
        if (isSTLContainer(trimmed)) {
            return mapSTLContainer(trimmed);
        }

        // Check if it's a builtin type
        return mapBuiltinType(trimmed);
    }

    /**
     * Map builtin types
     */
    static std::shared_ptr<Type> mapBuiltinType(const std::string& type_name) {
        static const std::map<std::string, TypeKind> builtin_map = {
            {"int", TypeKind::Integer},
            {"unsigned int", TypeKind::Integer},
            {"long", TypeKind::Integer},
            {"unsigned long", TypeKind::Integer},
            {"short", TypeKind::Integer},
            {"unsigned short", TypeKind::Integer},
            {"char", TypeKind::Integer},
            {"unsigned char", TypeKind::Integer},
            {"float", TypeKind::Float},
            {"double", TypeKind::Float},
            {"bool", TypeKind::Bool},
            {"void", TypeKind::Void}
        };

        auto it = builtin_map.find(type_name);
        if (it != builtin_map.end()) {
            auto type = std::make_shared<Type>(it->second);
            type->name = type_name;
            return type;
        }

        // Unknown type - treat as class/struct
        auto type = std::make_shared<Type>(TypeKind::Class);
        type->name = type_name;
        return type;
    }

    /**
     * Trim whitespace from string
     */
    static std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\n\r");
        if (start == std::string::npos) return "";

        size_t end = str.find_last_not_of(" \t\n\r");
        return str.substr(start, end - start + 1);
    }
};

} // namespace hybrid
