#include <scc/ir/user.hpp>

#include <scc/error.hpp>

scc::ir::User::User(Type *type)
    : Value(type)
{
}

void scc::ir::User::DropAll()
{
}

void scc::ir::User::Replace(Value *value, Value *with)
{
    Error("user does not use value '{}'", value);
}
