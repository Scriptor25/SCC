#include <scc/ir/instruction.hpp>

#include <scc/assert.hpp>

#include <ostream>

const std::unordered_map<scc::ir::FCompare, std::string_view> scc::ir::FCompareToString
{
    { FCompare::OLT, "folt" },
    { FCompare::ULT, "fult" },
    { FCompare::OGT, "fogt" },
    { FCompare::UGT, "fugt" },
    { FCompare::OLE, "fole" },
    { FCompare::ULE, "fule" },
    { FCompare::OGE, "foge" },
    { FCompare::UGE, "fuge" },
    { FCompare::OEQ, "foeq" },
    { FCompare::UEQ, "fueq" },
    { FCompare::ONE, "fone" },
    { FCompare::UNE, "fune" },
};

const std::unordered_map<std::string_view, scc::ir::FCompare> scc::ir::StringToFCompare
{
    { "folt", FCompare::OLT },
    { "fult", FCompare::ULT },
    { "fogt", FCompare::OGT },
    { "fugt", FCompare::UGT },
    { "fole", FCompare::OLE },
    { "fule", FCompare::ULE },
    { "foge", FCompare::OGE },
    { "fuge", FCompare::UGE },
    { "foeq", FCompare::OEQ },
    { "fueq", FCompare::UEQ },
    { "fone", FCompare::ONE },
    { "fune", FCompare::UNE },
};

scc::ir::FCompareInstruction::FCompareInstruction(
    Type *type,
    Block *block,
    std::string name,
    const FCompare compare,
    Value *lhs,
    Value *rhs)
    : Instruction(type, block, std::move(name)),
      m_Compare(compare),
      m_LHS(lhs),
      m_RHS(rhs)
{
    m_LHS->Use(this);
    m_RHS->Use(this);
}

scc::ir::FCompareInstruction::~FCompareInstruction()
{
    DropAll();
}

void scc::ir::FCompareInstruction::DropAll()
{
    if (m_LHS)
    {
        m_LHS->Drop(this);
        m_LHS = {};
    }
    if (m_RHS)
    {
        m_RHS->Drop(this);
        m_RHS = {};
    }
}

void scc::ir::FCompareInstruction::Replace(Value *value, Value *with)
{
    if (m_LHS == value)
    {
        value->Drop(this);
        if (with)
            with->Use(this);

        m_LHS = with;
    }

    if (m_RHS == value)
    {
        value->Drop(this);
        if (with)
            with->Use(this);

        m_RHS = with;
    }
}

std::ostream &scc::ir::FCompareInstruction::Print(std::ostream &stream) const
{
    if (IsUsed())
        stream << '%' << m_Name << " = ";

    stream << FCompareToString.at(m_Compare) << ' ';

    m_LHS->PrintOperand(stream, true);

    stream << ", ";

    m_RHS->PrintOperand(stream, false);

    return stream;
}

std::ostream &scc::ir::FCompareInstruction::PrintAssembly(std::ostream &stream, LoweringContext &context) const
{
    Error("TODO");
}

scc::ir::FCompare scc::ir::FCompareInstruction::GetCompare() const
{
    return m_Compare;
}

scc::ir::Value *scc::ir::FCompareInstruction::GetLHS() const
{
    return m_LHS;
}

scc::ir::Value *scc::ir::FCompareInstruction::GetRHS() const
{
    return m_RHS;
}
