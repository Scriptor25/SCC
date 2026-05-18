#include <scc/as/instruction.hpp>

#include <scc/platform.hpp>

scc::as::Instruction::Instruction(
    const Platform &platform,
    const Mnemonic mnemonic,
    std::vector<std::unique_ptr<Operand>> operands)
    : m_Platform(platform),
      m_Mnemonic(mnemonic),
      m_Operands(std::move(operands))
{
}

void scc::as::Instruction::SetMnemonic(const Mnemonic mnemonic)
{
    m_Mnemonic = mnemonic;
}

scc::Mnemonic scc::as::Instruction::GetMnemonic() const
{
    return m_Mnemonic;
}

size_t scc::as::Instruction::GetOperandCount() const
{
    return m_Operands.size();
}

scc::as::Operand &scc::as::Instruction::operator[](const size_t index)
{
    return *m_Operands[index];
}

const scc::as::Operand &scc::as::Instruction::operator[](const size_t index) const
{
    return *m_Operands[index];
}

scc::as::Instruction::iterator<false> scc::as::Instruction::begin()
{
    return { size_t(), m_Operands };
}

scc::as::Instruction::iterator<false> scc::as::Instruction::end()
{
    return { ~size_t(), m_Operands };
}

scc::as::Instruction::iterator<true> scc::as::Instruction::begin() const
{
    return { size_t(), m_Operands };
}

scc::as::Instruction::iterator<true> scc::as::Instruction::end() const
{
    return { ~size_t(), m_Operands };
}

std::ostream &scc::as::Instruction::Print(std::ostream &stream) const
{
    if (m_Operands.empty())
        return stream << m_Platform.ISA.Mnemonics.at(m_Mnemonic).Name;

    stream << m_Platform.ISA.Mnemonics.at(m_Mnemonic).Name << ' ';

    for (auto it = m_Operands.begin(); it != m_Operands.end(); ++it)
    {
        if (it != m_Operands.begin())
            stream << ", ";

        (*it)->Print(stream);
    }

    return stream;
}
