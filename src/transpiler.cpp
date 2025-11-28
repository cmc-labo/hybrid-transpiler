#include "transpiler.h"
#include "ir.h"
#include "codegen.h"
#include <fstream>
#include <sstream>

namespace hybrid {

Transpiler::Transpiler(const TranspilerOptions& options)
    : options_(options), ir_(std::make_unique<IR>()) {

    // Create appropriate code generator based on target
    if (options.target == TargetLanguage::Rust) {
        codegen_ = std::make_unique<RustCodeGenerator>();
    } else if (options.target == TargetLanguage::Go) {
        codegen_ = std::make_unique<GoCodeGenerator>();
    }
}

Transpiler::~Transpiler() = default;

bool Transpiler::transpile(const std::string& input_path) {
    // Parse the input file
    if (!parseSourceFile(input_path)) {
        return false;
    }

    // Generate output code
    if (!generateCode(options_.output_path)) {
        return false;
    }

    return true;
}

bool Transpiler::transpileBatch(const std::vector<std::string>& input_paths) {
    for (const auto& path : input_paths) {
        if (!transpile(path)) {
            return false;
        }
    }
    return true;
}

bool Transpiler::parseSourceFile(const std::string& input_path) {
    // TODO: Implement full Clang-based parsing
    // For now, this is a placeholder that demonstrates the structure

    std::ifstream file(input_path);
    if (!file.is_open()) {
        last_error_ = "Failed to open input file: " + input_path;
        return false;
    }

    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    // In a real implementation, this would:
    // 1. Use Clang's LibTooling to parse the C++ source
    // 2. Traverse the AST
    // 3. Build the IR representation
    // 4. Perform ownership analysis
    // 5. Validate safety properties

    // For demonstration, create a simple example IR
    // (In production, this would come from actual Clang AST traversal)

    ClassDecl point_class;
    point_class.name = "Point";
    point_class.is_struct = false;

    Variable x_field;
    x_field.name = "x";
    x_field.type = std::make_shared<Type>(TypeKind::Integer);
    x_field.type->name = "int";

    Variable y_field;
    y_field.name = "y";
    y_field.type = std::make_shared<Type>(TypeKind::Integer);
    y_field.type->name = "int";

    point_class.fields.push_back(x_field);
    point_class.fields.push_back(y_field);

    Function constructor;
    constructor.name = "Point";
    constructor.is_constructor = true;
    constructor.return_type = std::make_shared<Type>(TypeKind::Void);

    Parameter x_param;
    x_param.name = "x";
    x_param.type = std::make_shared<Type>(TypeKind::Integer);
    x_param.type->name = "int";

    Parameter y_param;
    y_param.name = "y";
    y_param.type = std::make_shared<Type>(TypeKind::Integer);
    y_param.type->name = "int";

    constructor.parameters.push_back(x_param);
    constructor.parameters.push_back(y_param);

    point_class.methods.push_back(constructor);

    ir_->addClass(point_class);

    return true;
}

bool Transpiler::generateCode(const std::string& output_path) {
    if (!codegen_) {
        last_error_ = "Code generator not initialized";
        return false;
    }

    // Generate code from IR
    std::string generated_code = codegen_->generate(*ir_);

    // Write to output file
    std::ofstream out_file(output_path);
    if (!out_file.is_open()) {
        last_error_ = "Failed to open output file: " + output_path;
        return false;
    }

    out_file << generated_code;
    out_file.close();

    return true;
}

} // namespace hybrid
