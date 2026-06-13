#pragma once

#include <scc/ir/builder.hpp>
#include <scc/ir/module.hpp>

#include <cstdint>
#include <iosfwd>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

namespace scc::ir
{
    template<typename T>
    concept range = requires(T t)
    {
        { t.begin() };
        { t.end() };
    };

    enum class TokenType
    {
        EndOfFile,
        EndOfLine,
        Identifier,
        Integer,
        FloatingPoint,
        String,
        Other,
    };

    struct Token
    {
        TokenType Type{};
        std::string Value;
        uint64_t IntValue{};
        float64_t FloatValue{};
    };

    class Parser
    {
    public:
        explicit Parser(std::istream &stream, Context &context, Module &module);

        void Parse();

    protected:
        int Get();
        Token &Next();

        [[nodiscard]] bool At(TokenType type) const;
        [[nodiscard]] bool At(const std::vector<TokenType> &types) const;

        template<std::same_as<TokenType>... Types>
        [[nodiscard]] bool At(Types... types) const
        {
            return At(std::vector<TokenType>{ types... });
        }

        [[nodiscard]] bool At(TokenType type, std::string_view value) const;

        template<typename I>
        [[nodiscard]] bool At(TokenType type, const I &begin, const I &end) const
        {
            if (m_Token.Type != type)
                return false;

            for (auto it = begin; it != end; ++it)
                if (m_Token.Value == *it)
                    return true;

            return false;
        }

        template<range R>
        [[nodiscard]] bool At(TokenType type, const R &range) const
        {
            return At(type, range.begin(), range.end());
        }

        template<std::convertible_to<std::string_view>... V>
        [[nodiscard]] bool At(TokenType type, const V &... values) const
        {
            return At(type, std::vector<std::string_view>{ values... });
        }

        bool Skip(TokenType type);
        bool Skip(TokenType type, std::string_view value);

        Token Skip();

        Token Expect(TokenType type);
        Token Expect(const std::vector<TokenType> &types);

        template<std::same_as<TokenType>... Types>
        Token Expect(Types &&... types)
        {
            return Expect(std::vector<TokenType>{ types... });
        }

        Token Expect(TokenType type, std::string_view value);

        template<typename I>
        Token Expect(TokenType type, const I &begin, const I &end)
        {
            Assert(m_Token.Type == type, "unexpected token");

            for (auto it = begin; it != end; ++it)
                if (m_Token.Value == *it)
                    return Skip();

            Error("unexpected token");
        }

        template<range R>
        Token Expect(TokenType type, const R &range)
        {
            return Expect(type, range.begin(), range.end());
        }

        template<std::convertible_to<std::string_view>... V>
        Token Expect(TokenType type, const V &... values)
        {
            return Expect(type, std::vector<std::string_view>{ values... });
        }

        Type *ParseType();
        Constant *ParseConstant(Type *type);
        Value *ParseValue(Type *type);
        Instruction *ParseInstruction();

        Instruction *ParseBranchInstruction();
        Instruction *ParseReturnInstruction();
        Instruction *ParseStoreInstruction();

        Instruction *ParseLoadInstruction(std::string name);
        Instruction *ParseICompareInstruction(std::string name);
        Instruction *ParseIOperatorInstruction(std::string name);
        Instruction *ParseFCompareInstruction(std::string name);
        Instruction *ParseFOperatorInstruction(std::string name);
        Instruction *ParseCallInstruction(std::string name);
        Instruction *ParseElementInstruction(std::string name);
        Instruction *ParsePhiInstruction(std::string name);
        Instruction *ParseAllocInstruction(std::string name);
        Instruction *ParseCastInstruction(std::string name);

    private:
        std::istream &m_Stream;

        Context &m_Context;
        Module &m_Module;

        int m_Buffer;
        Token m_Token;

        Builder m_Builder;
    };

    std::ostream &operator<<(std::ostream &stream, TokenType type);
}
