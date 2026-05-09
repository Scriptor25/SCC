#pragma once

#include <scc/as/as.hpp>
#include <scc/as/fragment.hpp>

#include <memory>
#include <string>
#include <vector>

namespace scc::as
{
    class Section
    {
    public:
        [[nodiscard]] const std::string &GetName() const;

        [[nodiscard]] size_t GetFragmentCount() const;

        Fragment &operator[](size_t index);
        [[nodiscard]] const Fragment &operator[](size_t index) const;

    private:
        std::string m_Name;
        std::vector<std::unique_ptr<Fragment>> m_Fragments;
    };
}
