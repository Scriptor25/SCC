#pragma once

#include <scc/as/as.hpp>
#include <scc/as/fragment.hpp>

namespace scc::as
{
    class Fill : public Fragment
    {
    public:
        explicit Fill(uint64_t count, uint8_t value);

        [[nodiscard]] uint64_t GetCount() const;
        [[nodiscard]] uint8_t GetValue() const;

        std::ostream &Print(std::ostream &stream) const override;
        void Encode(std::vector<uint8_t> &buffer, SymbolTable &symbol_table, FixupTable &fixup_table) const override;

    private:
        uint64_t m_Count;
        uint8_t m_Value;
    };
}
