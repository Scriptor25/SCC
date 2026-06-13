#include <scc/ir/lowering.hpp>
#include <scc/ir/user.hpp>
#include <scc/ir/value.hpp>

#include <scc/assert.hpp>
#include <scc/platform.hpp>

scc::ir::Value::Value(Type *type)
    : m_Type(type)
{
}

scc::ir::Value::~Value()
{
    ReplaceWith({});
}

std::ostream &scc::ir::Value::PrintOperandAssembly(
    std::ostream &stream,
    LoweringContext &context,
    const bool address) const
{
    if (const auto it = context.RegisterAssignment.find(this); it != context.RegisterAssignment.end())
    {
        auto *view = context.TargetPlatform.ISA.FindRegisterView(it->second);

        Assert(view, "view must not be null");

        return stream << '%' << view->CanonicalName();
    }

    if (const auto it = context.StackOffset.find(this); it != context.StackOffset.end())
    {
        // TODO: get frame base pointer register for target platform
        // TODO: build memory operand: `-<offset>(%<base pointer register>)`
        // TODO: print memory operand

        Error("TODO");
    }

    Error("value not instantiated");
}

void scc::ir::Value::Use(User *user)
{
    m_Uses.insert(user);
}

void scc::ir::Value::Drop(User *user)
{
    if (const auto it = m_Uses.find(user); it != m_Uses.end())
        m_Uses.erase(it);
}

void scc::ir::Value::ReplaceWith(Value *with)
{
    for (const auto copy = m_Uses; auto *user : copy)
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
    return value->PrintOperand(stream, true);
}
