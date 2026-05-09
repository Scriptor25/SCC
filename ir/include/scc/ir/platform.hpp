#pragma once

namespace scc::ir
{
    enum class Endian
    {
        Little,
        Big,
    };

    struct Platform
    {
        Endian Endianness;

        size_t PointerSize;
        size_t PointerAlign;

        size_t StackAlign;

        size_t MaxIntAlign;
        size_t MaxFloatAlign;
        size_t MaxVectorAlign;
        size_t MaxAggregateAlign;

        std::vector<size_t> LegalIntWidth;
        std::vector<size_t> LegalFloatWidth;
    };
}
