#include <scc/cc/builder.hpp>

scc::cc::Builder::Builder(
    ir::Context &context,
    ir::Module &module,
    ir::Builder &builder)
    : m_Context(context),
      m_Module(module),
      m_Builder(builder)
{
    m_Stack.push_back(
        {
            .Head = nullptr,
            .Tail = nullptr,
            .Named = {},
        });
}

scc::ir::Context &scc::cc::Builder::GetContext() const
{
    return m_Context;
}

scc::ir::Module &scc::cc::Builder::GetModule() const
{
    return m_Module;
}

scc::ir::Builder &scc::cc::Builder::GetBuilder() const
{
    return m_Builder;
}

scc::cc::Value *scc::cc::Builder::Manage(ValuePtr value)
{
    auto *ptr = value.get();
    m_Stack.back().Values.push_back(std::move(value));
    return ptr;
}

void scc::cc::Builder::PushFrame(ir::Block *head, ir::Block *tail)
{
    std::unordered_map<std::string, ValuePtr> named;
    if (!m_Stack.empty())
    {
        const auto &frame = m_Stack.back();

        if (!head)
            head = frame.Head;
        if (!tail)
            tail = frame.Tail;

        named = frame.Named;
    }

    m_Stack.push_back(
        {
            .Head = head,
            .Tail = tail,
            .Named = std::move(named),
        });
}

void scc::cc::Builder::PopFrame()
{
    m_Stack.pop_back();
}

scc::ir::Block *scc::cc::Builder::GetHead() const
{
    return m_Stack.back().Head;
}

scc::ir::Block *scc::cc::Builder::GetTail() const
{
    return m_Stack.back().Tail;
}

void scc::cc::Builder::SetNamed(const std::string &name, ValuePtr value)
{
    m_Stack.back().Named[name] = std::move(value);
}

const scc::cc::Value *scc::cc::Builder::GetNamed(const std::string &name) const
{
    if (const auto it = m_Stack.back().Named.find(name); it != m_Stack.back().Named.end())
        return it->second.get();
    return nullptr;
}
