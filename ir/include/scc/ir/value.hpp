#pragma once

#include <vector>
#include <scc/ir/ir.hpp>

namespace scc::ir
{
    class Value
    {
    public:
        using Ptr = std::shared_ptr<Value>;

        explicit Value(TypePtr type);
        virtual ~Value() = default;

        virtual std::ostream &Print(std::ostream &stream) const = 0;

    protected:
        TypePtr m_Type;
    };

    class Global : public Value
    {
    public:
        using Ptr = std::shared_ptr<Global>;

        explicit Global(TypePtr type, std::string name);

    protected:
        std::string m_Name;
    };

    class Variable final : public Global
    {
    public:
        using Ptr = std::shared_ptr<Variable>;

        explicit Variable(TypePtr type, std::string name);

        std::ostream &Print(std::ostream &stream) const override;
    };

    class Function final : public Global
    {
    public:
        using Ptr = std::shared_ptr<Function>;

        explicit Function(TypePtr type, std::string name);

        std::ostream &Print(std::ostream &stream) const override;
    };

    class Constant : public Value
    {
    public:
        using Ptr = std::shared_ptr<Constant>;

        explicit Constant(TypePtr type);
    };

    class ConstantInt final : public Constant
    {
    public:
        using Ptr = std::shared_ptr<ConstantInt>;

        explicit ConstantInt(TypePtr type, uint64_t value);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        uint64_t m_Value;
    };

    class ConstantFloat final : public Constant
    {
    public:
        using Ptr = std::shared_ptr<ConstantFloat>;

        explicit ConstantFloat(TypePtr type, double value);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        double m_Value;
    };

    class ConstantArray final : public Constant
    {
    public:
        using Ptr = std::shared_ptr<ConstantArray>;

        explicit ConstantArray(TypePtr type, std::vector<Constant::Ptr> elements);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        std::vector<Constant::Ptr> m_Elements;
    };

    class ConstantVector final : public Constant
    {
    public:
        using Ptr = std::shared_ptr<ConstantVector>;

        explicit ConstantVector(TypePtr type, std::vector<Constant::Ptr> elements);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        std::vector<Constant::Ptr> m_Elements;
    };

    class ConstantStruct final : public Constant
    {
    public:
        using Ptr = std::shared_ptr<ConstantStruct>;

        explicit ConstantStruct(TypePtr type, std::vector<Constant::Ptr> elements);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        std::vector<Constant::Ptr> m_Elements;
    };
}
