#pragma once

namespace scc
{
    template<typename D>
    D &merge(D &dst, D &&src)
    {
        for (auto &&entry : src)
            dst.insert(entry);

        return dst;
    }
}
