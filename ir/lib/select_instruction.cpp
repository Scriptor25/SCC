#include <scc/ir/block.hpp>
#include <scc/ir/instruction.hpp>
#include <scc/ir/type.hpp>

scc::ir::SelectInstruction::SelectInstruction(
    Type *type,
    Block *block,
    std::string name,
    std::vector<std::pair<Block *, Value *>> nodes)
    : Instruction(type, block, std::move(name)),
      m_Nodes(std::move(nodes))
{
    for (auto &[key, value] : m_Nodes)
    {
        key->Use(this);
        value->Use(this);
    }
}

scc::ir::SelectInstruction::~SelectInstruction()
{
    DropAll();
}

void scc::ir::SelectInstruction::DropAll()
{
    for (auto &[key, value] : m_Nodes)
    {
        if (key)
        {
            key->Drop(this);
            key = nullptr;
        }

        if (value)
        {
            value->Drop(this);
            value = nullptr;
        }
    }

    m_Nodes.clear();
}

void scc::ir::SelectInstruction::Replace(Value *value, Value *with)
{
    for (auto &[key, val] : m_Nodes)
    {
        if (key == value)
        {
            value->Drop(this);
            if (with)
                with->Use(this);

            key = dynamic_cast<Block *>(with);
        }

        if (val == value)
        {
            value->Drop(this);
            if (with)
                with->Use(this);

            val = with;
        }
    }
}

std::ostream &scc::ir::SelectInstruction::Print(std::ostream &stream) const
{
    if (IsUsed())
        stream << '%' << m_Name << " = ";

    m_Type->Print(stream << "select ") << ' ';

    for (auto i = m_Nodes.begin(); i != m_Nodes.end(); ++i)
    {
        if (i != m_Nodes.begin())
            stream << ", ";

        i->second->PrintOperand(i->first->PrintOperand(stream << "[ ", false) << ", ", false) << " ]";
    }

    return stream;
}

size_t scc::ir::SelectInstruction::GetNodeCount() const
{
    return m_Nodes.size();
}

std::pair<scc::ir::Block *, scc::ir::Value *> &scc::ir::SelectInstruction::GetNode(const size_t index)
{
    return m_Nodes[index];
}

const std::pair<scc::ir::Block *, scc::ir::Value *> &scc::ir::SelectInstruction::GetNode(const size_t index) const
{
    return m_Nodes[index];
}
