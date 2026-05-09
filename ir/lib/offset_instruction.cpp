#include <scc/ir/instruction.hpp>
#include <scc/ir/type.hpp>

#include <ostream>

scc::ir::OffsetInstruction::OffsetInstruction(
    Type *type,
    Block *block,
    std::string name,
    Value *base,
    std::vector<Value *> offsets)
    : Instruction(type, block, std::move(name)),
      m_Base(base),
      m_Offsets(std::move(offsets))
{
    m_Base->Use(this);

    for (auto *offset : m_Offsets)
        offset->Use(this);
}

scc::ir::OffsetInstruction::~OffsetInstruction()
{
    DropAll();
}

void scc::ir::OffsetInstruction::DropAll()
{
    if (m_Base)
    {
        m_Base->Drop(this);
        m_Base = nullptr;
    }

    for (auto &offset : m_Offsets)
        if (offset)
        {
            offset->Drop(this);
            offset = nullptr;
        }
}

void scc::ir::OffsetInstruction::Replace(Value *value, Value *with)
{
    if (m_Base == value)
    {
        value->Drop(this);
        if (with)
            with->Use(this);

        m_Base = with;
        return;
    }

    for (auto &offset : m_Offsets)
        if (offset == value)
        {
            value->Drop(this);
            if (with)
                with->Use(this);

            offset = with;
            return;
        }

    Instruction::Replace(value, with);
}

std::ostream &scc::ir::OffsetInstruction::Print(std::ostream &stream) const
{
    if (IsUsed())
        stream << '%' << m_Name << " = ";

    m_Base->PrintOperand(m_Type->Print(stream) << " offset ");

    for (const auto *offset : m_Offsets)
        offset->PrintOperand(stream << ", ");

    return stream;
}

scc::ir::Value *scc::ir::OffsetInstruction::GetBase() const
{
    return m_Base;
}

unsigned scc::ir::OffsetInstruction::GetOffsetCount() const
{
    return m_Offsets.size();
}

scc::ir::Value *scc::ir::OffsetInstruction::GetOffset(const unsigned index) const
{
    return m_Offsets[index];
}
