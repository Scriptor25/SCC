#pragma once

#include <memory>

namespace scc::ir
{
    class Parser;

    class Module;
    class Context;
    class Builder;

    class Type;
    using TypePtr = std::shared_ptr<Type>;

    class Value;
    using ValuePtr = std::shared_ptr<Value>;

    class NamedValue;
    using NamedValuePtr = std::shared_ptr<NamedValue>;

    class Global;
    using GlobalPtr = std::shared_ptr<Global>;

    class Constant;
    using ConstantPtr = std::shared_ptr<Constant>;

    class Block;
    using BlockPtr = std::shared_ptr<Block>;

    class Instruction;
    using InstructionPtr = std::shared_ptr<Instruction>;

    struct TypeHash final
    {
        unsigned operator()(const TypePtr &) const noexcept;
    };

    bool operator==(const TypePtr &, const TypePtr &);

    unsigned CombineHash(unsigned a, unsigned b);

    template<typename C>
    class Shared
    {
    public:
        using Ptr = std::shared_ptr<C>;
        using ConstPtr = std::shared_ptr<const C>;
        using WeakPtr = std::weak_ptr<C>;
        using ConstWeakPtr = std::weak_ptr<const C>;
    };
}
