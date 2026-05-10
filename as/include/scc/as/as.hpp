#pragma once

#include <cstdint>

namespace scc::as
{
    using Immediate = uint64_t;

    enum class OperandType;

    class Parser;
    class Writer;

    class Module;
    class Section;

    class Fragment;

    class Instruction;
    class Data;
    class Align;
    class Fill;

    class Operand;

    class ImmediateOperand;
    class RegisterOperand;
    class ReferenceOperand;
    class SymbolOperand;

    class Symbol;
}
