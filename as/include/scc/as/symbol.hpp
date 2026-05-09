#pragma once

#include <scc/as/as.hpp>

namespace scc::as
{
    class Symbol
    {
    public:
        explicit Symbol(Fragment &fragment);
        explicit Symbol(Section &section, size_t index);

        Fragment &operator*() const;
        Fragment *operator->() const;

        explicit operator bool() const;

    private:
        Fragment *m_Fragment;
    };
}
