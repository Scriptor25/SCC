#pragma once

#include <scc/cc/cc.hpp>

#include <scc/ir/value.hpp>

#include <memory>

namespace scc::cc
{
    struct RValue;
    struct LValue;

    struct Value
    {
        static std::unique_ptr<RValue> CreateR(QualifiedType type, ir::Value *value);
        static std::unique_ptr<LValue> CreateL(QualifiedType type, ir::Value *pointer);

        explicit Value(QualifiedType type);
        virtual ~Value() = default;

        [[nodiscard]] virtual ir::Value *Load(Builder &builder, bool is_volatile = false) const = 0;

        virtual void Store(
            ir::Context &context,
            ir::Builder &builder,
            ir::Value *value,
            bool is_volatile = false) const = 0;

        [[nodiscard]] virtual ir::Value *Reference() const = 0;

        QualifiedType Ty;
    };

    struct RValue : Value
    {
        explicit RValue(QualifiedType type, ir::Value *val);

        [[nodiscard]] ir::Value *Load(Builder &builder, bool is_volatile) const override;
        void Store(ir::Context &context, ir::Builder &builder, ir::Value *value, bool is_volatile) const override;
        [[nodiscard]] ir::Value *Reference() const override;

        ir::Value *Val;
    };

    struct LValue : Value
    {
        explicit LValue(QualifiedType type, ir::Value *ptr);

        [[nodiscard]] ir::Value *Load(Builder &builder, bool is_volatile) const override;
        void Store(ir::Context &context, ir::Builder &builder, ir::Value *value, bool is_volatile) const override;
        [[nodiscard]] ir::Value *Reference() const override;

        ir::Value *Ptr;
    };
}
