# SCC - The Scriptor Compiler Collection

WIP: collection of platform independent libraries for compiling, assembling, interpreting and linking binary files,
similar to toolkits like llvm or gcc.

## Structure

- `cc`:
    - parse `c/c++`
    - write `c/c++` and `ir`
- `ir`:
    - parse `ir`
    - write `ir` and `asm`
- `as`:
    - parse `asm`
    - write `asm` and `object`
- `ld`:
    - parse `object`
    - write `object` and `binary`
