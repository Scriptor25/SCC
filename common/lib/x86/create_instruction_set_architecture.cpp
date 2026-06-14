#include <scc/platform/x86.hpp>

#include <scc/merge.hpp>
#include <scc/mnemonic.hpp>
#include <scc/register.hpp>

toolkit::result<scc::platform::InstructionSetArchitecture> scc::platform::x86::CreateInstructionSetArchitecture(
    const Options &options)
{
    if (options.BitWidth != 16 && options.BitWidth != 32 && options.BitWidth != 64)
        return toolkit::make_error("invalid bitwidth {}", options.BitWidth);

    std::unordered_map<uint32_t, RegisterDescriptor> registers;
    std::unordered_map<Register, RegisterView> register_views;
    std::vector<InstructionForm> forms;
    std::unordered_map<Mnemonic, MnemonicDescriptor> mnemonics;

    if (options.BitWidth >= 16)
    {
        merge(
            registers,
            {
                // CS
                { 0b100000, { .Code = 0b100000, .Class = RegisterClass::Segment } },
                // DS
                { 0b100001, { .Code = 0b100001, .Class = RegisterClass::Segment } },
                // SS
                { 0b100010, { .Code = 0b100010, .Class = RegisterClass::Segment } },
                // ES
                { 0b100011, { .Code = 0b100011, .Class = RegisterClass::Segment } },

                // IP
                { 0b101000, { .Code = 0b101000, .Class = RegisterClass::Special } },

                // FLAGS
                { 0b101001, { .Code = 0b101001, .Class = RegisterClass::Special } },

                // A
                { 0b000, { .Code = 0b000, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                // C
                { 0b001, { .Code = 0b001, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                // D
                { 0b010, { .Code = 0b010, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                // B
                { 0b011, { .Code = 0b011, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                // SP
                { 0b100, { .Code = 0b100, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                // BP
                { 0b101, { .Code = 0b101, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                // SI
                { 0b110, { .Code = 0b110, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                // DI
                { 0b111, { .Code = 0b111, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
            });

        merge(
            register_views,
            {
                { Register::X86_CS, { .Code = 0b100000, .Names = { "cs" }, .BitWidth = 16 } },
                { Register::X86_DS, { .Code = 0b100001, .Names = { "ds" }, .BitWidth = 16 } },
                { Register::X86_SS, { .Code = 0b100010, .Names = { "ss" }, .BitWidth = 16 } },
                { Register::X86_ES, { .Code = 0b100011, .Names = { "es" }, .BitWidth = 16 } },

                { Register::X86_IP, { .Code = 0b101000, .Names = { "ip" }, .BitWidth = 16 } },
                { Register::X86_FLAGS, { .Code = 0b101001, .Names = { "flags" }, .BitWidth = 16 } },

                { Register::X86_AL, { .Code = 0b000, .Names = { "al" }, .BitWidth = 8 } },
                { Register::X86_AH, { .Code = 0b000, .Names = { "ah" }, .BitWidth = 8, .BitOffset = 8 } },
                { Register::X86_AX, { .Code = 0b000, .Names = { "ax" }, .BitWidth = 16 } },

                { Register::X86_CL, { .Code = 0b001, .Names = { "cl" }, .BitWidth = 8 } },
                { Register::X86_CH, { .Code = 0b001, .Names = { "ch" }, .BitWidth = 8, .BitOffset = 8 } },
                { Register::X86_CX, { .Code = 0b001, .Names = { "cx" }, .BitWidth = 16 } },

                { Register::X86_DL, { .Code = 0b010, .Names = { "dl" }, .BitWidth = 8 } },
                { Register::X86_DH, { .Code = 0b010, .Names = { "dh" }, .BitWidth = 8, .BitOffset = 8 } },
                { Register::X86_DX, { .Code = 0b010, .Names = { "dx" }, .BitWidth = 16 } },

                { Register::X86_BL, { .Code = 0b011, .Names = { "bl" }, .BitWidth = 8 } },
                { Register::X86_BH, { .Code = 0b011, .Names = { "bh" }, .BitWidth = 8, .BitOffset = 8 } },
                { Register::X86_BX, { .Code = 0b011, .Names = { "bx" }, .BitWidth = 16 } },

                { Register::X86_SPL, { .Code = 0b100, .Names = { "spl" }, .BitWidth = 8 } },
                { Register::X86_SP, { .Code = 0b100, .Names = { "sp" }, .BitWidth = 16 } },

                { Register::X86_BPL, { .Code = 0b101, .Names = { "bpl" }, .BitWidth = 8 } },
                { Register::X86_BP, { .Code = 0b101, .Names = { "bp" }, .BitWidth = 16 } },

                { Register::X86_SIL, { .Code = 0b110, .Names = { "sil" }, .BitWidth = 8 } },
                { Register::X86_SI, { .Code = 0b110, .Names = { "si" }, .BitWidth = 16 } },

                { Register::X86_DIL, { .Code = 0b111, .Names = { "dil" }, .BitWidth = 8 } },
                { Register::X86_DI, { .Code = 0b111, .Names = { "di" }, .BitWidth = 16 } },
            });

        constexpr OperandConstraint REG
        {
            .Kind = OperandKind::Register,
            .Class = RegisterClass::GeneralPurpose,
            .MinBitWidth = 8,
            .MaxBitWidth = 16,
        };

        constexpr OperandConstraint REG_AL
        {
            .Kind = OperandKind::Register,
            .Class = RegisterClass::GeneralPurpose,
            .Name = Register::X86_AL,
        };

        constexpr OperandConstraint REG_AX
        {
            .Kind = OperandKind::Register,
            .Class = RegisterClass::GeneralPurpose,
            .Name = Register::X86_AX,
        };

        constexpr OperandConstraint REG_CL
        {
            .Kind = OperandKind::Register,
            .Class = RegisterClass::GeneralPurpose,
            .Name = Register::X86_CL,
        };

        constexpr OperandConstraint REG_DX
        {
            .Kind = OperandKind::Register,
            .Class = RegisterClass::GeneralPurpose,
            .Name = Register::X86_DX,
        };

        // segment register
        constexpr OperandConstraint SREG
        {
            .Kind = OperandKind::Register,
            .Class = RegisterClass::Special,
            .MinBitWidth = 16,
            .MaxBitWidth = 16,
        };

        constexpr OperandConstraint MEM
        {
            .Kind = OperandKind::Memory,
            .MinBitWidth = 8,
            .MaxBitWidth = 16,
        };

        constexpr OperandConstraint IMM
        {
            .Kind = OperandKind::Immediate,
            .MinBitWidth = 8,
            .MaxBitWidth = 16,
        };

        constexpr OperandConstraint IMM8
        {
            .Kind = OperandKind::Immediate,
            .MinBitWidth = 8,
            .MaxBitWidth = 8,
        };

        constexpr OperandConstraint REL
        {
            .Kind = OperandKind::Relative,
            .MinBitWidth = 8,
            .MaxBitWidth = 16,
        };

        constexpr OperandConstraint REL8
        {
            .Kind = OperandKind::Relative,
            .MinBitWidth = 8,
            .MaxBitWidth = 8,
        };

        constexpr OperandConstraint SEL
        {
            .Kind = OperandKind::Selector,
            .MinBitWidth = 8,
            .MaxBitWidth = 16,
        };

        constexpr OperandConstraint SEL8
        {
            .Kind = OperandKind::Selector,
            .MinBitWidth = 8,
            .MaxBitWidth = 8,
        };

        constexpr OperandConstraint SEL16
        {
            .Kind = OperandKind::Selector,
            .MinBitWidth = 16,
            .MaxBitWidth = 16,
        };

        merge(
            forms,
            {
                // AAA
                {
                    .Name = Mnemonic::X86_AAA,
                },

                // AAD
                {
                    .Name = Mnemonic::X86_AAD,
                },

                // AAM
                {
                    .Name = Mnemonic::X86_AAM,
                },

                // AAS
                {
                    .Name = Mnemonic::X86_AAS,
                },

                // ADC
                {
                    .Name = Mnemonic::X86_ADC,
                    .Operands = { REG, REG },
                },
                {
                    .Name = Mnemonic::X86_ADC,
                    .Operands = { REG, MEM },
                },
                {
                    .Name = Mnemonic::X86_ADC,
                    .Operands = { REG, IMM },
                },
                {
                    .Name = Mnemonic::X86_ADC,
                    .Operands = { MEM, REG },
                },
                {
                    .Name = Mnemonic::X86_ADC,
                    .Operands = { MEM, IMM },
                },

                // ADD
                {
                    .Name = Mnemonic::X86_ADD,
                    .Operands = { REG, REG },
                },
                {
                    .Name = Mnemonic::X86_ADD,
                    .Operands = { REG, MEM },
                },
                {
                    .Name = Mnemonic::X86_ADD,
                    .Operands = { REG, IMM },
                },
                {
                    .Name = Mnemonic::X86_ADD,
                    .Operands = { MEM, REG },
                },
                {
                    .Name = Mnemonic::X86_ADD,
                    .Operands = { MEM, IMM },
                },

                // AND
                {
                    .Name = Mnemonic::X86_AND,
                    .Operands = { REG, REG },
                },
                {
                    .Name = Mnemonic::X86_AND,
                    .Operands = { REG, MEM },
                },
                {
                    .Name = Mnemonic::X86_AND,
                    .Operands = { REG, IMM },
                },
                {
                    .Name = Mnemonic::X86_AND,
                    .Operands = { MEM, REG },
                },
                {
                    .Name = Mnemonic::X86_AND,
                    .Operands = { MEM, IMM },
                },

                // CALL
                {
                    .Name = Mnemonic::X86_CALL,
                    .Operands = { REL },
                },
                {
                    .Name = Mnemonic::X86_CALL,
                    .Operands = { REG },
                },
                {
                    .Name = Mnemonic::X86_CALL,
                    .Operands = { MEM },
                },
                {
                    .Name = Mnemonic::X86_CALL,
                    .Operands = { SEL },
                },

                // CBW
                {
                    .Name = Mnemonic::X86_CBW,
                },

                // CLC
                {
                    .Name = Mnemonic::X86_CLC,
                },

                // CLD
                {
                    .Name = Mnemonic::X86_CLD,
                },

                // CLI
                {
                    .Name = Mnemonic::X86_CLI,
                },

                // CMC
                {
                    .Name = Mnemonic::X86_CMC,
                },

                // CMP
                {
                    .Name = Mnemonic::X86_CMP,
                    .Operands = { REG, REG },
                },
                {
                    .Name = Mnemonic::X86_CMP,
                    .Operands = { REG, MEM },
                },
                {
                    .Name = Mnemonic::X86_CMP,
                    .Operands = { REG, IMM },
                },
                {
                    .Name = Mnemonic::X86_CMP,
                    .Operands = { MEM, REG },
                },
                {
                    .Name = Mnemonic::X86_CMP,
                    .Operands = { MEM, IMM },
                },

                // CMPSB
                {
                    .Name = Mnemonic::X86_CMPSB,
                },

                // CMPSW
                {
                    .Name = Mnemonic::X86_CMPSW,
                },

                // CWD
                {
                    .Name = Mnemonic::X86_CWD,
                },

                // DAA
                {
                    .Name = Mnemonic::X86_DAA,
                },

                // DAS
                {
                    .Name = Mnemonic::X86_DAS,
                },

                // DEC
                {
                    .Name = Mnemonic::X86_DEC,
                    .Operands = { REG },
                },
                {
                    .Name = Mnemonic::X86_DEC,
                    .Operands = { MEM },
                },

                // DIV
                {
                    .Name = Mnemonic::X86_DIV,
                    .Operands = { REG },
                },
                {
                    .Name = Mnemonic::X86_DIV,
                    .Operands = { MEM },
                },

                // ESC
                {
                    .Name = Mnemonic::X86_ESC,
                },

                // HLT
                {
                    .Name = Mnemonic::X86_HLT,
                },

                // IDIV
                {
                    .Name = Mnemonic::X86_IDIV,
                    .Operands = { REG },
                },
                {
                    .Name = Mnemonic::X86_IDIV,
                    .Operands = { MEM },
                },

                // IMUL
                {
                    .Name = Mnemonic::X86_IMUL,
                    .Operands = { REG },
                },
                {
                    .Name = Mnemonic::X86_IMUL,
                    .Operands = { MEM },
                },

                // IN
                {
                    .Name = Mnemonic::X86_IN,
                    .Operands = { REG_AL, IMM8 },
                },
                {
                    .Name = Mnemonic::X86_IN,
                    .Operands = { REG_AX, IMM8 },
                },
                {
                    .Name = Mnemonic::X86_IN,
                    .Operands = { REG_AL, REG_DX },
                },
                {
                    .Name = Mnemonic::X86_IN,
                    .Operands = { REG_AX, REG_DX },
                },

                // INC
                {
                    .Name = Mnemonic::X86_INC,
                    .Operands = { REG },
                },
                {
                    .Name = Mnemonic::X86_INC,
                    .Operands = { MEM },
                },

                // INT
                {
                    .Name = Mnemonic::X86_INT,
                    .Operands = { IMM8 },
                },

                // INTO
                {
                    .Name = Mnemonic::X86_INTO,
                },

                // IRET
                {
                    .Name = Mnemonic::X86_IRET,
                },

                // JO
                {
                    .Name = Mnemonic::X86_JO,
                    .Operands = { REL },
                },

                // JNO
                {
                    .Name = Mnemonic::X86_JNO,
                    .Operands = { REL },
                },

                // JC
                {
                    .Name = Mnemonic::X86_JC,
                    .Operands = { REL },
                },

                // JNC
                {
                    .Name = Mnemonic::X86_JNC,
                    .Operands = { REL },
                },

                // JZ
                {
                    .Name = Mnemonic::X86_JZ,
                    .Operands = { REL },
                },

                // JNZ
                {
                    .Name = Mnemonic::X86_JNZ,
                    .Operands = { REL },
                },

                // JNA
                {
                    .Name = Mnemonic::X86_JNA,
                    .Operands = { REL },
                },

                // JA
                {
                    .Name = Mnemonic::X86_JA,
                    .Operands = { REL },
                },

                // JS
                {
                    .Name = Mnemonic::X86_JS,
                    .Operands = { REL },
                },

                // JNS
                {
                    .Name = Mnemonic::X86_JNS,
                    .Operands = { REL },
                },

                // JP
                {
                    .Name = Mnemonic::X86_JP,
                    .Operands = { REL },
                },

                // JNP
                {
                    .Name = Mnemonic::X86_JNP,
                    .Operands = { REL },
                },

                // JL
                {
                    .Name = Mnemonic::X86_JL,
                    .Operands = { REL },
                },

                // JNL
                {
                    .Name = Mnemonic::X86_JNL,
                    .Operands = { REL },
                },

                // JLE
                {
                    .Name = Mnemonic::X86_JLE,
                    .Operands = { REL },
                },

                // JNLE
                {
                    .Name = Mnemonic::X86_JNLE,
                    .Operands = { REL },
                },

                // JCXZ
                {
                    .Name = Mnemonic::X86_JCXZ,
                    .Operands = { REL },
                },

                // JMP
                {
                    .Name = Mnemonic::X86_JMP,
                    .Operands = { REL },
                },
                {
                    .Name = Mnemonic::X86_JMP,
                    .Operands = { REG },
                },
                {
                    .Name = Mnemonic::X86_JMP,
                    .Operands = { MEM },
                },
                {
                    .Name = Mnemonic::X86_JMP,
                    .Operands = { SEL },
                },

                // LAHF
                {
                    .Name = Mnemonic::X86_LAHF,
                },

                // LDS
                {
                    .Name = Mnemonic::X86_LDS,
                    .Operands = { REG, SEL },
                },

                // LEA
                {
                    .Name = Mnemonic::X86_LEA,
                    .Operands = { REG, MEM },
                },

                // LES
                {
                    .Name = Mnemonic::X86_LES,
                    .Operands = { REG, SEL },
                },

                // LOCK
                {
                    .Name = Mnemonic::X86_LOCK,
                },

                // LODSB
                {
                    .Name = Mnemonic::X86_LODSB,
                },

                // LODSW
                {
                    .Name = Mnemonic::X86_LODSW,
                },

                // LOOP
                {
                    .Name = Mnemonic::X86_LOOP,
                    .Operands = { REL8 },
                },

                // LOOPZ
                {
                    .Name = Mnemonic::X86_LOOPZ,
                    .Operands = { REL8 },
                },

                // LOOPNZ
                {
                    .Name = Mnemonic::X86_LOOPNZ,
                    .Operands = { REL8 },
                },

                // MOV
                {
                    .Name = Mnemonic::X86_MOV,
                    .Operands = { REG, REG },
                },
                {
                    .Name = Mnemonic::X86_MOV,
                    .Operands = { REG, MEM },
                },
                {
                    .Name = Mnemonic::X86_MOV,
                    .Operands = { REG, IMM },
                },
                {
                    .Name = Mnemonic::X86_MOV,
                    .Operands = { MEM, REG },
                },
                {
                    .Name = Mnemonic::X86_MOV,
                    .Operands = { MEM, IMM },
                },
                {
                    .Name = Mnemonic::X86_MOV,
                    .Operands = { REG, SREG },
                },
                {
                    .Name = Mnemonic::X86_MOV,
                    .Operands = { MEM, SREG },
                },
                {
                    .Name = Mnemonic::X86_MOV,
                    .Operands = { SREG, REG },
                },
                {
                    .Name = Mnemonic::X86_MOV,
                    .Operands = { SREG, MEM },
                },
                {
                    .Name = Mnemonic::X86_MOV,
                    .Operands = { REG_AL, SEL8 },
                },
                {
                    .Name = Mnemonic::X86_MOV,
                    .Operands = { REG_AX, SEL16 },
                },
                {
                    .Name = Mnemonic::X86_MOV,
                    .Operands = { SEL8, REG_AL },
                },
                {
                    .Name = Mnemonic::X86_MOV,
                    .Operands = { SEL16, REG_AX },
                },

                {
                    .Name = Mnemonic::X86_MOVSB,
                },
                {
                    .Name = Mnemonic::X86_MOVSW,
                },

                {
                    .Name = Mnemonic::X86_MUL,
                    .Operands = { REG },
                },
                {
                    .Name = Mnemonic::X86_MUL,
                    .Operands = { MEM },
                },

                // NEG
                {
                    .Name = Mnemonic::X86_NEG,
                    .Operands = { REG },
                },
                {
                    .Name = Mnemonic::X86_NEG,
                    .Operands = { MEM },
                },

                // NOP
                {
                    .Name = Mnemonic::X86_NOP,
                },

                // NOT
                {
                    .Name = Mnemonic::X86_NOT,
                    .Operands = { REG },
                },
                {
                    .Name = Mnemonic::X86_NOT,
                    .Operands = { MEM },
                },

                // OR
                {
                    .Name = Mnemonic::X86_OR,
                    .Operands = { REG, REG },
                },
                {
                    .Name = Mnemonic::X86_OR,
                    .Operands = { REG, MEM },
                },
                {
                    .Name = Mnemonic::X86_OR,
                    .Operands = { REG, IMM },
                },
                {
                    .Name = Mnemonic::X86_OR,
                    .Operands = { MEM, REG },
                },
                {
                    .Name = Mnemonic::X86_OR,
                    .Operands = { MEM, IMM },
                },

                // OUT
                {
                    .Name = Mnemonic::X86_OUT,
                    .Operands = { IMM8, REG_AL },
                },
                {
                    .Name = Mnemonic::X86_OUT,
                    .Operands = { IMM8, REG_AX },
                },
                {
                    .Name = Mnemonic::X86_OUT,
                    .Operands = { REG_DX, REG_AL },
                },

                {
                    .Name = Mnemonic::X86_OUT,
                    .Operands = { REG_DX, REG_AX },
                },

                {
                    .Name = Mnemonic::X86_POP,
                    .Operands = { REG },
                },
                {
                    .Name = Mnemonic::X86_POP,
                    .Operands = { MEM },
                },
                {
                    .Name = Mnemonic::X86_POP,
                    .Operands = { SREG },
                },

                {
                    .Name = Mnemonic::X86_POPF,
                },

                {
                    .Name = Mnemonic::X86_PUSH,
                    .Operands = { REG },
                },
                {
                    .Name = Mnemonic::X86_PUSH,
                    .Operands = { MEM },
                },
                {
                    .Name = Mnemonic::X86_PUSH,
                    .Operands = { SREG },
                },

                {
                    .Name = Mnemonic::X86_PUSHF,
                },

                {
                    .Name = Mnemonic::X86_RCL,
                    .Operands = { REG, IMM },
                },
                {
                    .Name = Mnemonic::X86_RCR,
                    .Operands = { REG, IMM },
                },

                {
                    .Name = Mnemonic::X86_REPZ,
                },
                {
                    .Name = Mnemonic::X86_REPNZ,
                },

                {
                    .Name = Mnemonic::X86_RETN,
                },
                {
                    .Name = Mnemonic::X86_RETF,
                },

                {
                    .Name = Mnemonic::X86_ROL,
                    .Operands = { REG, IMM },
                },
                {
                    .Name = Mnemonic::X86_ROR,
                    .Operands = { REG, IMM },
                },

                {
                    .Name = Mnemonic::X86_SAHF,
                },

                {
                    .Name = Mnemonic::X86_SAL,
                    .Operands = { REG },
                },
                {
                    .Name = Mnemonic::X86_SAL,
                    .Operands = { MEM },
                },
                {
                    .Name = Mnemonic::X86_SAL,
                    .Operands = { REG, REG_CL },
                },
                {
                    .Name = Mnemonic::X86_SAL,
                    .Operands = { MEM, REG_CL },
                },

                {
                    .Name = Mnemonic::X86_SAR,
                    .Operands = { REG },
                },
                {
                    .Name = Mnemonic::X86_SAR,
                    .Operands = { MEM },
                },
                {
                    .Name = Mnemonic::X86_SAR,
                    .Operands = { REG, REG_CL },
                },
                {
                    .Name = Mnemonic::X86_SAR,
                    .Operands = { MEM, REG_CL },
                },
            });

        merge(
            mnemonics,
            {
                { Mnemonic::X86_AAA, { .Names = { "aaa" } } },
                { Mnemonic::X86_AAD, { .Names = { "aad" } } },
                { Mnemonic::X86_AAM, { .Names = { "aam" } } },
                { Mnemonic::X86_AAS, { .Names = { "aas" } } },
                { Mnemonic::X86_ADC, { .Names = { "adc" } } },
                { Mnemonic::X86_ADD, { .Names = { "add" } } },
                { Mnemonic::X86_AND, { .Names = { "and" } } },
                { Mnemonic::X86_CALL, { .Names = { "call" } } },
                { Mnemonic::X86_CBW, { .Names = { "cbw" } } },
                { Mnemonic::X86_CLC, { .Names = { "clc" } } },
                { Mnemonic::X86_CLD, { .Names = { "cld" } } },
                { Mnemonic::X86_CLI, { .Names = { "cli" } } },
                { Mnemonic::X86_CMC, { .Names = { "cmc" } } },
                { Mnemonic::X86_CMP, { .Names = { "cmp" } } },
                { Mnemonic::X86_CMPSB, { .Names = { "cmpsb" } } },
                { Mnemonic::X86_CMPSW, { .Names = { "cmpsw" } } },
                { Mnemonic::X86_CWD, { .Names = { "cwd" } } },
                { Mnemonic::X86_DAA, { .Names = { "daa" } } },
                { Mnemonic::X86_DAS, { .Names = { "das" } } },
                { Mnemonic::X86_DEC, { .Names = { "dec" } } },
                { Mnemonic::X86_DIV, { .Names = { "div" } } },
                { Mnemonic::X86_ESC, { .Names = { "esc" } } },
                { Mnemonic::X86_HLT, { .Names = { "hlt" } } },
                { Mnemonic::X86_IDIV, { .Names = { "idiv" } } },
                { Mnemonic::X86_IMUL, { .Names = { "imul" } } },
                { Mnemonic::X86_IN, { .Names = { "in" } } },
                { Mnemonic::X86_INC, { .Names = { "inc" } } },
                { Mnemonic::X86_INT, { .Names = { "int" } } },
                { Mnemonic::X86_INTO, { .Names = { "into" } } },
                { Mnemonic::X86_IRET, { .Names = { "iret" } } },

                { Mnemonic::X86_JO, { .Names = { "jo" } } },
                { Mnemonic::X86_JNO, { .Names = { "jno" } } },
                { Mnemonic::X86_JC, { .Names = { "jc", "jb", "jnae" } } },
                { Mnemonic::X86_JNC, { .Names = { "jnc", "jnb", "jae" } } },
                { Mnemonic::X86_JZ, { .Names = { "jz", "je" } } },
                { Mnemonic::X86_JNZ, { .Names = { "jnz", "jne" } } },
                { Mnemonic::X86_JNA, { .Names = { "jna", "jbe" } } },
                { Mnemonic::X86_JA, { .Names = { "ja", "jnbe" } } },
                { Mnemonic::X86_JS, { .Names = { "js" } } },
                { Mnemonic::X86_JNS, { .Names = { "jns" } } },
                { Mnemonic::X86_JP, { .Names = { "jp", "jpe" } } },
                { Mnemonic::X86_JNP, { .Names = { "jnp", "jpo" } } },
                { Mnemonic::X86_JL, { .Names = { "jl", "jnge" } } },
                { Mnemonic::X86_JNL, { .Names = { "jnl", "jge" } } },
                { Mnemonic::X86_JLE, { .Names = { "jle", "jng" } } },
                { Mnemonic::X86_JNLE, { .Names = { "jnle", "jg" } } },

                { Mnemonic::X86_JCXZ, { .Names = { "jcxz" } } },

                { Mnemonic::X86_JMP, { .Names = { "jmp" } } },
                { Mnemonic::X86_LAHF, { .Names = { "lahf" } } },
                { Mnemonic::X86_LDS, { .Names = { "lds" } } },
                { Mnemonic::X86_LEA, { .Names = { "lea" } } },
                { Mnemonic::X86_LES, { .Names = { "les" } } },
                { Mnemonic::X86_LOCK, { .Names = { "lock" } } },
                { Mnemonic::X86_LODSB, { .Names = { "lodsb" } } },
                { Mnemonic::X86_LODSW, { .Names = { "lodsw" } } },
                { Mnemonic::X86_LOOP, { .Names = { "loop" } } },
                { Mnemonic::X86_LOOPZ, { .Names = { "loopz", "loope" } } },
                { Mnemonic::X86_LOOPNZ, { .Names = { "loopnz", "loopne" } } },
                { Mnemonic::X86_MOV, { .Names = { "mov" } } },
                { Mnemonic::X86_MOVSB, { .Names = { "movsb" } } },
                { Mnemonic::X86_MOVSW, { .Names = { "movsw" } } },
                { Mnemonic::X86_MUL, { .Names = { "mul" } } },
                { Mnemonic::X86_NEG, { .Names = { "neg" } } },
                { Mnemonic::X86_NOP, { .Names = { "nop" } } },
                { Mnemonic::X86_NOT, { .Names = { "not" } } },
                { Mnemonic::X86_OR, { .Names = { "or" } } },
                { Mnemonic::X86_OUT, { .Names = { "out" } } },
                { Mnemonic::X86_POP, { .Names = { "pop" } } },
                { Mnemonic::X86_POPF, { .Names = { "popf" } } },
                { Mnemonic::X86_PUSH, { .Names = { "push" } } },
                { Mnemonic::X86_PUSHF, { .Names = { "pushf" } } },
                { Mnemonic::X86_RCL, { .Names = { "rcl" } } },
                { Mnemonic::X86_RCR, { .Names = { "rcr" } } },

                { Mnemonic::X86_REPZ, { .Names = { "repz", "rep", "repe" } } },
                { Mnemonic::X86_REPNZ, { .Names = { "repnz", "repne" } } },

                { Mnemonic::X86_RETN, { .Names = { "retn" } } },
                { Mnemonic::X86_RETF, { .Names = { "retf" } } },
                { Mnemonic::X86_ROL, { .Names = { "rol" } } },
                { Mnemonic::X86_ROR, { .Names = { "ror" } } },
                { Mnemonic::X86_SAHF, { .Names = { "sahf" } } },
                { Mnemonic::X86_SAL, { .Names = { "sal" } } },
                { Mnemonic::X86_SAR, { .Names = { "sar" } } },
                { Mnemonic::X86_SBB, { .Names = { "sbb" } } },
                { Mnemonic::X86_SCASB, { .Names = { "scasb" } } },
                { Mnemonic::X86_SCASW, { .Names = { "scasw" } } },
                { Mnemonic::X86_SHL, { .Names = { "shl" } } },
                { Mnemonic::X86_SHR, { .Names = { "shr" } } },
                { Mnemonic::X86_STC, { .Names = { "stc" } } },
                { Mnemonic::X86_STD, { .Names = { "std" } } },
                { Mnemonic::X86_STI, { .Names = { "sti" } } },
                { Mnemonic::X86_STOSB, { .Names = { "stosb" } } },
                { Mnemonic::X86_STOSW, { .Names = { "stosw" } } },
                { Mnemonic::X86_SUB, { .Names = { "sub" } } },
                { Mnemonic::X86_TEST, { .Names = { "test" } } },
                { Mnemonic::X86_WAIT, { .Names = { "wait" } } },
                { Mnemonic::X86_XCHG, { .Names = { "xchg" } } },
                { Mnemonic::X86_XLAT, { .Names = { "xlat" } } },
                { Mnemonic::X86_XOR, { .Names = { "xor" } } },

                { Mnemonic::X86_PUSHA, { .Names = { "pusha" } } },
                { Mnemonic::X86_POPA, { .Names = { "popa" } } },
                { Mnemonic::X86_BOUND, { .Names = { "bound" } } },
                { Mnemonic::X86_IIMUL, { .Names = { "iimul" } } },
                { Mnemonic::X86_INSB, { .Names = { "insb" } } },
                { Mnemonic::X86_INSW, { .Names = { "insw" } } },
                { Mnemonic::X86_OUTSB, { .Names = { "outsb" } } },
                { Mnemonic::X86_OUTSW, { .Names = { "outsw" } } },
                { Mnemonic::X86_ENTER, { .Names = { "enter" } } },
                { Mnemonic::X86_LEAVE, { .Names = { "leave" } } },

                { Mnemonic::X86_LGDT, { .Names = { "lgdt" } } },
                { Mnemonic::X86_LIDT, { .Names = { "lidt" } } },
                { Mnemonic::X86_LMSW, { .Names = { "lmsw" } } },
                { Mnemonic::X86_CLTS, { .Names = { "clts" } } },
                { Mnemonic::X86_LLDT, { .Names = { "lldt" } } },
                { Mnemonic::X86_LTR, { .Names = { "ltr" } } },
                { Mnemonic::X86_SGDT, { .Names = { "sgdt" } } },
                { Mnemonic::X86_SIDT, { .Names = { "sidt" } } },
                { Mnemonic::X86_SMSW, { .Names = { "smsw" } } },
                { Mnemonic::X86_SLDT, { .Names = { "sldt" } } },
                { Mnemonic::X86_STR, { .Names = { "str" } } },
                { Mnemonic::X86_ARPL, { .Names = { "arpl" } } },
                { Mnemonic::X86_LAR, { .Names = { "lar" } } },
                { Mnemonic::X86_LSL, { .Names = { "lsl" } } },
                { Mnemonic::X86_VERR, { .Names = { "verr" } } },
                { Mnemonic::X86_VERW, { .Names = { "verw" } } },
                { Mnemonic::X86_LOADALL, { .Names = { "loadall" } } },
                { Mnemonic::X86_STOREALL, { .Names = { "storeall" } } },
            });
    }

    if (options.BitWidth >= 32)
    {
        merge(
            registers,
            {
                // FS
                { 0b100100, { .Code = 0b100100, .Class = RegisterClass::Segment } },
                // GS
                { 0b100101, { .Code = 0b100101, .Class = RegisterClass::Segment } },
            });

        merge(
            register_views,
            {
                { Register::X86_FS, { .Code = 0b100100, .Names = { "fs" }, .BitWidth = 16 } },
                { Register::X86_GS, { .Code = 0b100101, .Names = { "gs" }, .BitWidth = 16 } },

                { Register::X86_EIP, { .Code = 0b101000, .Names = { "eip" }, .BitWidth = 32 } },
                { Register::X86_EFLAGS, { .Code = 0b101001, .Names = { "eflags" }, .BitWidth = 32 } },

                { Register::X86_EAX, { .Code = 0b000, .Names = { "eax" }, .BitWidth = 32 } },
                { Register::X86_ECX, { .Code = 0b001, .Names = { "ecx" }, .BitWidth = 32 } },
                { Register::X86_EDX, { .Code = 0b010, .Names = { "edx" }, .BitWidth = 32 } },
                { Register::X86_EBX, { .Code = 0b011, .Names = { "ebx" }, .BitWidth = 32 } },
                { Register::X86_ESP, { .Code = 0b100, .Names = { "esp" }, .BitWidth = 32 } },
                { Register::X86_EBP, { .Code = 0b101, .Names = { "ebp" }, .BitWidth = 32 } },
                { Register::X86_ESI, { .Code = 0b110, .Names = { "esi" }, .BitWidth = 32 } },
                { Register::X86_EDI, { .Code = 0b111, .Names = { "edi" }, .BitWidth = 32 } },
            });

        merge(
            mnemonics,
            {
                { Mnemonic::X86_JECXZ, { .Names = { "jecxz" } } },

                { Mnemonic::X86_LODSD, { .Names = { "lodsd" } } },
                { Mnemonic::X86_STOSD, { .Names = { "stosd" } } },
                { Mnemonic::X86_MOVSD, { .Names = { "movsd" } } },
                { Mnemonic::X86_CMPSD, { .Names = { "cmpsd" } } },
                { Mnemonic::X86_SCASD, { .Names = { "scasd" } } },
                { Mnemonic::X86_INSD, { .Names = { "insd" } } },
                { Mnemonic::X86_OUTSD, { .Names = { "outsd" } } },
                { Mnemonic::X86_CWDE, { .Names = { "cwde" } } },
                { Mnemonic::X86_CDQ, { .Names = { "cdq" } } },
                { Mnemonic::X86_PUSHAD, { .Names = { "pushad" } } },
                { Mnemonic::X86_POPAD, { .Names = { "popad" } } },
                { Mnemonic::X86_PUSHFD, { .Names = { "pushfd" } } },
                { Mnemonic::X86_POPFD, { .Names = { "popfd" } } },
                { Mnemonic::X86_IRETD, { .Names = { "iretd" } } },
                { Mnemonic::X86_BT, { .Names = { "bt" } } },
                { Mnemonic::X86_BTS, { .Names = { "bts" } } },
                { Mnemonic::X86_BTR, { .Names = { "btr" } } },
                { Mnemonic::X86_BTC, { .Names = { "btc" } } },
                { Mnemonic::X86_BSF, { .Names = { "bsf" } } },
                { Mnemonic::X86_BSR, { .Names = { "bsr" } } },
                { Mnemonic::X86_SHLD, { .Names = { "shld" } } },
                { Mnemonic::X86_SHRD, { .Names = { "shrd" } } },
                { Mnemonic::X86_MOVZX, { .Names = { "movzx" } } },
                { Mnemonic::X86_MOVSX, { .Names = { "movsx" } } },

                { Mnemonic::X86_SETO, { .Names = { "seto" } } },
                { Mnemonic::X86_SETNO, { .Names = { "setno" } } },
                { Mnemonic::X86_SETC, { .Names = { "setc", "setb", "setnae" } } },
                { Mnemonic::X86_SETNC, { .Names = { "setnc", "setnb", "setae" } } },
                { Mnemonic::X86_SETZ, { .Names = { "setz", "sete" } } },
                { Mnemonic::X86_SETNZ, { .Names = { "setnz", "setne" } } },
                { Mnemonic::X86_SETNA, { .Names = { "setna", "setbe" } } },
                { Mnemonic::X86_SETA, { .Names = { "seta", "setnbe" } } },
                { Mnemonic::X86_SETS, { .Names = { "sets" } } },
                { Mnemonic::X86_SETNS, { .Names = { "setns" } } },
                { Mnemonic::X86_SETP, { .Names = { "setp", "setpe" } } },
                { Mnemonic::X86_SETNP, { .Names = { "setnp", "setpo" } } },
                { Mnemonic::X86_SETL, { .Names = { "setl", "setnge" } } },
                { Mnemonic::X86_SETNL, { .Names = { "setnl", "setge" } } },
                { Mnemonic::X86_SETLE, { .Names = { "setle", "setng" } } },
                { Mnemonic::X86_SETNLE, { .Names = { "setnle", "setg" } } },

                { Mnemonic::X86_LFS, { .Names = { "lfs" } } },
                { Mnemonic::X86_LGS, { .Names = { "lgs" } } },
                { Mnemonic::X86_LSS, { .Names = { "lss" } } },
                { Mnemonic::X86_ICEBP, { .Names = { "icebp" } } },
                { Mnemonic::X86_INT01, { .Names = { "int01" } } },
                { Mnemonic::X86_INT1, { .Names = { "int1" } } },
                { Mnemonic::X86_UMOV, { .Names = { "umov" } } },
                { Mnemonic::X86_XBTS, { .Names = { "xbts" } } },
                { Mnemonic::X86_IBTS, { .Names = { "ibts" } } },
                { Mnemonic::X86_LOADALLD, { .Names = { "loadalld" } } },
                { Mnemonic::X86_LOADALL386, { .Names = { "loadall386" } } },

                { Mnemonic::X86_BSWAP, { .Names = { "bswap" } } },
                { Mnemonic::X86_CMPXCHG, { .Names = { "cmpxchg" } } },
                { Mnemonic::X86_XADD, { .Names = { "xadd" } } },
                { Mnemonic::X86_INVLPG, { .Names = { "invlpg" } } },
                { Mnemonic::X86_WBINVD, { .Names = { "wbinvd" } } },
                { Mnemonic::X86_INVD, { .Names = { "invd" } } },

                { Mnemonic::X86_RDMSR, { .Names = { "rdmsr" } } },
                { Mnemonic::X86_WRMSR, { .Names = { "wrmsr" } } },
                { Mnemonic::X86_RSM, { .Names = { "rsm" } } },
                { Mnemonic::X86_CPUID, { .Names = { "cpuid" } } },
                { Mnemonic::X86_CMPXCHG8B, { .Names = { "cmpxchg8b" } } },
                { Mnemonic::X86_RDTSC, { .Names = { "rdtsc" } } },
                { Mnemonic::X86_RDPMC, { .Names = { "rdpmc" } } },

                { Mnemonic::X86_CMOVO, { .Names = { "cmovo" } } },
                { Mnemonic::X86_CMOVNO, { .Names = { "cmovno" } } },
                { Mnemonic::X86_CMOVC, { .Names = { "cmovc", "cmovb", "cmovnae" } } },
                { Mnemonic::X86_CMOVNC, { .Names = { "cmovnc", "cmovnb", "cmovae" } } },
                { Mnemonic::X86_CMOVZ, { .Names = { "cmovz", "cmove" } } },
                { Mnemonic::X86_CMOVNZ, { .Names = { "cmovnz", "cmovne" } } },
                { Mnemonic::X86_CMOVNA, { .Names = { "cmovna", "cmovbe" } } },
                { Mnemonic::X86_CMOVA, { .Names = { "cmova", "cmovnbe" } } },
                { Mnemonic::X86_CMOVS, { .Names = { "cmovs" } } },
                { Mnemonic::X86_CMOVNS, { .Names = { "cmovns" } } },
                { Mnemonic::X86_CMOVP, { .Names = { "cmovp", "cmovpe" } } },
                { Mnemonic::X86_CMOVNP, { .Names = { "cmovnp", "cmovpo" } } },
                { Mnemonic::X86_CMOVL, { .Names = { "cmovl", "cmovnge" } } },
                { Mnemonic::X86_CMOVNL, { .Names = { "cmovnl", "cmovge" } } },
                { Mnemonic::X86_CMOVLE, { .Names = { "cmovle", "cmovng" } } },
                { Mnemonic::X86_CMOVNLE, { .Names = { "cmovnle", "cmovg" } } },

                { Mnemonic::X86_NOPL, { .Names = { "nopl" } } },
                { Mnemonic::X86_UD2, { .Names = { "ud2", "ud2a" } } },
                { Mnemonic::X86_UD1, { .Names = { "ud1", "ud2b" } } },
                { Mnemonic::X86_UD0, { .Names = { "ud0", "oio" } } },
                { Mnemonic::X86_SYSCALL, { .Names = { "syscall" } } },
                { Mnemonic::X86_SYSRET, { .Names = { "sysret" } } },
                { Mnemonic::X86_SYSENTER, { .Names = { "sysenter" } } },
                { Mnemonic::X86_SYSEXIT, { .Names = { "sysexit" } } },
            });
    }

    if (options.BitWidth >= 64)
    {
        merge(
            registers,
            {
                // R8
                { 0b1000, { .Code = 0b1000, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                // R9
                { 0b1001, { .Code = 0b1001, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                // R10
                { 0b1010, { .Code = 0b1010, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                // R11
                { 0b1011, { .Code = 0b1011, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                // R12
                { 0b1100, { .Code = 0b1100, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                // R13
                { 0b1101, { .Code = 0b1101, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                // R14
                { 0b1110, { .Code = 0b1110, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
                // R15
                { 0b1111, { .Code = 0b1111, .Class = RegisterClass::GeneralPurpose, .Allocatable = true } },
            });

        merge(
            register_views,
            {
                { Register::X86_RIP, { .Code = 0b101000, .Names = { "rip" }, .BitWidth = 64 } },
                { Register::X86_RFLAGS, { .Code = 0b101001, .Names = { "rflags" }, .BitWidth = 64 } },

                { Register::X86_RAX, { .Code = 0b0000, .Names = { "rax" }, .BitWidth = 64 } },
                { Register::X86_RCX, { .Code = 0b0001, .Names = { "rcx" }, .BitWidth = 64 } },
                { Register::X86_RDX, { .Code = 0b0010, .Names = { "rdx" }, .BitWidth = 64 } },
                { Register::X86_RBX, { .Code = 0b0011, .Names = { "rbx" }, .BitWidth = 64 } },
                { Register::X86_RSP, { .Code = 0b0100, .Names = { "rsp" }, .BitWidth = 64 } },
                { Register::X86_RBP, { .Code = 0b0101, .Names = { "rbp" }, .BitWidth = 64 } },
                { Register::X86_RSI, { .Code = 0b0110, .Names = { "rsi" }, .BitWidth = 64 } },
                { Register::X86_RDI, { .Code = 0b0111, .Names = { "rdi" }, .BitWidth = 64 } },

                { Register::X86_R8B, { .Code = 0b1000, .Names = { "r8b" }, .BitWidth = 8 } },
                { Register::X86_R8W, { .Code = 0b1000, .Names = { "r8w" }, .BitWidth = 16 } },
                { Register::X86_R8D, { .Code = 0b1000, .Names = { "r8d" }, .BitWidth = 32 } },
                { Register::X86_R8, { .Code = 0b1000, .Names = { "r8" }, .BitWidth = 64 } },

                { Register::X86_R9B, { .Code = 0b1000, .Names = { "r9b" }, .BitWidth = 8 } },
                { Register::X86_R9W, { .Code = 0b1000, .Names = { "r9w" }, .BitWidth = 16 } },
                { Register::X86_R9D, { .Code = 0b1000, .Names = { "r9d" }, .BitWidth = 32 } },
                { Register::X86_R9, { .Code = 0b1000, .Names = { "r9" }, .BitWidth = 64 } },

                { Register::X86_R10B, { .Code = 0b1000, .Names = { "r10b" }, .BitWidth = 8 } },
                { Register::X86_R10W, { .Code = 0b1000, .Names = { "r10w" }, .BitWidth = 16 } },
                { Register::X86_R10D, { .Code = 0b1000, .Names = { "r10d" }, .BitWidth = 32 } },
                { Register::X86_R10, { .Code = 0b1000, .Names = { "r10" }, .BitWidth = 64 } },

                { Register::X86_R11B, { .Code = 0b1000, .Names = { "r11b" }, .BitWidth = 8 } },
                { Register::X86_R11W, { .Code = 0b1000, .Names = { "r11w" }, .BitWidth = 16 } },
                { Register::X86_R11D, { .Code = 0b1000, .Names = { "r11d" }, .BitWidth = 32 } },
                { Register::X86_R11, { .Code = 0b1000, .Names = { "r11" }, .BitWidth = 64 } },

                { Register::X86_R12B, { .Code = 0b1000, .Names = { "r12b" }, .BitWidth = 8 } },
                { Register::X86_R12W, { .Code = 0b1000, .Names = { "r12w" }, .BitWidth = 16 } },
                { Register::X86_R12D, { .Code = 0b1000, .Names = { "r12d" }, .BitWidth = 32 } },
                { Register::X86_R12, { .Code = 0b1000, .Names = { "r12" }, .BitWidth = 64 } },

                { Register::X86_R13B, { .Code = 0b1000, .Names = { "r13b" }, .BitWidth = 8 } },
                { Register::X86_R13W, { .Code = 0b1000, .Names = { "r13w" }, .BitWidth = 16 } },
                { Register::X86_R13D, { .Code = 0b1000, .Names = { "r13d" }, .BitWidth = 32 } },
                { Register::X86_R13, { .Code = 0b1000, .Names = { "r13" }, .BitWidth = 64 } },

                { Register::X86_R14B, { .Code = 0b1000, .Names = { "r14b" }, .BitWidth = 8 } },
                { Register::X86_R14W, { .Code = 0b1000, .Names = { "r14w" }, .BitWidth = 16 } },
                { Register::X86_R14D, { .Code = 0b1000, .Names = { "r14d" }, .BitWidth = 32 } },
                { Register::X86_R14, { .Code = 0b1000, .Names = { "r14" }, .BitWidth = 64 } },

                { Register::X86_R15B, { .Code = 0b1000, .Names = { "r15b" }, .BitWidth = 8 } },
                { Register::X86_R15W, { .Code = 0b1000, .Names = { "r15w" }, .BitWidth = 16 } },
                { Register::X86_R15D, { .Code = 0b1000, .Names = { "r15d" }, .BitWidth = 32 } },
                { Register::X86_R15, { .Code = 0b1000, .Names = { "r15" }, .BitWidth = 64 } },
            });

        merge(
            mnemonics,
            {
            });
    }

    return InstructionSetArchitecture
    {
        .Architecture = TargetArchitecture::X86,
        .Endianness = TargetEndianness::LittleEndian,
        .BitWidth = options.BitWidth,
        .Registers = std::move(registers),
        .RegisterViews = std::move(register_views),
        .Forms = std::move(forms),
        .Mnemonics = std::move(mnemonics),
    };
}
