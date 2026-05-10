#pragma once

#include <scc/as/as.hpp>
#include <scc/as/fragment.hpp>

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

namespace scc::as
{
    class Section
    {
        template<bool constant>
        struct iterator
        {
            using value_type = std::conditional_t<constant, const Fragment, Fragment>;

            bool operator!=(const iterator &other) const
            {
                return index != other.index;
            }

            value_type &operator*() const
            {
                return *fragments[index];
            }

            iterator &operator++()
            {
                if (index == ~size_t())
                    return *this;

                if (++index >= fragments.size())
                    index = ~size_t();

                return *this;
            }

            size_t index;
            const std::vector<std::unique_ptr<Fragment>> &fragments;
        };

    public:
        explicit Section(Module *module, std::string name = {});

        void SetName(std::string name);
        void Insert(std::unique_ptr<Fragment> fragment);
        Fragment *Insert(Instruction instruction);

        [[nodiscard]] const std::string &GetName() const;

        [[nodiscard]] size_t GetFragmentCount() const;

        Fragment &operator[](size_t index);

        [[nodiscard]] const Fragment &operator[](size_t index) const;

        iterator<false> begin();
        iterator<false> end();

        [[nodiscard]] iterator<true> begin() const;
        [[nodiscard]] iterator<true> end() const;

        std::ostream &Print(std::ostream &stream) const;

    private:
        Module *m_Module;

        std::string m_Name;
        std::vector<std::unique_ptr<Fragment>> m_Fragments;
    };
}
