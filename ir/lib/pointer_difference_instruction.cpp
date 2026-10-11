#include <scc/assert.hpp>
#include <scc/ir/instruction.hpp>
#include <scc/ir/type.hpp>

scc::ir::PointerDifferenceInstruction::PointerDifferenceInstruction(
    Type *type,
    Block *block,
    std::string name,
    Value *left,
    Value *right)
    : Instruction(type, block, std::move(name)),
      m_Left(left),
      m_Right(right)
{
}

scc::ir::PointerDifferenceInstruction::~PointerDifferenceInstruction()
{
    DropAll();
}

void scc::ir::PointerDifferenceInstruction::DropAll()
{
    if (m_Left)
    {
        m_Left->Drop(this);
        m_Left = {};
    }
    if (m_Right)
    {
        m_Right->Drop(this);
        m_Right = {};
    }
}

void scc::ir::PointerDifferenceInstruction::Replace(Value *value, Value *with)
{
    if (m_Left == value)
    {
        value->Drop(this);
        if (with)
            with->Use(this);

        m_Left = with;
    }
    if (m_Right == value)
    {
        value->Drop(this);
        if (with)
            with->Use(this);

        m_Right = with;
    }
}

std::ostream &scc::ir::PointerDifferenceInstruction::Print(std::ostream &stream) const
{
    if (IsUsed())
        stream << '%' << m_Name << " = ";

    m_Type->Print(stream) << " ptrdiff ";

    m_Left->PrintOperand(stream, true);

    stream << ", ";

    m_Right->PrintOperand(stream, false);

    return stream;
}

std::ostream &scc::ir::PointerDifferenceInstruction::PrintAssembly(
    std::ostream &stream,
    LoweringContext &context) const
{
    Error("TODO");
}
