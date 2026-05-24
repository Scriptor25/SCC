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

scc::as::Fragment *scc::as::Section::Insert(std::unique_ptr<Fragment> fragment)
{
    auto *ptr = fragment.get();

    m_Fragments.push_back(std::move(fragment));

    return ptr;
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

size_t scc::as::Section::size() const
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

    size_t i;

    for (i = 0; i < m_Fragments.size(); ++i)
    {
        for (const auto symbols = m_Module->GetSymbols(*this, i); const auto *symbol : symbols)
            stream << symbol->GetName() << ":\n";

        m_Fragments[i]->Print(stream << "    ") << '\n';
    }

    for (const auto symbols = m_Module->GetSymbols(*this, i); const auto *symbol : symbols)
        stream << symbol->GetName() << ":\n";

    return stream;
}
