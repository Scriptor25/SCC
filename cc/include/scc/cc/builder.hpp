#pragma once

#include <scc/cc/cc.hpp>
#include <scc/cc/value.hpp>

#include <scc/ir/builder.hpp>
#include <scc/ir/context.hpp>
#include <scc/ir/module.hpp>

#include <string>
#include <unordered_map>
#include <vector>

namespace scc::cc
{
    struct Frame
    {
        ir::Block *Head;
        ir::Block *Tail;

        std::vector<ValuePtr> Values;
        std::unordered_map<std::string, ValuePtr> Named;
    };

    class Builder
    {
    public:
        explicit Builder(
            ir::Context &context,
            ir::Module &module,
            ir::Builder &builder);

        [[nodiscard]] ir::Context &GetContext() const;
        [[nodiscard]] ir::Module &GetModule() const;
        [[nodiscard]] ir::Builder &GetBuilder() const;

        Value *Manage(ValuePtr value);

        void PushFrame(
            ir::Block *head,
            ir::Block *tail);
        void PopFrame();

        [[nodiscard]] ir::Block *GetHead() const;
        [[nodiscard]] ir::Block *GetTail() const;

        void SetNamed(const std::string &name, ValuePtr value);
        [[nodiscard]] const Value *GetNamed(const std::string &name) const;

    private:
        ir::Context &m_Context;
        ir::Module &m_Module;
        ir::Builder &m_Builder;

        std::vector<Frame> m_Stack;
    };
}
