#pragma once

#include <scc/as/as.hpp>

#include <scc/common.hpp>

#include <iosfwd>
#include <string>

namespace scc::as
{
    class Operand
    {
    public:
        explicit Operand(const Platform &platform);
        virtual ~Operand() = default;

        [[nodiscard]] virtual Immediate GetImmediate() const;

        virtual std::ostream &Print(std::ostream &stream) const = 0;

    protected:
        const Platform &m_Platform;
    };

    class ImmediateOperand : public Operand
    {
    public:
        explicit ImmediateOperand(const Platform &platform, Immediate value);

        [[nodiscard]] Immediate GetImmediate() const override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] Immediate GetValue() const;

    private:
        Immediate m_Value;
    };

    class StringOperand : public Operand
    {
    public:
        explicit StringOperand(const Platform &platform, std::string value);

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] const std::string &GetValue() const;

    private:
        std::string m_Value;
    };

    class SymbolAddressOperand : public Operand
    {
    public:
        explicit SymbolAddressOperand(const Platform &platform, Symbol *symbol);

        [[nodiscard]] Immediate GetImmediate() const override;

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] Symbol *GetSymbol() const;

    private:
        Symbol *m_Symbol;
    };

    class RegisterOperand : public Operand
    {
    public:
        explicit RegisterOperand(const Platform &platform, Register register_);

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] Register GetRegister() const;

    private:
        Register m_Register;
    };

    class DirectOperand : public Operand
    {
    public:
        explicit DirectOperand(const Platform &platform, Immediate address);

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] Immediate GetAddress() const;

    private:
        Immediate m_Address;
    };

    class ReferenceOperand : public Operand
    {
    public:
        ReferenceOperand(
            const Platform &platform,
            Immediate displacement,
            Register base_register,
            Register index_register,
            Immediate scale);

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] Immediate GetDisplacement() const;
        [[nodiscard]] Register GetBaseRegister() const;
        [[nodiscard]] Register GetIndexRegister() const;
        [[nodiscard]] Immediate GetScale() const;

    private:
        Immediate m_Displacement;
        Register m_BaseRegister;
        Register m_IndexRegister;
        Immediate m_Scale;
    };

    class SymbolOperand : public Operand
    {
    public:
        explicit SymbolOperand(const Platform &platform, Symbol *symbol);

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] Symbol *GetSymbol() const;

    private:
        Symbol *m_Symbol;
    };
}
