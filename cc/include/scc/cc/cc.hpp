#pragma once

#include <memory>

namespace scc::cc
{
    class Context;
    class Parser;
    class Builder;

    struct Type;
    struct Node;

    class Value;
    using ValuePtr = std::unique_ptr<Value>;
}
