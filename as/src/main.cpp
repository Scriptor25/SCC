#include <scc/as/module.hpp>
#include <scc/as/parser.hpp>

#include <scc/platform.hpp>

#include <fstream>
#include <iostream>

int main(const int argc, const char **argv)
{
    if (argc != 2)
        return 1;

    std::ifstream in(argv[1]);
    if (!in)
        return 1;

    const scc::Platform platform
    {
        .IR = {
            .LegalIntWidth = { 1, 8, 16, 32, 64 },
            .LegalFloatWidth = { 32, 64 },
        },
        .ISA = {
            .Architecture = scc::platform::TargetArchitecture::X86,
            .Endianness = scc::platform::TargetEndianness::LittleEndian,
            .Mnemonics = {
                { "mov", 0 },
                { "movb", 0 },
                { "movw", 0 },
                { "movl", 0 },
                { "movq", 0 },
                { "add", 0 },
                { "sub", 0 },
                { "imul", 0 },
                { "div", 0 },
                { "inc", 0 },
                { "dec", 0 },
                { "xor", 0 },
                { "call", 0 },
                { "ret", 0 },
                { "push", 0 },
                { "pop", 0 },
                { "jmp", 0 },
                { "cmp", 0 },
                { "jeq", 0 },
                { "jne", 0 },
                { "jlt", 0 },
                { "jle", 0 },
                { "jgt", 0 },
                { "jge", 0 },
                { "syscall", 0 },
            },
            .Registers = {
                { "al", 0b0000000 },
                { "ah", 0b1000000 },
                { "ax", 0b0010000 },
                { "eax", 0b0100000 },
                { "rax", 0b0110000 },

                { "bl", 0b0000011 },
                { "bh", 0b1000011 },
                { "bx", 0b0010011 },
                { "ebx", 0b0100011 },
                { "rbx", 0b0110011 },

                { "cl", 0b0000001 },
                { "ch", 0b1000001 },
                { "cx", 0b0010001 },
                { "ecx", 0b0100001 },
                { "rcx", 0b0110001 },

                { "dl", 0b0000010 },
                { "dh", 0b1000010 },
                { "dx", 0b0010010 },
                { "edx", 0b0100010 },
                { "rdx", 0b0110010 },

                { "sil", 0b0000110 },
                { "si", 0b0010110 },
                { "esi", 0b0100110 },
                { "rsi", 0b0110110 },

                { "dil", 0b0000111 },
                { "di", 0b0010111 },
                { "edi", 0b0100111 },
                { "rdi", 0b0110111 },

                { "bpl", 0b0000101 },
                { "bp", 0b0010101 },
                { "ebp", 0b0100101 },
                { "rbp", 0b0110101 },

                { "spl", 0b0000100 },
                { "sp", 0b0010100 },
                { "esp", 0b0100100 },
                { "rsp", 0b0110100 },

                { "r8b", 0b0001000 },
                { "r8w", 0b0011000 },
                { "r8d", 0b0101000 },
                { "r8", 0b0111000 },

                { "r9b", 0b0001001 },
                { "r9w", 0b0011001 },
                { "r9d", 0b0101001 },
                { "r9", 0b0111001 },

                { "r10b", 0b0001010 },
                { "r10w", 0b0011010 },
                { "r10d", 0b0101010 },
                { "r10", 0b0111010 },

                { "r11b", 0b0001011 },
                { "r11w", 0b0011011 },
                { "r11d", 0b0101011 },
                { "r11", 0b0111011 },

                { "r12b", 0b0001100 },
                { "r12w", 0b0011100 },
                { "r12d", 0b0101100 },
                { "r12", 0b0111100 },

                { "r13b", 0b0001101 },
                { "r13w", 0b0011101 },
                { "r13d", 0b0101101 },
                { "r13", 0b0111101 },

                { "r14b", 0b0001110 },
                { "r14w", 0b0011110 },
                { "r14d", 0b0101110 },
                { "r14", 0b0111110 },

                { "r15b", 0b0001111 },
                { "r15w", 0b0011111 },
                { "r15d", 0b0101111 },
                { "r15", 0b0111111 },
            },
        },
        .ABI = {
            .DataLayout = {
                .PointerSize = 8,
                .PointerAlign = 8,
                .MaxIntAlign = 8,
                .MaxFloatAlign = 8,
                .MaxVectorAlign = 16,
                .MaxAggregateAlign = 16,
            },
            .CallLayout = {
                .StackAlign = 16,
            }
        },
        .Features = {},
    };

    scc::as::Module module;
    scc::as::Parser parser(in, platform, module);

    parser.Parse();

    module.Print(std::cout);

    return 0;
}
