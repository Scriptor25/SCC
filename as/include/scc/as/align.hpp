#pragma once

#include <scc/as/as.hpp>
#include <scc/as/fragment.hpp>

namespace scc::as
{
    class Align : public Fragment
    {
    public:
        explicit Align(uint8_t alignment);

        [[nodiscard]] uint8_t GetAlignment() const;

        std::ostream &Print(std::ostream &stream) const override;
        void Encode(std::vector<uint8_t> &buffer, SymbolTable &symbol_table, FixupTable &fixup_table) const override;

    private:
        uint8_t m_Alignment;
    };
}
