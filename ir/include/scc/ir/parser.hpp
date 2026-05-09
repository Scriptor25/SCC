#pragma once

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

    struct Token final
    {
        TokenType Type{};
        std::string Value;
        uint64_t IntValue{};
    };

    class Parser final
    {
    public:
        explicit Parser(std::istream &stream);

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

        Module ParseModule(Context &context);

        Type *ParseType(Context &context);
        Constant *ParseConstant(Builder &builder, Type *type);
        Value *ParseValue(Module &module, Builder &builder, Type *type);
        Instruction *ParseInstruction(Module &module, Builder &builder);

        Instruction *ParseBranchInstruction(Module &module, Builder &builder);
        Instruction *ParseReturnInstruction(Module &module, Builder &builder);
        Instruction *ParseStoreInstruction(Module &module, Builder &builder);

        Instruction *ParseLoadInstruction(Module &module, Builder &builder, std::string name);
        Instruction *ParseComparatorInstruction(Module &module, Builder &builder, std::string name);
        Instruction *ParseOperatorInstruction(Module &module, Builder &builder, std::string name);
        Instruction *ParseCallInstruction(Module &module, Builder &builder, std::string name);
        Instruction *ParseElementInstruction(Module &module, Builder &builder, std::string name);
        Instruction *ParseSelectInstruction(Module &module, Builder &builder, std::string name);
        Instruction *ParseAllocInstruction(Module &module, Builder &builder, std::string name);
        Instruction *ParseCastInstruction(Module &module, Builder &builder, std::string name);

    private:
        std::istream &m_Stream;
        int m_Buffer;
        Token m_Token;
    };

    std::ostream &operator<<(std::ostream &stream, TokenType type);
}
