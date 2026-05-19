#pragma once

#include <scc/as/as.hpp>
#include <scc/as/fragment.hpp>

#include <vector>

namespace scc::as
{
    class Data : public Fragment
    {
    public:
        explicit Data(std::vector<uint8_t> data);

        [[nodiscard]] const std::vector<uint8_t> &GetData() const;

        std::ostream &Print(std::ostream &stream) const override;

    private:
        std::vector<uint8_t> m_Data;
    };
}
