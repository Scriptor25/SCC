#pragma once

#include <memory>

namespace scc::ir
{
    class Module;
    class Context;
    class Builder;

    class Type;
    using TypePtr = std::shared_ptr<Type>;

    class Value;
    using ValuePtr = std::shared_ptr<Value>;

    struct TypeHash final
    {
        unsigned operator()(const TypePtr &) const noexcept;
    };

    bool operator==(const TypePtr &, const TypePtr &);

    unsigned CombineHash(unsigned a, unsigned b);
}
