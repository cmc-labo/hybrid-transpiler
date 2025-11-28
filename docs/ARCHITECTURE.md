# Hybrid Transpiler Architecture

## Overview

The Hybrid Transpiler is designed as a multi-stage pipeline that converts C++ source code into equivalent Rust or Go code while preserving semantics and improving safety.

## Architecture Layers

### 1. Parser Layer (C++ → AST)

**Components:**
- Clang LibTooling integration
- AST visitor for C++ constructs
- Preprocessor handling
- Template instantiation

**Responsibilities:**
- Parse C++ source files using Clang's full parser
- Handle preprocessor directives
- Resolve template instantiations
- Extract complete type information

**Key Files:**
- `src/parser/ast_builder.cpp`
- `src/parser/type_resolver.cpp`

### 2. Intermediate Representation (IR)

**Components:**
- Type system representation
- Control flow graph
- Ownership analysis
- Safety verification

**Responsibilities:**
- Represent C++ constructs in a language-neutral format
- Perform ownership and lifetime analysis
- Identify memory management patterns (RAII, smart pointers)
- Validate safety properties

**Key Files:**
- `include/ir.h`
- `src/ir/ir_builder.cpp`
- `src/ir/type_system.cpp`
- `src/ir/ownership_analyzer.cpp`

### 3. Code Generation Layer (IR → Target Language)

**Components:**
- Rust code generator
- Go code generator
- Type mapping system
- Idiom transformation

**Responsibilities:**
- Convert IR to target language syntax
- Map type systems appropriately
- Generate idiomatic code
- Preserve comments and documentation

**Key Files:**
- `src/codegen/rust/rust_codegen.cpp`
- `src/codegen/go/go_codegen.cpp`
- `src/parser/type_mapper.cpp`

## Data Flow

```
C++ Source
    ↓
[Clang Parser]
    ↓
C++ AST
    ↓
[AST Visitor]
    ↓
Intermediate Representation (IR)
    ↓
[Ownership Analyzer]
    ↓
Annotated IR
    ↓
┌───────────┴────────────┐
↓                        ↓
[Rust Codegen]    [Go Codegen]
↓                        ↓
Rust Code          Go Code
```

## Type System Mapping

### C++ → Rust

| C++ Type | Rust Type | Notes |
|----------|-----------|-------|
| `int` | `i32` | Default integer mapping |
| `unsigned int` | `u32` | Unsigned variants |
| `float` | `f32` | Floating point |
| `T*` | `*const T` or `*mut T` | Raw pointers (unsafe) |
| `T&` | `&T` | Immutable reference |
| `T&` (non-const) | `&mut T` | Mutable reference |
| `unique_ptr<T>` | `Box<T>` | Unique ownership |
| `shared_ptr<T>` | `Rc<T>` or `Arc<T>` | Shared ownership |
| `vector<T>` | `Vec<T>` | Dynamic array |
| `string` | `String` | Owned string |

### C++ → Go

| C++ Type | Go Type | Notes |
|----------|---------|-------|
| `int` | `int32` | Fixed-size integer |
| `unsigned int` | `uint32` | Unsigned variant |
| `float` | `float32` | Floating point |
| `T*` | `*T` | Pointer (GC managed) |
| `T&` | `*T` | Reference becomes pointer |
| `unique_ptr<T>` | `*T` | GC handles lifetime |
| `shared_ptr<T>` | `*T` | GC handles sharing |
| `vector<T>` | `[]T` | Slice |
| `string` | `string` | Built-in string |

## Ownership Analysis

### Patterns Detected

1. **Unique Ownership**
   - `unique_ptr`
   - Move semantics
   - → Rust: `Box<T>`, Go: `*T`

2. **Shared Ownership**
   - `shared_ptr`
   - Reference counting
   - → Rust: `Rc<T>` or `Arc<T>`, Go: `*T`

3. **Borrowed References**
   - `const T&`
   - Non-owning access
   - → Rust: `&T`, Go: `*T`

4. **Mutable Borrows**
   - `T&` (non-const)
   - Exclusive mutable access
   - → Rust: `&mut T`, Go: `*T`

### Lifetime Analysis

The analyzer tracks:
- Variable declarations and initializations
- Function parameters and return values
- Object construction and destruction
- Resource acquisition and release (RAII)

## Memory Safety Features

### 1. Smart Pointer Recognition

Detects and converts C++ smart pointers:
```cpp
// C++
std::unique_ptr<Data> data = std::make_unique<Data>();

// → Rust
let data: Box<Data> = Box::new(Data::new());

// → Go
data := new(Data)
```

### 2. RAII Pattern Conversion

```cpp
// C++
class Resource {
    ~Resource() { cleanup(); }
};

// → Rust
impl Drop for Resource {
    fn drop(&mut self) {
        self.cleanup();
    }
}

// → Go
// Use defer in constructor or explicit cleanup
```

### 3. Reference Safety

Ensures references don't outlive their referents:
```cpp
// C++ (unsafe pattern detected)
int& getDanglingRef() {
    int x = 10;
    return x;  // WARNING: dangling reference
}

// → Rust (compile error, prevents generation)
// → Go (returns copy or pointer to heap-allocated value)
```

## Code Generation Strategies

### Idiomatic Transformations

**Rust:**
- Convert getters/setters to field access where appropriate
- Use `Result<T, E>` for error handling
- Implement standard traits (`Debug`, `Clone`, etc.)
- Follow naming conventions (snake_case)

**Go:**
- Export public members (capitalize)
- Use error return values
- Implement standard interfaces
- Follow Go naming conventions

### Comment Preservation

- Doc comments → Rust doc comments (`///`)
- Doc comments → Go doc comments (`//`)
- Inline comments preserved where possible

## Optimization Levels

- **Level 0**: Direct translation, preserve structure
- **Level 1**: Basic idiom transformations
- **Level 2**: Advanced optimizations, smart container conversions
- **Level 3**: Aggressive optimizations, may restructure code

## Extension Points

The architecture supports extensions:
1. Custom type mappers
2. Additional target languages
3. Custom analysis passes
4. Code formatting plugins

## Future Enhancements

1. **Template Metaprogramming**
   - Full template expansion
   - Const generic support

2. **Async/Await**
   - Convert C++ coroutines
   - Map to Rust async/await or Go goroutines

3. **Macro Expansion**
   - Handle complex C++ macros
   - Generate equivalent code in target

4. **FFI Generation**
   - Create C bindings
   - Enable gradual migration
