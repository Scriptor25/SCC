#include <scc/ir/type.hpp>

scc::ir::FunctionType::FunctionType(
    Context &context,
    Type *result,
    std::vector<Type *> arguments,
    const bool variadic)
    : Type(context, Kind::Function),
      m_Result(result),
      m_Arguments(std::move(arguments)),
      m_Variadic(variadic)
{
}

bool scc::ir::FunctionType::Compare(Type *type) const
{
    if (const auto *function_type = dynamic_cast<FunctionType *>(type))
    {
        if (m_Result != function_type->m_Result)
            return false;

        if (m_Variadic != function_type->m_Variadic)
            return false;

        if (m_Arguments.size() != function_type->m_Arguments.size())
            return false;

        for (size_t i = 0; i < m_Arguments.size(); ++i)
            if (m_Arguments[i] != function_type->m_Arguments[i])
                return false;

        return true;
    }

    return false;
}

size_t scc::ir::FunctionType::GetSize() const
{
    return 0u;
}

size_t scc::ir::FunctionType::GetAlign() const
{
    return 1u;
}

bool scc::ir::FunctionType::IsElement() const
{
    return false;
}

std::ostream &scc::ir::FunctionType::Print(std::ostream &stream) const
{
    m_Result->Print(stream) << '(';

    for (auto i = m_Arguments.begin(); i != m_Arguments.end(); ++i)
    {
        if (i != m_Arguments.begin())
            stream << ',';

        (*i)->Print(stream);
    }

    if (m_Variadic)
    {
        if (!m_Arguments.empty())
            stream << ',';

        stream << "...";
    }

    return stream << ')';
}

scc::ir::Type *scc::ir::FunctionType::GetResult() const
{
    return m_Result;
}

size_t scc::ir::FunctionType::GetArgumentCount() const
{
    return m_Arguments.size();
}

scc::ir::Type *scc::ir::FunctionType::GetArgument(size_t index) const
{
    return m_Arguments[index];
}

std::vector<scc::ir::Type *>::const_iterator scc::ir::FunctionType::begin() const
{
    return m_Arguments.begin();
}

std::vector<scc::ir::Type *>::const_iterator scc::ir::FunctionType::end() const
{
    return m_Arguments.end();
}

bool scc::ir::FunctionType::IsVariadic() const
{
    return m_Variadic;
}
