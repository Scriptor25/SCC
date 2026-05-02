#pragma once

#include <compare>
#include <type_traits>

namespace scc::ir
{
    using float32_t = std::conditional_t<sizeof(float) == 4, float, void>;
    using float64_t = std::conditional_t<sizeof(double) == 8, double, void>;

    struct Platform;

    class Module;
    class Context;

    class Builder;

    class Parser;

    class Type;
    class VoidType;
    class IntType;
    class FloatType;
    class PointerType;
    class ArrayType;
    class VectorType;
    class StructType;
    class FunctionType;

    class Value;

    class Argument;

    class Global;
    class Variable;
    class Function;

    class Constant;
    class ConstantInt;
    class ConstantFloat;
    class ConstantArray;
    class ConstantVector;
    class ConstantStruct;

    class Instruction;

    class Block;

    class User;

    unsigned AlignTo(unsigned value, unsigned align);
}
