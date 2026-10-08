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
        static std::unique_ptr<RValue> CreateR(const Type *type, ir::Value *value);
        static std::unique_ptr<LValue> CreateL(const Type *type, ir::Value *pointer);

        explicit Value(const Type *type);
        virtual ~Value() = default;

        [[nodiscard]] const Type *GetType() const;

        [[nodiscard]] virtual ir::Value *Load(Builder &builder, bool is_volatile = false) const = 0;

        virtual void Store(
            ir::Context &context,
            ir::Builder &builder,
            ir::Value *value,
            bool is_volatile = false) const = 0;

    private:
        const Type *m_Type;
    };

    class RValue : public Value
    {
    public:
        explicit RValue(const Type *type, ir::Value *value);

        [[nodiscard]] ir::Value *Load(Builder &builder, bool is_volatile) const override;
        void Store(ir::Context &context, ir::Builder &builder, ir::Value *value, bool is_volatile) const override;

    private:
        ir::Value *m_Value;
    };

    class LValue : public Value
    {
    public:
        explicit LValue(const Type *type, ir::Value *pointer);

        [[nodiscard]] ir::Value *Load(Builder &builder, bool is_volatile) const override;
        void Store(ir::Context &context, ir::Builder &builder, ir::Value *value, bool is_volatile) const override;

    private:
        ir::Value *m_Pointer;
    };
}
