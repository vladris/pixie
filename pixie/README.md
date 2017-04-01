Pixie is a 16-bit word-addressable register VM. It has 7 registers, 16 op codes, and 128 KB of memory.

# Architecture

## Registers

The VM has a total of 7 registers: 4 general-purpose registers, `R0`, `R1`, `R2`, `R3`; stack base and stack pointer registers `SB` and `SP`; and program counter register `PC`. Programs are loaded in memory starting at address 0x0000. `SB` and `SP` are not treated specially besides being initialized to the address right after the program. `PC` is incremented with each tick and the machine halts once `PC` becomes 0xFFFF.

## Memory

Memory consists of 65536 16-bit words (128 KB), starts at address 0x000 and ends at 0xFFFF. Memory is word-addressable. Programs are loaded beginning at address 0x0000.

## Instructions

Instructions are encoded as 1, 2, or 3 words with the following schema: most significant 8 bits of the word represent the op code (0xFF00), next 4 bits encode the first operand (0x00F0), least significant 4 bits encode the second operand (0x000F).

An operand is encoded in a byte as follows: it can be a register or a memory address. Registers are encoded from 0b0000 to 0b0110, addresses are represented by 0b0111. In case operands are addresses, the actual address value should be supplied as the next words in the program following the instruction. For example `mov r0 r1` is encoded in one word, while `mov r0 0xBEEF` is encoded in two words. The most significant bit of an operand (0b1000) encodes whether the operand is a dereference, in which case it is resolved to the address pointed to by the operand instead of the operand itself. For example `add sp 1` increments the stack pointer while `add *sp 1` increments the value at the top of the stack.

Below is the list of op codes. All operations store their result into the first operand.

`mov` - copies the value of the second operand into the first operand
`add` - adds the second operant to the first operand
`sub` - subtracts the second operand from the first operand
`mul` - multiplies the first operand by the second operand
`div` - divides the first operand by the second operand
`rem` - computes remainder from division of first operand by second operand
`not` - replaces the first operand by its logical negation (second operand is ignored)
`and` - bitwise AND between the operands
`or` - bitwise OR between the operands
`xor` - bitwise XOR between the operands
`eq` - replaces first operand by 1 if operands are equal, with 0 otherwise
`le` - replaces first operand by 1 if it is less than second operand, with 0 otherwise
`leq` - replaces first operand by 1 if it is less than or equal to the second operand
`jnz` - moves the instruction pointer to the address supplied by the second operand if the first operand is not 0
`in` - reads a word from the input and stores it in the first operand (second operand is ignored)
`out` - outputs the first operand (second operand is ignored)

## I/O

I/O is blocking, performed by the `in` and `out` instructions as detailed above.

# Implementation

Pixie VM is fully implemented in the header `pixie.h`. `pixie.cpp` provides a simple way to load a program from a file. Expected format of a program is a text file consisting of any number of 16 bit unsigned integers separated by spaces.

The project comes with a VS solution file. Pixie uses C++17 std::variant so it requires a compiler/STL which supports this type.
