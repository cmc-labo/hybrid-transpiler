#include "ir.h"
#include "codegen.h"
#include <cassert>
#include <iostream>

namespace hybrid {
namespace test {

void testRustCodeGeneration() {
    IR ir;

    // Create a simple class
    ClassDecl test_class;
    test_class.name = "TestClass";

    Variable field;
    field.name = "value";
    field.type = std::make_shared<Type>(TypeKind::Integer);
    field.type->name = "int";

    test_class.fields.push_back(field);

    Function method;
    method.name = "getValue";
    method.is_const = true;
    method.return_type = std::make_shared<Type>(TypeKind::Integer);
    method.return_type->name = "int";

    test_class.methods.push_back(method);

    ir.addClass(test_class);

    // Generate Rust code
    RustCodeGenerator rust_gen;
    std::string code = rust_gen.generate(ir);

    // Check generated code contains expected elements
    assert(code.find("pub struct") != std::string::npos);
    assert(code.find("test_class") != std::string::npos);
    assert(code.find("value") != std::string::npos);

    std::cout << "  ✓ Rust code generation test passed\n";
}

void testGoCodeGeneration() {
    IR ir;

    // Create a simple class
    ClassDecl test_class;
    test_class.name = "TestClass";

    Variable field;
    field.name = "value";
    field.type = std::make_shared<Type>(TypeKind::Integer);
    field.type->name = "int";

    test_class.fields.push_back(field);

    ir.addClass(test_class);

    // Generate Go code
    GoCodeGenerator go_gen;
    std::string code = go_gen.generate(ir);

    // Check generated code contains expected elements
    assert(code.find("type TestClass struct") != std::string::npos);
    assert(code.find("Value") != std::string::npos);

    std::cout << "  ✓ Go code generation test passed\n";
}

void runAllCodegenTests() {
    std::cout << "\nRunning Code Generation Tests:\n";
    testRustCodeGeneration();
    testGoCodeGeneration();
    std::cout << "All code generation tests passed!\n";
}

} // namespace test
} // namespace hybrid
