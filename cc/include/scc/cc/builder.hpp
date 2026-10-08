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
        size_t Depth;

        ir::Block *Head;
        ir::Block *Tail;

        std::vector<ValuePtr> Values;
        std::unordered_map<std::string, ValuePtr> Named;
    };

    class Builder
    {
    public:
        explicit Builder(
            Context &context,
            ir::Context &ir_context,
            ir::Module &ir_module,
            ir::Builder &ir_builder);

        [[nodiscard]] Context &GetContext() const;
        [[nodiscard]] ir::Context &GetIRContext() const;
        [[nodiscard]] ir::Module &GetIRModule() const;
        [[nodiscard]] ir::Builder &GetIRBuilder() const;

        Value *Manage(ValuePtr value);

        void PushFrame(
            ir::Block *head,
            ir::Block *tail);
        void PopFrame();

        [[nodiscard]] ir::Block *GetHead() const;
        [[nodiscard]] ir::Block *GetTail() const;

        void SetNamed(const std::string &name, ValuePtr value);
        [[nodiscard]] Value *GetNamed(const std::string &name) const;

    private:
        Context &m_Context;
        ir::Context &m_IRContext;
        ir::Module &m_IRModule;
        ir::Builder &m_IRBuilder;

        std::vector<Frame> m_Stack;
    };
}
