#include <scc/platform/riscv.hpp>

#include <scc/mnemonic.hpp>
#include <scc/register.hpp>

toolkit::result<scc::platform::InstructionSetArchitecture> scc::platform::riscv::CreateInstructionSetArchitecture(
    const Options &options)
{
    switch (options.BitWidth)
    {
    case 32:
    case 64:
        return InstructionSetArchitecture
        {
            .Architecture = TargetArchitecture::RiscV,
            .Endianness = TargetEndianness::LittleEndian,
            .BitWidth = options.BitWidth,
            .Registers = {
                { 0b00000, { .Code = 0b00000, .Class = RegisterClass::Special } },
                { 0b00001, { .Code = 0b00001, .Class = RegisterClass::Special } },
                { 0b00010, { .Code = 0b00010, .Class = RegisterClass::Special } },
                { 0b00011, { .Code = 0b00011, .Class = RegisterClass::Special } },
                { 0b00100, { .Code = 0b00100, .Class = RegisterClass::Special } },
                { 0b00101, { .Code = 0b00101, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                { 0b00110, { .Code = 0b00110, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                { 0b00111, { .Code = 0b00111, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                { 0b01000, { .Code = 0b01000, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                { 0b01001, { .Code = 0b01001, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                { 0b01010, { .Code = 0b01010, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                { 0b01011, { .Code = 0b01011, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                { 0b01100, { .Code = 0b01100, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                { 0b01101, { .Code = 0b01101, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                { 0b01110, { .Code = 0b01110, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                { 0b01111, { .Code = 0b01111, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                { 0b10000, { .Code = 0b10000, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                { 0b10001, { .Code = 0b10001, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                { 0b10010, { .Code = 0b10010, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                { 0b10011, { .Code = 0b10011, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                { 0b10100, { .Code = 0b10100, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                { 0b10101, { .Code = 0b10101, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                { 0b10110, { .Code = 0b10110, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                { 0b10111, { .Code = 0b10111, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                { 0b11000, { .Code = 0b11000, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                { 0b11001, { .Code = 0b11001, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                { 0b11010, { .Code = 0b11010, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                { 0b11011, { .Code = 0b11011, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                { 0b11100, { .Code = 0b11100, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                { 0b11101, { .Code = 0b11101, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                { 0b11110, { .Code = 0b11110, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                { 0b11111, { .Code = 0b11111, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
            },
            .RegisterViews = {
                { Register::RISCV_ZERO, { .Code = 0b00000, .Names = { "x0", "zero" } } },

                { Register::RISCV_RA, { .Code = 0b00001, .Names = { "x1", "ra" } } },
                { Register::RISCV_SP, { .Code = 0b00010, .Names = { "x2", "sp" } } },
                { Register::RISCV_GP, { .Code = 0b00011, .Names = { "x3", "gp" } } },
                { Register::RISCV_TP, { .Code = 0b00100, .Names = { "x4", "tp" } } },

                { Register::RISCV_T0, { .Code = 0b00101, .Names = { "x5", "t0" } } },
                { Register::RISCV_T1, { .Code = 0b00110, .Names = { "x6", "t1" } } },
                { Register::RISCV_T2, { .Code = 0b00111, .Names = { "x7", "t2" } } },

                { Register::RISCV_S0, { .Code = 0b01000, .Names = { "x8", "s0" } } },
                { Register::RISCV_S1, { .Code = 0b01001, .Names = { "x9", "s1" } } },

                { Register::RISCV_A0, { .Code = 0b01010, .Names = { "x10", "a0" } } },
                { Register::RISCV_A1, { .Code = 0b01011, .Names = { "x11", "a1" } } },
                { Register::RISCV_A2, { .Code = 0b01100, .Names = { "x12", "a2" } } },
                { Register::RISCV_A3, { .Code = 0b01101, .Names = { "x13", "a3" } } },
                { Register::RISCV_A4, { .Code = 0b01110, .Names = { "x14", "a4" } } },
                { Register::RISCV_A5, { .Code = 0b01111, .Names = { "x15", "a5" } } },
                { Register::RISCV_A6, { .Code = 0b10000, .Names = { "x16", "a6" } } },
                { Register::RISCV_A7, { .Code = 0b10001, .Names = { "x17", "a7" } } },

                { Register::RISCV_S2, { .Code = 0b10010, .Names = { "x18", "s2" } } },
                { Register::RISCV_S3, { .Code = 0b10011, .Names = { "x19", "s3" } } },
                { Register::RISCV_S4, { .Code = 0b10100, .Names = { "x20", "s4" } } },
                { Register::RISCV_S5, { .Code = 0b10101, .Names = { "x21", "s5" } } },
                { Register::RISCV_S6, { .Code = 0b10110, .Names = { "x22", "s6" } } },
                { Register::RISCV_S7, { .Code = 0b10111, .Names = { "x23", "s7" } } },
                { Register::RISCV_S8, { .Code = 0b11000, .Names = { "x24", "s8" } } },
                { Register::RISCV_S9, { .Code = 0b11001, .Names = { "x25", "s9" } } },
                { Register::RISCV_S10, { .Code = 0b11010, .Names = { "x26", "s10" } } },
                { Register::RISCV_S11, { .Code = 0b11011, .Names = { "x27", "s11" } } },

                { Register::RISCV_T3, { .Code = 0b11100, .Names = { "x28", "t3" } } },
                { Register::RISCV_T4, { .Code = 0b11101, .Names = { "x29", "t4" } } },
                { Register::RISCV_T5, { .Code = 0b11110, .Names = { "x30", "t5" } } },
                { Register::RISCV_T6, { .Code = 0b11111, .Names = { "x31", "t6" } } },
            },
            .Mnemonics = {
                { Mnemonic::RISCV_AND, { .Names = { "and" } } },
                { Mnemonic::RISCV_OR, { .Names = { "or" } } },
                { Mnemonic::RISCV_XOR, { .Names = { "xor" } } },

                { Mnemonic::RISCV_ANDI, { .Names = { "andi" } } },
                { Mnemonic::RISCV_ORI, { .Names = { "ori" } } },
                { Mnemonic::RISCV_XORI, { .Names = { "xori" } } },

                { Mnemonic::RISCV_SLL, { .Names = { "sll" } } },
                { Mnemonic::RISCV_SRL, { .Names = { "srl" } } },
                { Mnemonic::RISCV_SRA, { .Names = { "sra" } } },

                { Mnemonic::RISCV_SLLI, { .Names = { "slli" } } },
                { Mnemonic::RISCV_SRLI, { .Names = { "srli" } } },
                { Mnemonic::RISCV_SRAI, { .Names = { "srai" } } },

                { Mnemonic::RISCV_ADD, { .Names = { "add" } } },
                { Mnemonic::RISCV_SUB, { .Names = { "sub" } } },
                { Mnemonic::RISCV_ADDI, { .Names = { "addi" } } },
                { Mnemonic::RISCV_MUL, { .Names = { "mul" } } },
                { Mnemonic::RISCV_DIV, { .Names = { "div" } } },
                { Mnemonic::RISCV_DIVU, { .Names = { "divu" } } },
                { Mnemonic::RISCV_REM, { .Names = { "rem" } } },
                { Mnemonic::RISCV_REMU, { .Names = { "remu" } } },

                { Mnemonic::RISCV_LW, { .Names = { "lw" } } },
                { Mnemonic::RISCV_LH, { .Names = { "lh" } } },
                { Mnemonic::RISCV_LHU, { .Names = { "lhu" } } },
                { Mnemonic::RISCV_LB, { .Names = { "lb" } } },
                { Mnemonic::RISCV_LBU, { .Names = { "lbu" } } },

                { Mnemonic::RISCV_SW, { .Names = { "sw" } } },
                { Mnemonic::RISCV_SH, { .Names = { "sh" } } },
                { Mnemonic::RISCV_SB, { .Names = { "sb" } } },

                { Mnemonic::RISCV_BEQ, { .Names = { "beq" } } },
                { Mnemonic::RISCV_BNE, { .Names = { "bne" } } },
                { Mnemonic::RISCV_BLT, { .Names = { "blt" } } },
                { Mnemonic::RISCV_BLTU, { .Names = { "bltu" } } },
                { Mnemonic::RISCV_BGE, { .Names = { "bge" } } },
                { Mnemonic::RISCV_BGEU, { .Names = { "bgeu" } } },

                { Mnemonic::RISCV_JAL, { .Names = { "jal" } } },
                { Mnemonic::RISCV_JALR, { .Names = { "jalr" } } },

                { Mnemonic::RISCV_ECALL, { .Names = { "ecall" } } },
                { Mnemonic::RISCV_MRET, { .Names = { "mret" } } },

                { Mnemonic::RISCV_CSRRW, { .Names = { "csrrw" } } },

                { Mnemonic::RISCV_SLT, { .Names = { "slt" } } },
                { Mnemonic::RISCV_SLTU, { .Names = { "sltu" } } },

                { Mnemonic::RISCV_SLTI, { .Names = { "slti" } } },
                { Mnemonic::RISCV_SLTUI, { .Names = { "sltui" } } },
            },
        };

    default:
        return toolkit::make_error("invalid bitwidth {}", options.BitWidth);
    }
}
