#pragma once

#include <scc/ir/builder.hpp>
#include <scc/ir/module.hpp>

#include <cstdint>
#include <iosfwd>
#include <string>
#include <string_view>
#include <vector>

namespace scc::ir
{
    enum class TokenType
    {
        EndOfFile,
        EndOfLine,
        Identifier,
        Integer,
        String,
        Other,
    };

    struct Token
    {
        TokenType Type{};
        std::string Value;
        uint64_t IntValue{};
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
        bool At(Types &&... types) const
        {
            return At(std::vector<TokenType>{ types... });
        }

        [[nodiscard]] bool At(TokenType type, std::string_view value) const;
        [[nodiscard]] bool At(TokenType type, const std::vector<std::string_view> &values) const;

        template<std::convertible_to<std::string_view>... Values>
        bool At(TokenType type, Values &&... values) const
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
        Token Expect(TokenType type, const std::vector<std::string_view> &values);

        template<std::convertible_to<std::string_view>... Values>
        Token Expect(TokenType type, Values &&... values)
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
        Instruction *ParseComparatorInstruction(std::string name);
        Instruction *ParseOperatorInstruction(std::string name);
        Instruction *ParseCallInstruction(std::string name);
        Instruction *ParseElementInstruction(std::string name);
        Instruction *ParseSelectInstruction(std::string name);
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
