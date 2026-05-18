#pragma once

#include <scc/as/as.hpp>

#include <scc/common.hpp>

#include <iosfwd>

namespace scc::as
{
    class Operand
    {
    public:
        explicit Operand(const Platform &platform);
        virtual ~Operand() = default;

        virtual std::ostream &Print(std::ostream &stream) const = 0;

    protected:
        const Platform &m_Platform;
    };

    class ImmediateOperand final : public Operand
    {
    public:
        explicit ImmediateOperand(const Platform &platform, Immediate immediate);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        Immediate m_Immediate;
    };

    class RegisterOperand final : public Operand
    {
    public:
        explicit RegisterOperand(const Platform &platform, Register register_);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        Register m_Register;
    };

    class ReferenceOperand final : public Operand
    {
    public:
        ReferenceOperand(
            const Platform &platform,
            Immediate displacement,
            Register base_register,
            Register index_register,
            Immediate scale);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        Immediate m_Displacement;
        Register m_BaseRegister;
        Register m_IndexRegister;
        Immediate m_Scale;
    };

    class SymbolOperand final : public Operand
    {
    public:
        explicit SymbolOperand(const Platform &platform, Symbol *symbol);

        std::ostream &Print(std::ostream &stream) const override;

    private:
        Symbol *m_Symbol;
    };
}
