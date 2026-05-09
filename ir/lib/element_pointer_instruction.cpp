#include <scc/ir/instruction.hpp>
#include <scc/ir/type.hpp>

#include <ostream>

scc::ir::ElementPointerInstruction::ElementPointerInstruction(
    Type *type,
    Block *block,
    std::string name,
    Value *pointer,
    std::vector<Value *> indices)
    : Instruction(type, block, std::move(name)),
      m_Pointer(pointer),
      m_Indices(std::move(indices))
{
    m_Pointer->Use(this);

    for (auto *index : m_Indices)
        index->Use(this);
}

scc::ir::ElementPointerInstruction::~ElementPointerInstruction()
{
    DropAll();
}

void scc::ir::ElementPointerInstruction::DropAll()
{
    if (m_Pointer)
    {
        m_Pointer->Drop(this);
        m_Pointer = nullptr;
    }

    for (auto &index : m_Indices)
        if (index)
        {
            index->Drop(this);
            index = nullptr;
        }

    m_Indices.clear();
}

void scc::ir::ElementPointerInstruction::Replace(Value *value, Value *with)
{
    if (m_Pointer == value)
    {
        value->Drop(this);
        if (with)
            with->Use(this);

        m_Pointer = with;
    }

    for (auto &index : m_Indices)
        if (index == value)
        {
            value->Drop(this);
            if (with)
                with->Use(this);

            index = with;
        }
}

std::ostream &scc::ir::ElementPointerInstruction::Print(std::ostream &stream) const
{
    if (IsUsed())
        stream << '%' << m_Name << " = ";

    m_Pointer->PrintOperand(stream << "element ", true);

    for (const auto *index : m_Indices)
        index->PrintOperand(stream << ", ", true);

    return stream;
}

scc::ir::Value *scc::ir::ElementPointerInstruction::GetBase() const
{
    return m_Pointer;
}

size_t scc::ir::ElementPointerInstruction::GetIndexCount() const
{
    return m_Indices.size();
}

scc::ir::Value *scc::ir::ElementPointerInstruction::GetIndex(const size_t index) const
{
    return m_Indices[index];
}
