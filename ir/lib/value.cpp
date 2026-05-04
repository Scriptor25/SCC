#include <scc/ir/user.hpp>
#include <scc/ir/value.hpp>

scc::ir::Value::Value(Type *type)
    : m_Type(type)
{
}

scc::ir::Value::~Value()
{
    ReplaceWith(nullptr);
}

void scc::ir::Value::Use(User *user)
{
    m_Uses.insert(user);
}

void scc::ir::Value::Drop(User *user)
{
    m_Uses.erase(user);
}

void scc::ir::Value::ReplaceWith(Value *with)
{
    for (const auto copy = m_Uses; const auto user : copy)
        user->Replace(this, with);
}

bool scc::ir::Value::IsUsed() const
{
    return !m_Uses.empty();
}

size_t scc::ir::Value::GetUseCount() const
{
    return m_Uses.size();
}

scc::ir::Type *scc::ir::Value::GetType() const
{
    return m_Type;
}

std::ostream &scc::ir::operator<<(std::ostream &stream, const Value *value)
{
    return value->PrintOperand(stream);
}
