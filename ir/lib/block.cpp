#include <scc/ir/block.hpp>
#include <scc/ir/empty_value.hpp>
#include <scc/ir/function.hpp>
#include <scc/ir/instruction.hpp>

#include <scc/assert.hpp>
#include <scc/error.hpp>

#include <ostream>

scc::ir::Block::Block(std::string name, Function *function)
    : Value(nullptr),
      m_Name(std::move(name)),
      m_Function(function)
{
}

void scc::ir::Block::DropAll()
{
    for (auto &instruction : m_Instructions)
    {
        instruction->ReplaceWith(nullptr);
        instruction->DropAll();
        instruction.reset();
    }

    m_Instructions.clear();
}

void scc::ir::Block::SetName(std::string name)
{
    m_Name = std::move(name);
}

const std::string &scc::ir::Block::GetName() const
{
    return m_Name;
}

std::ostream &scc::ir::Block::Print(std::ostream &stream) const
{
    stream << '.' << m_Name << ':';

    if (auto predecessors = GetPredecessors(); !predecessors.empty())
    {
        stream << " ; ";
        for (auto i = predecessors.begin(); i != predecessors.end(); ++i)
        {
            if (i != predecessors.begin())
                stream << ", ";
            stream << (*i)->GetName();
        }
    }

    for (auto &instruction : m_Instructions)
        instruction->Print(stream << std::endl << "    ");

    return stream;
}

std::ostream &scc::ir::Block::PrintOperand(std::ostream &stream, bool print_type) const
{
    return stream << '.' << m_Name;
}

scc::ir::Function *scc::ir::Block::GetFunction() const
{
    return m_Function;
}

size_t scc::ir::Block::GetInstructionCount() const
{
    return m_Instructions.size();
}

scc::ir::Instruction *scc::ir::Block::GetInstruction(const size_t index) const
{
    AssertIndexInBounds(index, m_Instructions.size());

    return m_Instructions[index].get();
}

scc::ir::Instruction *scc::ir::Block::Insert(std::unique_ptr<Instruction> instruction)
{
    Assert(!!instruction, "instruction must not be null");

    auto *ptr = instruction.get();

    if (auto &name = instruction->GetName(); !name.empty())
        if (auto *value = m_Function->FindValue(name))
        {
            auto *empty = dynamic_cast<EmptyValue *>(value);
            Assert(!!empty, "value %{} does already exist and is not empty", name);

            empty->ReplaceWith(ptr);
        }

    m_Instructions.push_back(std::move(instruction));

    return ptr;
}

void scc::ir::Block::Erase(const Instruction *instruction)
{
    Assert(!!instruction, "instruction must not be null");

    for (auto it = m_Instructions.begin(); it != m_Instructions.end(); ++it)
        if (it->get() == instruction)
        {
            m_Instructions.erase(it);
            return;
        }

    Error("cannot erase instruction because block does not own it");
}

scc::ir::Value *scc::ir::Block::CreateEmpty(Type *type, std::string name) const
{
    return m_Function->CreateEmpty(type, std::move(name));
}

scc::ir::Value *scc::ir::Block::FindValue(const std::string &name) const
{
    for (auto &instruction : m_Instructions)
        if (instruction->GetName() == name)
            return instruction.get();

    return nullptr;
}

scc::ir::Instruction *scc::ir::Block::GetTerminator() const
{
    for (auto &instruction : m_Instructions)
        if (instruction->IsTerminator())
            return instruction.get();

    return nullptr;
}

std::unordered_set<scc::ir::Block *> scc::ir::Block::GetPredecessors() const
{
    return m_Function->GetPredecessors(this);
}

std::unordered_set<scc::ir::Block *> scc::ir::Block::GetSuccessors() const
{
    if (const auto *terminator = GetTerminator())
    {
        std::unordered_set<Block *> successors;

        for (size_t i = 0; i < terminator->GetSuccessorCount(); ++i)
            successors.insert(terminator->GetSuccessor(i));

        return successors;
    }

    return {};
}
