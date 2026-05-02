#pragma once

#include <scc/ir/module.hpp>

namespace scc::ir
{
    enum class TokenType
    {
        EndOfFile,
        EoL,
        Identifier,
        Integer,
        String,
        Other,
    };

    struct Token final
    {
        TokenType Type;
        std::string Value;
        uint64_t IntValue = 0ull;
    };

    template<typename T>
    concept TokenTypeLike = std::is_convertible_v<T, TokenType>;

    class Parser final
    {
    public:
        explicit Parser(std::istream &stream);

        int Get();
        Token &Next();

        [[nodiscard]] bool At(TokenType type) const;
        [[nodiscard]] bool At(TokenType type, std::string_view value) const;

        bool Skip(TokenType type);
        bool Skip(TokenType type, std::string_view value);

        Token Skip();

        Token Expect(TokenType type);
        Token Expect(TokenType type, std::string_view value);

        Token Expect(const std::vector<TokenType> &types);

        template<TokenTypeLike... Types>
        Token Expect(Types &&... types)
        {
            return Expect(std::vector<TokenType>{ types... });
        }

        Module ParseModule(Context &context);

        TypeFwd::Ptr ParseType(Context &context);
        ConstantFwd::Ptr ParseConstant(Builder &builder);
        ValueFwd::Ptr ParseValue(Module &module, Builder &builder);
        InstructionFwd::Ptr ParseInstruction(Module &module, Builder &builder);

    private:
        std::istream &m_Stream;
        int m_Buffer;
        Token m_Token;
    };
}

std::ostream &operator<<(std::ostream &stream, scc::ir::TokenType type);
