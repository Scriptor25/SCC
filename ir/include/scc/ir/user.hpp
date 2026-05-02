#pragma once

#include <scc/ir/value.hpp>

namespace scc::ir
{
    class User : public Value
    {
    public:
        explicit User(Type *type);

        virtual void DropAll();
        virtual void Replace(Value *value, Value *with);
    };
}
