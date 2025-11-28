# Quick Start Guide - Hybrid Transpiler

Get started with the Hybrid Transpiler in 5 minutes!

## Installation

```bash
# Clone the repository
git clone https://github.com/yourusername/hybrid-transpiler.git
cd hybrid-transpiler

# Install dependencies (Ubuntu/Debian)
sudo apt-get update
sudo apt-get install -y llvm-15-dev clang-15 libclang-15-dev cmake build-essential

# Build
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)

# Optionally install system-wide
sudo make install
```

## Your First Transpilation

### 1. Create a simple C++ file

Create `hello.cpp`:

```cpp
#include <iostream>
#include <string>

class Greeter {
private:
    std::string name;

public:
    Greeter(const std::string& n) : name(n) {}

    void greet() const {
        std::cout << "Hello, " << name << "!" << std::endl;
    }
};

int main() {
    Greeter greeter("World");
    greeter.greet();
    return 0;
}
```

### 2. Transpile to Rust

```bash
# From the build directory
./hybrid-transpiler -i ../hello.cpp -o hello.rs -t rust

# The generated hello.rs will look like:
cat hello.rs
```

**Expected Output:**
```rust
pub struct Greeter {
    name: String,
}

impl Greeter {
    pub fn new(n: &str) -> Self {
        Self {
            name: n.to_string(),
        }
    }

    pub fn greet(&self) {
        println!("Hello, {}!", self.name);
    }
}

fn main() {
    let greeter = Greeter::new("World");
    greeter.greet();
}
```

### 3. Test the Rust code

```bash
# Compile and run
rustc hello.rs && ./hello
# Output: Hello, World!
```

### 4. Transpile to Go

```bash
./hybrid-transpiler -i ../hello.cpp -o hello.go -t go
cat hello.go
```

**Expected Output:**
```go
package main

import "fmt"

type Greeter struct {
    Name string
}

func NewGreeter(n string) *Greeter {
    return &Greeter{Name: n}
}

func (g *Greeter) Greet() {
    fmt.Printf("Hello, %s!\n", g.Name)
}

func main() {
    greeter := NewGreeter("World")
    greeter.Greet()
}
```

### 5. Test the Go code

```bash
go run hello.go
# Output: Hello, World!
```

## Try More Examples

The repository includes several examples:

```bash
cd ../examples

# View the example C++ code
cat simple_class.cpp

# Transpile to Rust
../build/hybrid-transpiler -i simple_class.cpp -o output.rs -t rust

# Compare with expected output
diff output.rs expected_output.rs

# Transpile to Go
../build/hybrid-transpiler -i simple_class.cpp -o output.go -t go

# Compare with expected output
diff output.go expected_output.go
```

## Common Usage Patterns

### Basic Translation
```bash
hybrid-transpiler -i input.cpp -o output.rs -t rust
```

### With Optimization
```bash
hybrid-transpiler -i input.cpp -o output.rs -t rust -O2
```

### Auto-Generate Output Name
```bash
# Creates input.rs
hybrid-transpiler -i input.cpp -t rust
```

### Batch Processing
```bash
for file in src/*.cpp; do
    hybrid-transpiler -i "$file" -t rust
done
```

## Next Steps

1. **Read the Documentation**
   - [README.md](README.md) - Project overview
   - [ARCHITECTURE.md](docs/ARCHITECTURE.md) - Technical details
   - [USER_GUIDE.md](docs/USER_GUIDE.md) - Comprehensive guide

2. **Explore Examples**
   - `examples/simple_class.cpp` - Basic class conversion
   - `examples/expected_output.rs` - Rust output
   - `examples/expected_output.go` - Go output

3. **Run Tests**
   ```bash
   cd build
   make test
   ```

4. **Join the Community**
   - GitHub Issues: Report bugs
   - Discussions: Ask questions
   - Discord: Real-time support

## Troubleshooting

### "clang not found"
```bash
sudo apt-get install clang-15 llvm-15-dev
```

### "CMake cannot find LLVM"
```bash
export LLVM_DIR=/usr/lib/llvm-15/lib/cmake/llvm
cmake ..
```

### Generated code doesn't compile
- Check for TODO comments in generated code
- Review type mappings
- Report issue with minimal example

## Get Help

- **Documentation**: [docs/](docs/)
- **Examples**: [examples/](examples/)
- **Issues**: GitHub Issues
- **Support**: Community Discord

---

**Happy Transpiling!** 🚀
