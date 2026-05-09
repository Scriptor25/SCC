#include <scc/ir/block.hpp>
#include <scc/ir/instruction.hpp>

scc::ir::SelectInstruction::SelectInstruction(
    Type *type,
    Block *block,
    std::string name,
    std::vector<std::pair<Block *, Value *>> options)
    : Instruction(type, block, std::move(name)),
      m_Options(std::move(options))
{
    for (auto &[key, value] : m_Options)
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
    for (auto &[key, value] : m_Options)
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
}

void scc::ir::SelectInstruction::Replace(Value *value, Value *with)
{
    for (auto &[key, val] : m_Options)
    {
        if (key == value)
        {
            value->Drop(this);
            if (with)
                with->Use(this);

            key = dynamic_cast<Block *>(value);
            return;
        }

        if (val == value)
        {
            value->Drop(this);
            if (with)
                with->Use(this);

            val = dynamic_cast<Block *>(value);
            return;
        }
    }

    Instruction::Replace(value, with);
}

std::ostream &scc::ir::SelectInstruction::Print(std::ostream &stream) const
{
    if (IsUsed())
        stream << '%' << m_Name << " = ";

    stream << "select ";

    for (auto i = m_Options.begin(); i != m_Options.end(); ++i)
    {
        if (i != m_Options.begin())
            stream << ", ";

        i->second->PrintOperand(i->first->PrintOperand(stream << "[ ") << ", ") << " ]";
    }

    return stream;
}

unsigned scc::ir::SelectInstruction::GetOptionCount() const
{
    return m_Options.size();
}

std::pair<scc::ir::Block *, scc::ir::Value *> scc::ir::SelectInstruction::GetOption(const unsigned index) const
{
    return m_Options[index];
}
