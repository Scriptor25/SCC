#pragma once

#include <scc/common.hpp>

#include <bitset>
#include <cstdint>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

namespace scc::platform
{
    enum class TargetArchitecture
    {
        X86,
        PPC,
        ARM,
        RiscV,
    };

    enum class TargetSystem
    {
        None,
        Linux,
        Windows,
        Darwin,
    };

    enum class TargetEnvironment
    {
        EABI,
        GNU,
        MSVC,
        ELF,
        Android,
        MachO,
    };

    enum class TargetEndianness
    {
        LittleEndian,
        BigEndian,
    };

    enum class TargetCallingConvention
    {
        /**
         * cdecl
         * - arguments on stack, right-to-left
         * - result in %eax or %st0
         * - caller saves %eax, %ecx and %edx
         * - caller resets stack pointer
         */
        CDECL,

        /**
         * syscall
         * - arguments on stack, right-to-left
         * - result in %eax or %st0
         * - caller resets stack pointer
         * - argument count (in dwords) in %al
         */
        SYSCALL,

        /**
         * optlink
         * - arg0 in %eax or %st0, arg1 in %edx or %st1, arg2 in %ecx or %st2, arg3 in %st3, rest on stack, right-to-left
         * - floating-point arguments have reserved space on stack, even though they can be passed in registers
         * - result in %eax or %st0
         * - caller saves %ebp, %ebx, %esi and %edi
         * - caller resets stack pointer
         */
        OPTLINK,

        /**
         * pascal
         * - arguments on stack, left-to-right
         * - ordinal result in %al (8-bit), %ax (16-bit), %eax (32-bit) or %dx:%ax (2x16-bit)
         * - real result in %dx:%bx:%ax
         * - floating-point result in %st0
         * - pointer result in %ax (16-bit) or %eax (32-bit)
         * - string result at temporary address, pointed to by @Result
         * - callee resets stack pointer
         */
        PASCAL,

        /**
         * stdcall
         * - arguments on stack, right-to-left
         * - result in %eax
         * - callee resets stack pointer
         * - caller saves %eax, %ecx and %edx
         */
        STDCALL,

        /**
         * fastcall
         * - arg0 in %ecx, arg1 in %edx, rest on stack, right-to-left
         * - callee resets stack pointer
         */
        FASTCALL,

        /**
         * System-V
         * - arguments in:
         *   - %rdi, %rsi, %rdx, %rcx, %r8, %r9
         *   - static chain pointer in %r10
         *   - %xmm0 - %xmm7
         *   - rest on stack, right-to-left
         * - result in:
         *   - %rax (64-bit) or %rax:%rdx (128-bit)
         *   - %xmm0 and %xmm1
         */
        SystemV,

        /**
         * Microsoft x64
         */
        MSX64,

        /**
         * ARM Procedure Call Standard
         */
        AAPCS,

        /**
         * RiscV Call
         */
        RiscVCall,
    };

    enum class TargetObjectFormat
    {
        Raw,
        ELF,
        PE,
        COFF,
        MachO,
    };

    struct TargetDataLayout
    {
        size_t PointerSize;
        size_t PointerAlign;

        size_t MaxIntAlign;
        size_t MaxFloatAlign;
        size_t MaxVectorAlign;
        size_t MaxAggregateAlign;
    };

    struct TargetCallLayout
    {
        size_t StackAlign;
        bool HasRedZone;

        TargetCallingConvention CallingConvention;
    };

    enum class TargetRegisterClass
    {
        GeneralPurpose,
        FloatingPoint,
        Vector,
        Special,
    };

    struct TargetRegister
    {
        uint32_t Code;
        TargetRegisterClass Class;

        bool Allocatable;
    };

    struct TargetRegisterView
    {
        uint32_t Code;
        std::string_view Name;

        size_t BitWidth;
        size_t BitOffset;
    };

    struct TargetMnemonic
    {
        std::string_view Name;
    };

    struct TargetIR
    {
        std::unordered_set<size_t> LegalIntWidth;
        std::unordered_set<size_t> LegalFloatWidth;
    };

    struct TargetISA
    {
        TargetArchitecture Architecture;
        TargetEndianness Endianness;

        std::unordered_map<uint32_t, TargetRegister> Registers;
        std::unordered_map<Register, TargetRegisterView> RegisterViews;

        std::unordered_map<Mnemonic, TargetMnemonic> Mnemonics;

        std::unordered_map<std::string_view, Register> RegisterNames;
        std::unordered_map<std::string_view, Mnemonic> MnemonicNames;
    };

    struct TargetABI
    {
        TargetDataLayout DataLayout;
        TargetCallLayout CallLayout;

        TargetObjectFormat ObjectFormat;
    };

    using TargetFeatures = std::bitset<256>;

    struct TargetOptions
    {
    };

    /**
     * x86_64-linux-gnu, aarch64-linux-android, etc.
     *
     * {architecture}-{system}-{environment}
     */
    struct TargetTriple
    {
        static const std::unordered_map<std::string_view, TargetArchitecture> architectures;
        static const std::unordered_map<std::string_view, TargetSystem> systems;
        static const std::unordered_map<std::string_view, TargetEnvironment> environments;

        static const std::unordered_map<std::string_view, size_t> bit_widths;

        [[nodiscard]] std::string ToString() const;

        [[nodiscard]] Platform GetPlatform(const TargetOptions &options) const;

        [[nodiscard]] TargetIR GetIR() const;
        [[nodiscard]] TargetISA GetISA() const;
        [[nodiscard]] TargetABI GetABI() const;

        [[nodiscard]] TargetArchitecture GetArchitecture() const;
        [[nodiscard]] TargetSystem GetSystem() const;
        [[nodiscard]] TargetEnvironment GetEnvironment() const;

        [[nodiscard]] size_t GetBitWidth() const;
        [[nodiscard]] bool Is32Bit() const;
        [[nodiscard]] bool Is64Bit() const;

        /**
         * x86_64, i386, arm, thumb, mips, etc.
         */
        std::string Architecture;
        /**
         * none, linux, windows, darwin, cuda, etc.
         */
        std::string System;
        /**
         * eabi, gnu, android, macho, elf, etc.
         */
        std::string Environment;
    };

    TargetTriple ParseTriple(std::string_view triple);
}

namespace scc
{
    enum class Register
    {
        None,

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
    };

    enum class Mnemonic
    {
        X86_MOV,

        X86_ADD,
        X86_SUB,

        X86_IMUL,
        X86_DIV,

        X86_INC,
        X86_DEC,

        X86_XOR,

        X86_CALL,
        X86_RET,

        X86_PUSH,
        X86_POP,

        X86_JMP,

        X86_CMP,

        X86_JEQ,
        X86_JNE,
        X86_JLT,
        X86_JLE,
        X86_JGT,
        X86_JGE,

        X86_SYSCALL,
    };

    struct Platform
    {
        platform::TargetIR IR;
        platform::TargetISA ISA;
        platform::TargetABI ABI;
        platform::TargetFeatures Features;
    };
}
