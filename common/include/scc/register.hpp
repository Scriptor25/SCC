#pragma once

namespace scc
{
    enum class Register
    {
#pragma region X86

        X86_CS,
        X86_DS,
        X86_SS,
        X86_ES,
        X86_FS,
        X86_GS,

        X86_FLAGS,
        X86_EFLAGS,
        X86_RFLAGS,

        X86_IP,
        X86_EIP,
        X86_RIP,

        X86_AL,
        X86_AH,
        X86_AX,
        X86_EAX,
        X86_RAX,

        X86_BL,
        X86_BH,
        X86_BX,
        X86_EBX,
        X86_RBX,

        X86_CL,
        X86_CH,
        X86_CX,
        X86_ECX,
        X86_RCX,

        X86_DL,
        X86_DH,
        X86_DX,
        X86_EDX,
        X86_RDX,

        X86_SIL,
        X86_SI,
        X86_ESI,
        X86_RSI,

        X86_DIL,
        X86_DI,
        X86_EDI,
        X86_RDI,

        X86_BPL,
        X86_BP,
        X86_EBP,
        X86_RBP,

        X86_SPL,
        X86_SP,
        X86_ESP,
        X86_RSP,

        X86_R8B,
        X86_R8W,
        X86_R8D,
        X86_R8,

        X86_R9B,
        X86_R9W,
        X86_R9D,
        X86_R9,

        X86_R10B,
        X86_R10W,
        X86_R10D,
        X86_R10,

        X86_R11B,
        X86_R11W,
        X86_R11D,
        X86_R11,

        X86_R12B,
        X86_R12W,
        X86_R12D,
        X86_R12,

        X86_R13B,
        X86_R13W,
        X86_R13D,
        X86_R13,

        X86_R14B,
        X86_R14W,
        X86_R14D,
        X86_R14,

        X86_R15B,
        X86_R15W,
        X86_R15D,
        X86_R15,

#pragma endregion

#pragma region RISCV

        RISCV_ZERO,

        RISCV_RA,
        RISCV_SP,
        RISCV_GP,
        RISCV_TP,

        RISCV_T0,
        RISCV_T1,
        RISCV_T2,
        RISCV_T3,
        RISCV_T4,
        RISCV_T5,
        RISCV_T6,

        RISCV_S0,
        RISCV_S1,
        RISCV_S2,
        RISCV_S3,
        RISCV_S4,
        RISCV_S5,
        RISCV_S6,
        RISCV_S7,
        RISCV_S8,
        RISCV_S9,
        RISCV_S10,
        RISCV_S11,

        RISCV_A0,
        RISCV_A1,
        RISCV_A2,
        RISCV_A3,
        RISCV_A4,
        RISCV_A5,
        RISCV_A6,
        RISCV_A7,

#pragma endregion
    };
}
