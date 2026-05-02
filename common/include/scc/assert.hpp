#pragma once

#include <format>
#include <stdexcept>

namespace scc
{
    template<typename... Args>
    void Assert(const bool condition, std::format_string<Args...> format, Args &&... args)
    {
        if (condition)
            return;

        auto message = std::format(std::move(format), std::forward<Args>(args)...);
        throw std::runtime_error(message);
    }

    void AssertIndexInBounds(size_t index, size_t count);
}
