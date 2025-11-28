# Hybrid Transpiler User Guide

## Table of Contents

1. [Installation](#installation)
2. [Basic Usage](#basic-usage)
3. [Command-Line Options](#command-line-options)
4. [Examples](#examples)
5. [Best Practices](#best-practices)
6. [Troubleshooting](#troubleshooting)

## Installation

### Prerequisites

Ensure you have the following installed:

```bash
# LLVM and Clang (version 15 or later)
sudo apt-get install llvm-15-dev clang-15 libclang-15-dev

# CMake (version 3.15 or later)
sudo apt-get install cmake

# Build tools
sudo apt-get install build-essential

# Optional: Rust for testing generated code
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh

# Optional: Go for testing generated code
sudo apt-get install golang-go
```

### Building from Source

```bash
git clone https://github.com/yourusername/hybrid-transpiler.git
cd hybrid-transpiler

mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)

# Optional: Install system-wide
sudo make install
```

### Verify Installation

```bash
hybrid-transpiler --version
```

## Basic Usage

### Simple Transpilation

Convert a C++ file to Rust:

```bash
hybrid-transpiler -i myclass.cpp -o myclass.rs -t rust
```

Convert a C++ file to Go:

```bash
hybrid-transpiler -i myclass.cpp -o myclass.go -t go
```

### Auto-Generated Output Names

If you don't specify an output file, it will be generated automatically:

```bash
# Generates myclass.rs
hybrid-transpiler -i myclass.cpp -t rust

# Generates myclass.go
hybrid-transpiler -i myclass.cpp -t go
```

## Command-Line Options

### Input/Output Options

| Option | Description | Example |
|--------|-------------|---------|
| `-i, --input <file>` | Input C++ source file | `-i main.cpp` |
| `-o, --output <file>` | Output file path | `-o main.rs` |

### Target Language

| Option | Description | Default |
|--------|-------------|---------|
| `-t, --target <lang>` | Target language (rust\|go) | rust |

**Examples:**
```bash
hybrid-transpiler -i app.cpp -t rust
hybrid-transpiler -i app.cpp -t go
```

### Optimization Levels

| Level | Description |
|-------|-------------|
| `-O0` | No optimization, direct translation |
| `-O1` | Basic idiom transformations |
| `-O2` | Advanced optimizations, smart conversions |
| `-O3` | Aggressive optimizations, may restructure |

**Examples:**
```bash
# No optimization
hybrid-transpiler -i app.cpp -O0

# Balanced optimization
hybrid-transpiler -i app.cpp -O2
```

### Additional Options

| Option | Description |
|--------|-------------|
| `--no-safety-checks` | Disable safety analysis |
| `--no-comments` | Don't preserve comments |
| `--gen-tests` | Generate test cases |
| `-h, --help` | Show help message |
| `-v, --version` | Show version info |

## Examples

### Example 1: Simple Class Translation

**Input (point.cpp):**
```cpp
class Point {
private:
    int x, y;
public:
    Point(int x, int y) : x(x), y(y) {}
    int getX() const { return x; }
    int getY() const { return y; }
};
```

**Command:**
```bash
hybrid-transpiler -i point.cpp -o point.rs -t rust
```

**Output (point.rs):**
```rust
pub struct Point {
    x: i32,
    y: i32,
}

impl Point {
    pub fn new(x: i32, y: i32) -> Self {
        Self { x, y }
    }

    pub fn get_x(&self) -> i32 {
        self.x
    }

    pub fn get_y(&self) -> i32 {
        self.y
    }
}
```

### Example 2: Smart Pointers to Rust

**Input (resource.cpp):**
```cpp
#include <memory>

class Resource {
    std::unique_ptr<int[]> data;
public:
    Resource(size_t size)
        : data(std::make_unique<int[]>(size)) {}
};
```

**Command:**
```bash
hybrid-transpiler -i resource.cpp -t rust -O2
```

**Output (resource.rs):**
```rust
pub struct Resource {
    data: Vec<i32>,
}

impl Resource {
    pub fn new(size: usize) -> Self {
        Self {
            data: vec![0; size],
        }
    }
}
```

### Example 3: Converting to Go

**Input (data.cpp):**
```cpp
class Data {
    int value;
public:
    Data(int v) : value(v) {}
    int getValue() const { return value; }
    void setValue(int v) { value = v; }
};
```

**Command:**
```bash
hybrid-transpiler -i data.cpp -t go
```

**Output (data.go):**
```go
package main

type Data struct {
    Value int32
}

func NewData(v int32) *Data {
    return &Data{Value: v}
}

func (d *Data) GetValue() int32 {
    return d.Value
}

func (d *Data) SetValue(v int32) {
    d.Value = v
}
```

## Best Practices

### 1. Start with Modern C++

The transpiler works best with modern C++ code that uses:
- Smart pointers (`unique_ptr`, `shared_ptr`)
- RAII patterns
- `const` correctness
- Standard library containers

### 2. Avoid Problematic Patterns

Some C++ patterns don't translate well:
- Raw pointer arithmetic
- `goto` statements
- Complex macro metaprogramming
- Multiple inheritance (limited support)

### 3. Review Generated Code

Always review the generated code:
```bash
# Generate code
hybrid-transpiler -i mycode.cpp -o mycode.rs -t rust

# Review and test
cat mycode.rs
cargo build
cargo test
```

### 4. Incremental Migration

For large codebases:
1. Start with small, self-contained modules
2. Test each conversion thoroughly
3. Use FFI for gradual integration
4. Migrate incrementally

### 5. Optimize Appropriately

Choose optimization level based on your needs:
- `-O0`: Initial conversion, easy to verify
- `-O1`: Production use, idiomatic code
- `-O2`: Performance-critical code
- `-O3`: Experimental, verify thoroughly

## Troubleshooting

### Build Errors

**Problem:** `clang not found`

**Solution:**
```bash
# Install clang development libraries
sudo apt-get install llvm-dev clang libclang-dev

# Or specify LLVM path
cmake -DLLVM_DIR=/path/to/llvm/lib/cmake/llvm ..
```

**Problem:** `CMake cannot find LLVM`

**Solution:**
```bash
# Find LLVM installation
llvm-config --prefix

# Set LLVM_DIR
export LLVM_DIR=/usr/lib/llvm-15/lib/cmake/llvm
cmake ..
```

### Transpilation Issues

**Problem:** Output file is empty or incomplete

**Causes:**
- Unsupported C++ features
- Parse errors in input file
- Missing dependencies

**Solution:**
```bash
# Check input file syntax
clang++ -fsyntax-only myfile.cpp

# Enable verbose output (future feature)
hybrid-transpiler -i myfile.cpp --verbose
```

**Problem:** Generated code doesn't compile

**Causes:**
- Template instantiation issues
- Type mapping edge cases
- Unsupported constructs

**Solution:**
1. Review the generated code
2. Check for TODO comments
3. Manually adjust if needed
4. Report issue with minimal example

### Runtime Errors

**Problem:** Transpiled code has different behavior

**Causes:**
- Undefined behavior in original code
- Ownership/lifetime issues
- Integer overflow differences

**Solution:**
1. Enable safety checks: `--enable-safety-checks`
2. Run with sanitizers
3. Add assertions to verify behavior

## Getting Help

### Documentation

- Architecture: [ARCHITECTURE.md](ARCHITECTURE.md)
- API Reference: Generated by Doxygen
- Examples: `/examples` directory

### Community Support

- GitHub Issues: Report bugs and feature requests
- Discussion Board: Ask questions
- Discord: Real-time community support

### Reporting Bugs

When reporting issues, include:

1. Hybrid Transpiler version: `hybrid-transpiler --version`
2. LLVM/Clang version: `clang --version`
3. Minimal C++ example that reproduces the issue
4. Expected vs actual output
5. Command line used

**Template:**
```
**Version:** hybrid-transpiler 0.1.0, clang 15.0.0

**Command:**
hybrid-transpiler -i test.cpp -o test.rs -t rust

**Input (test.cpp):**
[minimal example]

**Expected Output:**
[what you expected]

**Actual Output:**
[what you got]

**Additional Context:**
[any other relevant info]
```

## Advanced Features

### Batch Processing

```bash
# Process multiple files (future feature)
for file in src/*.cpp; do
    hybrid-transpiler -i "$file" -t rust
done
```

### CI/CD Integration

```yaml
# GitHub Actions example
- name: Transpile C++ to Rust
  run: |
    hybrid-transpiler -i src/legacy.cpp -o src/modern.rs -t rust
    cargo build
    cargo test
```

### Custom Type Mappings

Create a configuration file (future feature):

```json
{
  "type_mappings": {
    "MyCustomInt": "i64",
    "MyString": "String"
  }
}
```

## Performance Tips

1. **Large Files:** Consider splitting into smaller modules
2. **Templates:** Pre-instantiate common types
3. **Parallel Processing:** Use build system parallelization
4. **Caching:** Transpiler caches parsed results (future)

## Next Steps

- Read [ARCHITECTURE.md](ARCHITECTURE.md) for internals
- Explore `/examples` for more patterns
- Check GitHub for latest updates
- Join community discussions
