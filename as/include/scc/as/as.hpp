#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

namespace scc::as
{
    using Immediate = uint64_t;

    class Parser;
    class Writer;

    class Module;
    class Section;

    class Fragment;

    class Instruction;
    using InstructionPtr = std::unique_ptr<Instruction>;

    class Data;
    class Align;
    class Fill;

    class Operand;
    using OperandPtr = std::unique_ptr<Operand>;

    class ImmediateOperand;
    class StringOperand;
    class RegisterOperand;
    class ReferenceOperand;
    class SymbolOperand;

    class Symbol;

    struct FixupTableEntry
    {
        size_t Offset;
        size_t Size;
        size_t Subtract;
    };

    using SymbolTable = std::unordered_map<const Symbol *, size_t>;
    using FixupTable = std::unordered_map<const Symbol *, std::vector<FixupTableEntry>>;
}
