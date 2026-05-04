#pragma once

#include <scc/ir/argument.hpp>
#include <scc/ir/block.hpp>
#include <scc/ir/global.hpp>

#include <iosfwd>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

namespace scc::ir
{
    class Function final : public Global
    {
    public:
        explicit Function(FunctionType *type, std::string name);
        ~Function() override;

        std::ostream &Print(std::ostream &stream) const override;

        void InsertBlock(std::unique_ptr<Block> block);
        Block *CreateBlock(std::string name);
        [[nodiscard]] Block *FindBlock(const std::string &name) const;

        [[nodiscard]] Value *FindValue(const std::string &name) const;

        [[nodiscard]] unsigned GetArgumentCount() const;
        [[nodiscard]] Argument *GetArgument(unsigned index) const;

        [[nodiscard]] std::unordered_set<Block *> GetPredecessors(const Block *block) const;

    private:
        Type *m_Result;
        bool m_Variadic;

        std::vector<std::unique_ptr<Argument>> m_Arguments;
        std::vector<std::unique_ptr<Block>> m_Blocks;
    };
}
