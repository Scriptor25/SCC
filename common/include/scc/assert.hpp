#pragma once

#include <format>
#include <stdexcept>

namespace scc
{
    template<typename... Args>
    void Assert(const bool condition, std::string_view format, Args &&... args)
    {
        if (condition)
        {
            return;
        }

        auto message = std::vformat(std::move(format), std::make_format_args(std::forward<Args>(args)...));
        throw std::runtime_error(message);
    }
}
