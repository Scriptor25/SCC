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
        static std::unique_ptr<RValue> CreateR(Type *type, ir::Value *value);
        static std::unique_ptr<LValue> CreateL(Type *type, ir::Value *pointer);

        explicit Value(Type *type);
        virtual ~Value() = default;

        [[nodiscard]] Type *GetType() const;

        [[nodiscard]] virtual ir::Value *Load(Builder &builder) const = 0;

        virtual void Store(ir::Context &context, ir::Builder &builder, ir::Value *value) const = 0;

    private:
        Type *m_Type;
    };

    class RValue : public Value
    {
    public:
        explicit RValue(Type *type, ir::Value *value);

        [[nodiscard]] ir::Value *Load(Builder &builder) const override;
        void Store(ir::Context &context, ir::Builder &builder, ir::Value *value) const override;

    private:
        ir::Value *m_Value;
    };

    class LValue : public Value
    {
    public:
        explicit LValue(Type *type, ir::Value *pointer);

        [[nodiscard]] ir::Value *Load(Builder &builder) const override;
        void Store(ir::Context &context, ir::Builder &builder, ir::Value *value) const override;

    private:
        ir::Value *m_Pointer;
    };
}
