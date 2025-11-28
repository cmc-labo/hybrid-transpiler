#include "../src/parser/type_mapper.cpp"
#include <cassert>
#include <iostream>

namespace hybrid {
namespace test {

void testBuiltinTypeMapping() {
    auto int_type = TypeMapper::mapBuiltinType("int");
    assert(int_type != nullptr);
    assert(int_type->kind == TypeKind::Integer);
    assert(int_type->name == "int");
    assert(int_type->size_bytes == 4);
    std::cout << "  ✓ Builtin type mapping test passed\n";
}

void testPointerTypeMapping() {
    auto int_type = TypeMapper::mapBuiltinType("int");
    auto ptr_type = TypeMapper::mapPointerType(int_type);

    assert(ptr_type != nullptr);
    assert(ptr_type->kind == TypeKind::Pointer);
    assert(ptr_type->element_type == int_type);
    std::cout << "  ✓ Pointer type mapping test passed\n";
}

void testReferenceTypeMapping() {
    auto int_type = TypeMapper::mapBuiltinType("int");
    auto ref_type = TypeMapper::mapReferenceType(int_type, true);

    assert(ref_type != nullptr);
    assert(ref_type->kind == TypeKind::Reference);
    assert(ref_type->is_const == true);
    assert(ref_type->element_type == int_type);
    std::cout << "  ✓ Reference type mapping test passed\n";
}

void testArrayTypeMapping() {
    auto int_type = TypeMapper::mapBuiltinType("int");
    auto array_type = TypeMapper::mapArrayType(int_type, 10);

    assert(array_type != nullptr);
    assert(array_type->kind == TypeKind::Array);
    assert(array_type->element_type == int_type);
    assert(array_type->size_bytes == 40); // 10 * 4 bytes
    std::cout << "  ✓ Array type mapping test passed\n";
}

void testSmartPointerMapping() {
    auto int_type = TypeMapper::mapBuiltinType("int");
    auto unique_ptr = TypeMapper::mapSmartPointer("unique_ptr<int>", int_type);

    assert(unique_ptr != nullptr);
    assert(unique_ptr->kind == TypeKind::Pointer);
    assert(unique_ptr->element_type == int_type);
    std::cout << "  ✓ Smart pointer mapping test passed\n";
}

void runAllTypeMappingTests() {
    std::cout << "\nRunning Type Mapping Tests:\n";
    testBuiltinTypeMapping();
    testPointerTypeMapping();
    testReferenceTypeMapping();
    testArrayTypeMapping();
    testSmartPointerMapping();
    std::cout << "All type mapping tests passed!\n";
}

} // namespace test
} // namespace hybrid
