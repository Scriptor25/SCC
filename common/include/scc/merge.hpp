#pragma once

namespace scc
{
    template<typename T>
        requires (
            toolkit::map_type<T>
            || toolkit::unordered_map_type<T>
            || toolkit::set_type<T>
            || toolkit::unordered_set_type<T>
        )
    T &merge(T &dst, T &&src)
    {
        for (auto &&entry : std::forward<T>(src))
            dst.insert(entry);

        return dst;
    }

    template<toolkit::vector_type V>
    V &merge(V &dst, V &&src)
    {
        for (auto &&entry : std::forward<V>(src))
            dst.push_back(entry);

        return dst;
    }
}
