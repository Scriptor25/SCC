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

        [[nodiscard]] const Platform &GetPlatform() const;

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

    class ReferenceOperand : public Operand
    {
    public:
        ReferenceOperand(const Platform &platform, int64_t displacement);

        ReferenceOperand(
            const Platform &platform,
            int64_t displacement,
            bool has_base_register,
            Register base_register,
            bool has_index_register,
            Register index_register,
            uint8_t scale);

        std::ostream &Print(std::ostream &stream) const override;

        [[nodiscard]] int64_t GetDisplacement() const;
        [[nodiscard]] bool HasBaseRegister() const;
        [[nodiscard]] Register GetBaseRegister() const;
        [[nodiscard]] bool HasIndexRegister() const;
        [[nodiscard]] Register GetIndexRegister() const;
        [[nodiscard]] uint8_t GetScale() const;

    private:
        int64_t m_Displacement;

        bool m_HasBaseRegister;
        Register m_BaseRegister;

        bool m_HasIndexRegister;
        Register m_IndexRegister;

        uint8_t m_Scale;
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
