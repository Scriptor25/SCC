#include <scc/platform.hpp>
#include <scc/as/module.hpp>

scc::as::Module::Module(const Platform &platform)
    : m_Platform(platform)
{
}

const scc::Platform &scc::as::Module::GetPlatform() const
{
    return m_Platform;
}

scc::as::Section *scc::as::Module::CreateSection(std::string name)
{
    m_Sections.push_back(std::make_unique<Section>(this, std::move(name)));

    return m_Sections.back().get();
}

scc::as::Symbol *scc::as::Module::CreateSymbol(std::string name)
{
    m_Symbols.push_back(std::make_unique<Symbol>(std::move(name)));

    return m_Symbols.back().get();
}

scc::as::Symbol *scc::as::Module::CreateSymbol(Section &section, const size_t index, std::string name)
{
    m_Symbols.push_back(std::make_unique<Symbol>(section, index, std::move(name)));

    return m_Symbols.back().get();
}

scc::as::Section *scc::as::Module::GetSection(const std::string &name) const
{
    for (auto &section : m_Sections)
        if (section->GetName() == name)
            return section.get();

    return {};
}

scc::as::Symbol *scc::as::Module::GetSymbol(const std::string &name) const
{
    for (auto &symbol : m_Symbols)
        if (symbol->GetName() == name)
            return symbol.get();

    return {};
}

std::vector<scc::as::Symbol *> scc::as::Module::GetSymbols(const Section &section, const size_t index) const
{
    std::vector<Symbol *> symbols;

    for (auto &symbol : m_Symbols)
        if (symbol->GetSection() == &section && symbol->GetIndex() == index)
            symbols.push_back(symbol.get());

    return symbols;
}

std::vector<scc::as::Symbol *> scc::as::Module::GetSymbols(const Fragment &fragment) const
{
    std::vector<Symbol *> symbols;

    for (auto &symbol : m_Symbols)
        if (symbol->GetFragment() == &fragment)
            symbols.push_back(symbol.get());

    return symbols;
}

scc::as::Section *scc::as::Module::GetOrCreateSection(const std::string &name)
{
    if (auto *ptr = GetSection(name))
        return ptr;

    return CreateSection(name);
}

scc::as::Symbol *scc::as::Module::GetOrCreateSymbol(const std::string &name)
{
    if (auto *ptr = GetSymbol(name))
        return ptr;

    return CreateSymbol(name);
}

std::ostream &scc::as::Module::Print(std::ostream &stream) const
{
    for (auto &section : m_Sections)
        section->Print(stream);

    return stream;
}

void scc::as::Module::Encode(std::vector<uint8_t> &buffer) const
{
    SymbolTable symbol_table;
    FixupTable fixup_table;

    for (auto &section : m_Sections)
        for (size_t i = 0; i < section->size(); ++i)
        {
            auto &fragment = (*section)[i];

            for (auto symbols = GetSymbols(*section, i); const auto *symbol : symbols)
            {
                const auto address = buffer.size();

                for (auto &[offset, size, subtract] : fixup_table[symbol])
                {
                    const auto value = address - subtract;
                    const auto little = m_Platform.ISA.Endianness == platform::TargetEndianness::LittleEndian;

                    for (size_t x = 0; x < size; ++x)
                    {
                        const auto shift = little ? x * 8 : (size - x - 1) * 8;
                        buffer[offset + x] = value >> shift & 0xFF;
                    }
                }

                symbol_table[symbol] = address;
                fixup_table.erase(symbol);
            }

            fragment.Encode(buffer, symbol_table, fixup_table);
        }

    switch (m_Platform.ABI.ObjectFormat)
    {
    case platform::TargetObjectFormat::Raw:
        break;

    case platform::TargetObjectFormat::ELF:
        break;

    case platform::TargetObjectFormat::PE:
        break;

    case platform::TargetObjectFormat::COFF:
        break;

    case platform::TargetObjectFormat::MachO:
        break;
    }
}
