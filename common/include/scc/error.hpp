#pragma once

#include <format>

namespace scc
{
    template<typename... Args>
    [[noreturn]] void Error(std::format_string<Args...> format, Args &&... args)
    {
        auto message = std::format(std::move(format), std::forward<Args>(args)...);
        throw std::runtime_error(message);
    }
}
