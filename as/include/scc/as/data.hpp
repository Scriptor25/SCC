#pragma once

#include <span>
#include <scc/as/as.hpp>
#include <scc/as/fragment.hpp>

#include <vector>

namespace scc::as
{
    class Data : public Fragment
    {
    public:
        explicit Data(std::vector<uint8_t> data);

        [[nodiscard]] std::span<const uint8_t> GetData() const;
        [[nodiscard]] size_t GetDataSize() const;

        std::ostream &Print(std::ostream &stream) const override;

    private:
        std::vector<uint8_t> m_Data;
    };
}
