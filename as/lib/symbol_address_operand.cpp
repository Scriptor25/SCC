#include <scc/as/fragment.hpp>
#include <scc/as/operand.hpp>
#include <scc/as/symbol.hpp>

scc::as::SymbolAddressOperand::SymbolAddressOperand(const Platform &platform, Symbol *symbol)
    : Operand(platform),
      m_Symbol(symbol)
{
}

scc::as::Immediate scc::as::SymbolAddressOperand::GetImmediate() const
{
    return m_Symbol->GetAddress();
}

std::ostream &scc::as::SymbolAddressOperand::Print(std::ostream &stream) const
{
    return PrintSelector(stream) << '$' << m_Symbol->GetName();
}

scc::as::Symbol *scc::as::SymbolAddressOperand::GetSymbol() const
{
    return m_Symbol;
}
