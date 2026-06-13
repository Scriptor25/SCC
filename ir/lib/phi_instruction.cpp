#include <scc/ir/block.hpp>
#include <scc/ir/instruction.hpp>
#include <scc/ir/type.hpp>

#include <scc/assert.hpp>

scc::ir::PhiInstruction::PhiInstruction(
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

scc::ir::PhiInstruction::~PhiInstruction()
{
    DropAll();
}

void scc::ir::PhiInstruction::DropAll()
{
    for (auto &[key, value] : m_Nodes)
    {
        if (key)
        {
            key->Drop(this);
            key = {};
        }

        if (value)
        {
            value->Drop(this);
            value = {};
        }
    }

    m_Nodes.clear();
}

void scc::ir::PhiInstruction::Replace(Value *value, Value *with)
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

std::ostream &scc::ir::PhiInstruction::Print(std::ostream &stream) const
{
    if (IsUsed())
        stream << '%' << m_Name << " = ";

    m_Type->Print(stream << "phi ") << ' ';

    for (auto i = m_Nodes.begin(); i != m_Nodes.end(); ++i)
    {
        if (i != m_Nodes.begin())
            stream << ", ";

        i->second->PrintOperand(i->first->PrintOperand(stream << "[ ", false) << ", ", false) << " ]";
    }

    return stream;
}

std::ostream &scc::ir::PhiInstruction::PrintAssembly(std::ostream &stream, LoweringContext &context) const
{
    Error("TODO");
}

size_t scc::ir::PhiInstruction::GetNodeCount() const
{
    return m_Nodes.size();
}

std::pair<scc::ir::Block *, scc::ir::Value *> &scc::ir::PhiInstruction::GetNode(const size_t index)
{
    return m_Nodes[index];
}

const std::pair<scc::ir::Block *, scc::ir::Value *> &scc::ir::PhiInstruction::GetNode(const size_t index) const
{
    return m_Nodes[index];
}
