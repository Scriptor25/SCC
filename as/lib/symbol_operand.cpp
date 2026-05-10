#include <scc/as/operand.hpp>
#include <scc/as/symbol.hpp>

#include <ostream>

scc::as::SymbolOperand::SymbolOperand(Symbol *symbol)
    : m_Symbol(symbol)
{
}

std::ostream &scc::as::SymbolOperand::Print(std::ostream &stream) const
{
    return stream << m_Symbol->GetName();
}
