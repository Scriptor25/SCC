#include <scc/as/fragment.hpp>
#include <scc/as/operand.hpp>
#include <scc/as/symbol.hpp>

scc::as::SymbolOperand::SymbolOperand(const Platform &platform, Symbol *symbol)
    : Operand(platform),
      m_Symbol(symbol)
{
}

std::ostream &scc::as::SymbolOperand::Print(std::ostream &stream) const
{
    return PrintSelector(stream) << m_Symbol->GetName();
}

scc::as::Symbol *scc::as::SymbolOperand::GetSymbol() const
{
    return m_Symbol;
}
