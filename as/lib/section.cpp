#include <scc/as/instruction.hpp>
#include <scc/as/module.hpp>
#include <scc/as/section.hpp>

scc::as::Section::Section(Module *module, std::string name)
    : m_Module(module),
      m_Name(std::move(name))
{
}

void scc::as::Section::SetName(std::string name)
{
    m_Name = std::move(name);
}

void scc::as::Section::Insert(std::unique_ptr<Fragment> fragment)
{
    m_Fragments.push_back(std::move(fragment));
}

scc::as::Fragment *scc::as::Section::Insert(Instruction instruction)
{
    auto fragment = std::make_unique<Instruction>(std::move(instruction));
    auto *ptr = fragment.get();

    m_Fragments.push_back(std::move(fragment));

    return ptr;
}

const std::string &scc::as::Section::GetName() const
{
    return m_Name;
}

size_t scc::as::Section::GetFragmentCount() const
{
    return m_Fragments.size();
}

scc::as::Fragment &scc::as::Section::operator[](const size_t index)
{
    return *m_Fragments[index];
}

const scc::as::Fragment &scc::as::Section::operator[](const size_t index) const
{
    return *m_Fragments[index];
}

scc::as::Section::iterator<false> scc::as::Section::begin()
{
    return { size_t(), m_Fragments };
}

scc::as::Section::iterator<false> scc::as::Section::end()
{
    return { ~size_t(), m_Fragments };
}

scc::as::Section::iterator<true> scc::as::Section::begin() const
{
    return { size_t(), m_Fragments };
}

scc::as::Section::iterator<true> scc::as::Section::end() const
{
    return { ~size_t(), m_Fragments };
}

std::ostream &scc::as::Section::Print(std::ostream &stream) const
{
    stream << ".section " << m_Name << '\n';

    for (auto &fragment : m_Fragments)
    {
        if (const auto *symbol = m_Module->GetSymbol(fragment.get()))
        {
            stream << symbol->GetName() << ":\n";
        }

        fragment->Print(stream << "    ") << '\n';
    }

    return stream;
}
