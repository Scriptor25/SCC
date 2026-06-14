#pragma once

#include <scc/as/as.hpp>

#include <scc/common.hpp>

#include <format>
#include <iosfwd>
#include <optional>
#include <sstream>
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

        std::ostream &PrintSelector(std::ostream &stream) const;

        [[nodiscard]] uint8_t GetSegmentSelector() const;
        [[nodiscard]] bool HasSegmentRegister() const;
        [[nodiscard]] Register GetSegmentRegister() const;

        void SetSegmentSelector(uint8_t sel);
        void SetSegmentRegister(Register reg);

    protected:
        const Platform &m_Platform;

        uint8_t m_SegmentSelector;
        std::optional<Register> m_SegmentRegister;
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

    class MemoryOperand : public Operand
    {
    public:
        MemoryOperand(
            const Platform &platform,
            int64_t displacement);

        MemoryOperand(
            const Platform &platform,
            int64_t displacement,
            std::optional<Register> base_register,
            std::optional<Register> index_register,
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

        std::optional<Register> m_BaseRegister;

        std::optional<Register> m_IndexRegister;

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

template<std::derived_from<scc::as::Operand> T>
struct std::formatter<const T *>
{
    template<typename C>
    static constexpr auto parse(C &&ctx)
    {
        return ctx.begin();
    }

    template<typename C>
    static auto format(const T *value, C &&ctx)
    {
        std::ostringstream stream;
        value->Print(stream);

        for (auto c : stream.view())
            *ctx.out()++ = c;

        return ctx.out();
    }
};
