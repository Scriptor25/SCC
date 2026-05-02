#pragma once

namespace scc::ir
{
    class Usable
    {
    public:
        virtual ~Usable() = default;

        void Use();
        void Drop();

        [[nodiscard]] unsigned GetUseCount() const;
        [[nodiscard]] bool IsUsed() const;

    protected:
        unsigned m_UseCount = 0u;
    };
}
