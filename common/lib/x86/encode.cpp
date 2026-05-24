#include <scc/platform/x86.hpp>

#include <toolkit/defer.hxx>

#define X86_MOD_MEM_NO_DISPLACEMENT 0b00
#define X86_MOD_MEM_8_BIT_DISPLACEMENT 0b01
#define X86_MOD_MEM_32_BIT_DISPLACEMENT 0b10
#define X86_MOD_REG 0b11

static void encode_imm(std::vector<uint8_t> &buffer, const uint64_t value, const unsigned bytes)
{
    for (auto i = 0u; i < bytes; ++i)
        buffer.push_back(value >> (i * 8u) & 0xFF);
}

static void encode_mod_rm_and_sib(
    std::vector<uint8_t> &buffer,
    const scc::platform::x86::Operand &reg_operand,
    const scc::platform::x86::Operand &rm_operand)
{
    uint8_t mod{}, rm{};
    const auto reg = reg_operand.Register;

    switch (rm_operand.Type)
    {
    case scc::platform::x86::OperandType::REG:
        mod = X86_MOD_REG;
        rm = rm_operand.Register;
        break;

    case scc::platform::x86::OperandType::MEM:
    {
        const auto &memory = rm_operand.Memory;

        mod = !memory.HasDisplacement
                  ? X86_MOD_MEM_NO_DISPLACEMENT
                  : memory.Displacement >= INT8_MIN && memory.Displacement <= INT8_MAX
                  ? X86_MOD_MEM_8_BIT_DISPLACEMENT
                  : X86_MOD_MEM_32_BIT_DISPLACEMENT;

        rm = memory.HasBaseRegister ? memory.BaseRegister : 0b101;

        if (rm == 0b100 || memory.HasIndexRegister)
        {
            rm = 0b100;

            const auto scale = memory.Scale;
            const auto index = memory.HasIndexRegister ? memory.IndexRegister : 0b100;
            const auto base = memory.HasBaseRegister ? memory.BaseRegister : 0b101;

            buffer.push_back((scale & 0b11) << 6 | (index & 0b111) << 3 | base & 0b111);
        }

        if (memory.HasDisplacement)
        {
            if (mod == X86_MOD_MEM_8_BIT_DISPLACEMENT)
                buffer.push_back(memory.Displacement);
            else if (mod == X86_MOD_MEM_32_BIT_DISPLACEMENT || rm == 0b101)
                encode_imm(buffer, memory.Displacement, 4);
        }

        break;
    }

    default:
        break;
    }

    buffer.push_back((mod & 0b11) << 6 | (reg & 0b111) << 3 | rm & 0b111);
}

toolkit::result<> scc::platform::x86::Encode(
    std::vector<uint8_t> &buffer,
    const InstructionTemplate &temp,
    const std::span<Operand> operands)
{
    const auto reset = buffer.size();

    auto guard0 = toolkit::defer(
        [](std::vector<uint8_t> &b, const size_t r)
        {
            b.resize(r);
        },
        buffer,
        reset);

    if (operands.size() != temp.OperandTypes.size())
        return toolkit::make_error(
            "operand count mismatch, require {}, got {}",
            temp.OperandTypes.size(),
            operands.size());

    for (size_t i = 0; i < operands.size(); ++i)
        if (operands[i].Type != temp.OperandTypes[i])
            return toolkit::make_error(
                "operand type mismatch at index {}, require {}, got {}",
                i,
                temp.OperandTypes[i],
                operands[i].Type);

    for (auto prefix : temp.Opcode_Prefix)
        buffer.push_back(prefix);

    auto opcode = temp.Opcode;

    if (temp.OpcodePlusReg)
    {
        if (temp.OpcodePlusReg_Operand < 0 || temp.OpcodePlusReg_Operand >= operands.size())
            return toolkit::make_error(
                "index out of bounds, opcode+reg {} outside range [{};{})",
                temp.OpcodePlusReg_Operand,
                0,
                operands.size());

        const auto &operand = operands[temp.OpcodePlusReg_Operand];

        if (operand.Type != OperandType::REG)
            return toolkit::make_error(
                "operand type mismatch at index {}, require {}, got {}",
                temp.OpcodePlusReg_Operand,
                OperandType::REG,
                operand.Type);

        opcode += operand.Register;
    }

    buffer.push_back(opcode);

    if (temp.UseModRM)
    {
        if (temp.ModRM_RegOperand < 0 || temp.ModRM_RegOperand >= operands.size())
            return toolkit::make_error(
                "index out of bounds, mod-r/m.reg {} outside range [{};{})",
                temp.ModRM_RegOperand,
                0,
                operands.size());

        if (temp.ModRM_RMOperand < 0 || temp.ModRM_RMOperand >= operands.size())
            return toolkit::make_error(
                "index out of bounds, mod-r/m.rm {} outside range [{};{})",
                temp.ModRM_RMOperand,
                0,
                operands.size());

        const auto &reg_operand = operands[temp.ModRM_RegOperand];
        const auto &rm_operand = operands[temp.ModRM_RMOperand];

        if (reg_operand.Type != OperandType::REG)
            return toolkit::make_error(
                "operand type mismatch at index {}, require {}, got {}",
                temp.ModRM_RegOperand,
                OperandType::REG,
                reg_operand.Type);

        encode_mod_rm_and_sib(buffer, reg_operand, rm_operand);
    }

    for (auto &rule : temp.ImmediateRules)
    {
        if (rule.Operand < 0 || rule.Operand >= operands.size())
            return toolkit::make_error(
                "index out of bounds, immediate.operand {} outside range [{};{})",
                rule.Operand,
                0,
                operands.size());

        const auto &operand = operands[rule.Operand];

        if (operand.Type != OperandType::IMM && operand.Type != OperandType::REL)
            return toolkit::make_error(
                "operand type mismatch at index {}, require {} or {}, got {}",
                rule.Operand,
                OperandType::IMM,
                OperandType::REL,
                operand.Type);

        encode_imm(buffer, operand.Immediate, 1 << rule.Size);
    }

    guard0.deactivate();
    return {};
}
