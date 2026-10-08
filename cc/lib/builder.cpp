#include <scc/cc/builder.hpp>

scc::cc::Builder::Builder(
    Context &context,
    ir::Context &ir_context,
    ir::Module &ir_module,
    ir::Builder &ir_builder)
    : m_Context(context),
      m_IRContext(ir_context),
      m_IRModule(ir_module),
      m_IRBuilder(ir_builder)
{
    m_Stack.push_back(
        {
            .Head = nullptr,
            .Tail = nullptr,
            .Named = {},
        });
}

scc::cc::Context &scc::cc::Builder::GetContext() const
{
    return m_Context;
}

scc::ir::Context &scc::cc::Builder::GetIRContext() const
{
    return m_IRContext;
}

scc::ir::Module &scc::cc::Builder::GetIRModule() const
{
    return m_IRModule;
}

scc::ir::Builder &scc::cc::Builder::GetIRBuilder() const
{
    return m_IRBuilder;
}

scc::cc::Value *scc::cc::Builder::Manage(ValuePtr value)
{
    auto *ptr = value.get();
    m_Stack.back().Values.push_back(std::move(value));
    return ptr;
}

void scc::cc::Builder::PushFrame(ir::Block *head, ir::Block *tail)
{
    m_Stack.push_back(
        {
            .Depth = m_Stack.size(),
            .Head = head,
            .Tail = tail,
            .Named = {},
        });
}

void scc::cc::Builder::PopFrame()
{
    m_Stack.pop_back();
}

scc::ir::Block *scc::cc::Builder::GetHead() const
{
    for (auto i = m_Stack.back().Depth; i > 0; --i)
        if (auto &frame = m_Stack[i - 1]; frame.Head)
            return frame.Head;
    return nullptr;
}

scc::ir::Block *scc::cc::Builder::GetTail() const
{
    for (auto i = m_Stack.back().Depth; i > 0; --i)
        if (auto &frame = m_Stack[i - 1]; frame.Tail)
            return frame.Tail;
    return nullptr;
}

void scc::cc::Builder::SetNamed(const std::string &name, ValuePtr value)
{
    auto &frame = m_Stack.back();
    if (frame.Named.contains(name))
        Error("duplicate named value {} in frame", name);

    frame.Named[name] = std::move(value);
}

scc::cc::Value *scc::cc::Builder::GetNamed(const std::string &name) const
{
    for (auto i = m_Stack.back().Depth; i > 0; --i)
    {
        auto &frame = m_Stack[i - 1];
        if (const auto it = frame.Named.find(name); it != frame.Named.end())
            return it->second.get();
    }
    return nullptr;
}
