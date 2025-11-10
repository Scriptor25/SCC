#pragma once

#include <memory>

namespace scc::ir
{
    template<typename C>
    class Shared
    {
    public:
        using Ptr = std::shared_ptr<C>;
        using WeakPtr = std::weak_ptr<C>;
    };

    class Parser;

    class Module;
    class Context;
    class Builder;

    class Type;
    using TypeFwd = Shared<Type>;

    class Value;
    using ValueFwd = Shared<Value>;

    class IdentifiedValue;
    using IdentifiedValueFwd = Shared<IdentifiedValue>;

    class Global;
    using GlobalFwd = Shared<Global>;

    class Constant;
    using ConstantFwd = Shared<Constant>;

    class Block;
    using BlockFwd = Shared<Block>;

    class Instruction;
    using InstructionFwd = Shared<Instruction>;

    class Register;
    using RegisterFwd = Shared<Register>;

    class Usable;
    using UsableFwd = Shared<Usable>;

    struct TypeHash final
    {
        unsigned operator()(const TypeFwd::Ptr &) const noexcept;
    };

    bool operator==(const TypeFwd::Ptr &, const TypeFwd::Ptr &);

    unsigned CombineHash(unsigned a, unsigned b);
}
