#pragma once

#include <scc/as/as.hpp>

#include <string>

namespace scc::as
{
    class Symbol
    {
    public:
        explicit Symbol(std::string name = {});
        explicit Symbol(Fragment &fragment, std::string name = {});
        explicit Symbol(Section &section, size_t index, std::string name = {});

        void SetName(std::string name);
        void SetFragment(Fragment &fragment);
        void SetFragment(Section &section, size_t index);

        [[nodiscard]] const std::string &GetName() const;
        [[nodiscard]] Fragment *GetFragment() const;

        Fragment &operator*() const;
        Fragment *operator->() const;

        explicit operator bool() const;

    private:
        std::string m_Name;
        Fragment *m_Fragment;
    };
}
