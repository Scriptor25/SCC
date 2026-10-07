#pragma once

#include <scc/cc/cc.hpp>

#include <scc/ir/value.hpp>

#include <memory>

namespace scc::cc
{
    class RValue;
    class LValue;

    class Value
    {
    public:
        static std::unique_ptr<RValue> CreateR(ir::Value *value);
        static std::unique_ptr<LValue> CreateL(ir::Value *pointer);

        virtual ~Value() = default;

        virtual ir::Value *Load(Builder &builder) const = 0;
        virtual void Store(ir::Context &context, ir::Builder &builder, ir::Value *value) const = 0;
    };

    class RValue : public Value
    {
    public:
        explicit RValue(ir::Value *value);

        ir::Value *Load(Builder &builder) const override;
        void Store(ir::Context &context, ir::Builder &builder, ir::Value *value) const override;

    private:
        ir::Value *m_Value;
    };

    class LValue : public Value
    {
    public:
        explicit LValue(ir::Value *pointer);

        ir::Value *Load(Builder &builder) const override;
        void Store(ir::Context &context, ir::Builder &builder, ir::Value *value) const override;

    private:
        ir::Value *m_Pointer;
    };
}
