#include <scc/ir/instruction.hpp>

#include <scc/assert.hpp>

#include <ostream>

const std::unordered_map<scc::ir::ICompare, std::string_view> scc::ir::ICompareToString
{
    { ICompare::SLT, "slt" },
    { ICompare::ULT, "ult" },
    { ICompare::SGT, "sgt" },
    { ICompare::UGT, "ugt" },
    { ICompare::SLE, "sle" },
    { ICompare::ULE, "ule" },
    { ICompare::SGE, "sge" },
    { ICompare::UGE, "uge" },
    { ICompare::EQU, "equ" },
    { ICompare::NEQ, "neq" },
};

const std::unordered_map<std::string_view, scc::ir::ICompare> scc::ir::StringToICompare
{
    { "slt", ICompare::SLT },
    { "ult", ICompare::ULT },
    { "sgt", ICompare::SGT },
    { "ugt", ICompare::UGT },
    { "sle", ICompare::SLE },
    { "ule", ICompare::ULE },
    { "sge", ICompare::SGE },
    { "uge", ICompare::UGE },
    { "equ", ICompare::EQU },
    { "neq", ICompare::NEQ },
};

scc::ir::ICompareInstruction::ICompareInstruction(
    Type *type,
    Block *block,
    std::string name,
    const ICompare compare,
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

scc::ir::ICompareInstruction::~ICompareInstruction()
{
    DropAll();
}

void scc::ir::ICompareInstruction::DropAll()
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

void scc::ir::ICompareInstruction::Replace(Value *value, Value *with)
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

std::ostream &scc::ir::ICompareInstruction::Print(std::ostream &stream) const
{
    if (IsUsed())
        stream << '%' << m_Name << " = ";

    stream << ICompareToString.at(m_Compare) << ' ';

    m_LHS->PrintOperand(stream, true);

    stream << ", ";

    m_RHS->PrintOperand(stream, false);

    return stream;
}

std::ostream &scc::ir::ICompareInstruction::PrintAssembly(std::ostream &stream, LoweringContext &context) const
{
    Error("TODO");
}

scc::ir::ICompare scc::ir::ICompareInstruction::GetCompare() const
{
    return m_Compare;
}

scc::ir::Value *scc::ir::ICompareInstruction::GetLHS() const
{
    return m_LHS;
}

scc::ir::Value *scc::ir::ICompareInstruction::GetRHS() const
{
    return m_RHS;
}
