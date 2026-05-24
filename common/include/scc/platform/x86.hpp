#pragma once

#include <scc/target.hpp>

#include <toolkit/result.hxx>

namespace scc::platform::x86
{
    enum class InstructionModRMMode
    {
        IMPLIED,
        REG_REG,
        REG_MEM,
        RM_ONLY,
    };

    enum class OperandType
    {
        NON,
        REG,
        MEM,
        IMM,
        REL,
    };

    struct ImmediateRule
    {
        size_t Operand;
        uint8_t Size;
        bool IsSigned;
    };

    struct InstructionTemplate
    {
        uint8_t Opcode;
        std::vector<uint8_t> Opcode_Prefix;

        bool OpcodePlusReg = false;
        int OpcodePlusReg_Operand = -1;

        bool UseModRM = false;

        InstructionModRMMode ModRM_Mode = InstructionModRMMode::IMPLIED;

        int ModRM_RegOperand = -1;
        int ModRM_RMOperand = -1;

        std::vector<ImmediateRule> ImmediateRules;
        std::vector<OperandType> OperandTypes;
    };

    struct MemoryOperand
    {
        bool HasBaseRegister = false;
        uint8_t BaseRegister = 0;

        bool HasIndexRegister = false;
        uint8_t IndexRegister = 0;

        uint8_t Scale = 0;

        bool HasDisplacement = false;
        int64_t Displacement = 0;
    };

    struct Operand
    {
        OperandType Type = OperandType::NON;

        /* REG */

        uint8_t Register{};

        /* MEM */

        MemoryOperand Memory;

        /* IMM */

        uint64_t Immediate{};
        uint8_t ImmediateSize{};
    };

    toolkit::result<> Encode(
        std::vector<uint8_t> &buffer,
        const InstructionTemplate &temp,
        std::span<Operand> operands);

    toolkit::result<IntermediateRepresentation> CreateIntermediateRepresentation(const Options &options);
    toolkit::result<InstructionSetArchitecture> CreateInstructionSetArchitecture(const Options &options);
}

template<>
struct std::formatter<scc::platform::x86::OperandType>
{
    template<typename C>
    static constexpr auto parse(C &&ctx)
    {
        return ctx.begin();
    }

    template<typename C>
    auto format(const scc::platform::x86::OperandType value, C &&ctx) const
    {
        static const std::unordered_map<scc::platform::x86::OperandType, const char *> map
        {
            { scc::platform::x86::OperandType::NON, "non" },
            { scc::platform::x86::OperandType::REG, "reg" },
            { scc::platform::x86::OperandType::MEM, "mem" },
            { scc::platform::x86::OperandType::IMM, "imm" },
            { scc::platform::x86::OperandType::REL, "rel" },
        };

        if (const auto it = map.find(value); it != map.end())
            return std::format_to(ctx.out(), "{}", it->second);

        return ctx.out();
    }
};
