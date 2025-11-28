#include "codegen.h"

namespace hybrid {

void CodeGenerator::writeLine(const std::string& line) {
    if (!line.empty()) {
        writeIndent();
        output_ << line;
    }
    output_ << "\n";
}

void CodeGenerator::writeIndent() {
    for (int i = 0; i < indent_level_; ++i) {
        output_ << "    "; // 4 spaces per indent level
    }
}

} // namespace hybrid
