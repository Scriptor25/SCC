#include <scc/ir/type.hpp>

scc::ir::FunctionType::FunctionType(
    Context &context,
    TypeFwd::Ptr result,
    std::vector<TypeFwd::Ptr> arguments,
    const bool variadic)
    : Type(context, Kind_Function),
      m_Result(std::move(result)),
      m_Arguments(std::move(arguments)),
      m_Variadic(variadic)
{
}

unsigned scc::ir::FunctionType::GenerateHash() const
{
    auto hash = m_Result->GenerateHash();
    for (auto &argument : m_Arguments)
    {
        hash = CombineHash(hash, argument->GenerateHash());
    }
    hash = CombineHash(hash, m_Variadic ? 0xDEADBEEFu : 0xBAADF00Du);
    return CombineHash(7, hash);
}

bool scc::ir::FunctionType::Equals(const TypeFwd::Ptr &type) const
{
    if (type->GetKind() != Kind_Function)
    {
        return false;
    }

    if (const auto p = std::dynamic_pointer_cast<FunctionType>(type))
    {
        if (m_Result != p->m_Result)
        {
            return false;
        }

        if (m_Variadic != p->m_Variadic)
        {
            return false;
        }

        if (m_Arguments.size() != p->m_Arguments.size())
        {
            return false;
        }

        for (unsigned i = 0; i < m_Arguments.size(); ++i)
        {
            if (m_Arguments.at(i) != p->m_Arguments.at(i))
            {
                return false;
            }
        }

        return true;
    }

    return false;
}

std::ostream &scc::ir::FunctionType::Print(std::ostream &stream) const
{
    m_Result->Print(stream) << '(';
    for (auto i = m_Arguments.begin(); i != m_Arguments.end(); ++i)
    {
        if (i != m_Arguments.begin())
        {
            stream << ',';
        }
        (*i)->Print(stream);
    }
    if (m_Variadic)
    {
        if (!m_Arguments.empty())
        {
            stream << ',';
        }
        stream << "...";
    }
    return stream << ')';
}

scc::ir::TypeFwd::Ptr scc::ir::FunctionType::GetResult() const
{
    return m_Result;
}

unsigned scc::ir::FunctionType::GetArgumentCount() const
{
    return m_Arguments.size();
}

scc::ir::TypeFwd::Ptr scc::ir::FunctionType::GetArgument(const unsigned index) const
{
    return m_Arguments.at(index);
}

std::vector<scc::ir::TypeFwd::Ptr>::const_iterator scc::ir::FunctionType::begin() const
{
    return m_Arguments.begin();
}

std::vector<scc::ir::TypeFwd::Ptr>::const_iterator scc::ir::FunctionType::end() const
{
    return m_Arguments.end();
}

bool scc::ir::FunctionType::IsVariadic() const
{
    return m_Variadic;
}
