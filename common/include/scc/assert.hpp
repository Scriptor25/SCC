#pragma once

#include <format>

namespace scc
{
    template<typename... A>
    [[noreturn]] void Error(std::format_string<A...> fmt, A &&... args)
    {
        throw std::runtime_error(std::format(std::move(fmt), std::forward<A>(args)...));
    }

    template<typename C, typename... A>
    void Assert(C condition, std::format_string<A...> fmt, A &&... args)
    {
        if (!condition)
            throw std::runtime_error(std::format(std::move(fmt), std::forward<A>(args)...));
    }

    [[noreturn]] inline void ErrorOutOfBounds(size_t index, size_t count)
    {
        Error("index {} out of bounds [{};{})", index, 0, count);
    }

    inline void AssertInBounds(size_t index, size_t count)
    {
        Assert(index < count, "index {} out of bounds [{};{})", index, 0, count);
    }
}
