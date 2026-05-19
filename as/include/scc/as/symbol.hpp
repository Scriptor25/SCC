#pragma once

#include <scc/as/as.hpp>

#include <string>

namespace scc::as
{
    class Symbol
    {
    public:
        explicit Symbol(std::string name = {});
        explicit Symbol(Section &section, size_t index, std::string name = {});

        void SetName(std::string name);
        void SetLocation(Section &section, size_t index);
        void SetAddress(size_t address);

        [[nodiscard]] const std::string &GetName() const;

        [[nodiscard]] bool IsResolved() const;

        [[nodiscard]] Section *GetSection() const;
        [[nodiscard]] size_t GetIndex() const;

        [[nodiscard]] Fragment *GetFragment() const;

        [[nodiscard]] Immediate GetAddress() const;

    private:
        std::string m_Name;

        bool m_Resolved;
        Section *m_Section;
        size_t m_Index;
    };
}
