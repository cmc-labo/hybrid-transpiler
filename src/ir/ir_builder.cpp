#include "ir.h"

namespace hybrid {

void IR::addClass(const ClassDecl& class_decl) {
    classes_.push_back(class_decl);

    // Register the class as a type
    auto type = std::make_shared<Type>(TypeKind::Class);
    type->name = class_decl.name;
    registerType(class_decl.name, type);
}

void IR::addFunction(const Function& func) {
    functions_.push_back(func);
}

void IR::addGlobalVariable(const Variable& var) {
    global_vars_.push_back(var);
}

std::shared_ptr<Type> IR::findType(const std::string& name) const {
    auto it = type_registry_.find(name);
    if (it != type_registry_.end()) {
        return it->second;
    }
    return nullptr;
}

void IR::registerType(const std::string& name, std::shared_ptr<Type> type) {
    type_registry_[name] = type;
}

} // namespace hybrid
