#pragma once

#include <scc/ir/ir.hpp>

#include <format>
#include <iosfwd>
#include <sstream>
#include <unordered_set>

namespace scc::ir
{
    class Value
    {
    public:
        explicit Value(Type *type);
        virtual ~Value();

        virtual std::ostream &Print(std::ostream &stream) const = 0;
        virtual std::ostream &PrintOperand(std::ostream &stream, bool print_type) const = 0;

        void Use(User *user);
        void Drop(User *user);
        void ReplaceWith(Value *with);

        bool IsUsed() const;
        size_t GetUseCount() const;

        [[nodiscard]] Type *GetType() const;

    protected:
        Type *m_Type;

        std::unordered_set<User *> m_Uses;
    };
}

template<std::derived_from<scc::ir::Value> T>
struct std::formatter<T *>
{
    template<typename C>
    static constexpr auto parse(C &&ctx)
    {
        return ctx.begin();
    }

    template<typename C>
    static auto format(T *value, C &&ctx)
    {
        std::ostringstream stream;
        value->PrintOperand(stream, true);

        for (auto c : stream.view())
            *ctx.out()++ = c;

        return ctx.out();
    }
};
