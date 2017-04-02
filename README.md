Pixie is a 16-bit word-addressable register VM with 7 registers, 16 op codes, and 128 KB of memory. Dust is an assembler for the VM.

# Architecture

## Registers

The VM has a total of 7 registers: 4 general-purpose registers, `R0`, `R1`, `R2`, `R3`; stack base and stack pointer registers `SB` and `SP`; and program counter register `PC`. Programs are loaded in memory starting at address `0x0000`. `SB` and `SP` are not treated specially besides being initialized to the address right after the program. `PC` is incremented with each tick and the machine halts once `PC` becomes `0xFFFF`.

## Memory

Memory consists of 65536 16-bit words (128 KB), starts at address `0x000` and ends at `0xFFFF`. Memory is word-addressable. Programs are loaded beginning at address `0x0000`.

## Instructions

Instructions are encoded as 1, 2, or 3 words with the following schema: most significant 8 bits of the word represent the op code (`0xFF00`), next 4 bits encode the first operand (`0x00F0`), least significant 4 bits encode the second operand (`0x000F`).

An operand is encoded in a byte as follows: it can be a register or a value. Registers are encoded from `0b0000` to `0b0110`, values are represented by `0b0111`. In case operands are values, the actual values should be supplied as the next words in the program following the instruction. For example `mov r0 r1` is encoded in one word, while `mov r0 0xBEEF` is encoded in two words. The most significant bit of an operand (`0b1000`) encodes whether the operand is a dereference, in which case it is resolved to the address pointed to by the operand instead of the operand itself. For example `add sp 1` increments the stack pointer while `add *sp 1` increments the value at the top of the stack.

Below is the list of op codes. All operations store their result into the first operand.

`mov` - copies the value of the second operand into the first operand

`add` - adds the second operand to the first operand

`sub` - subtracts the second operand from the first operand

`mul` - multiplies the first operand by the second operand

`div` - divides the first operand by the second operand

`rem` - computes remainder from division of first operand by second operand

`not` - replaces the first operand by the logical negation of the second operand

`and` - bitwise AND between the operands

`or` - bitwise OR between the operands

`xor` - bitwise XOR between the operands

`eq` - replaces first operand by 1 if operands are equal, with 0 otherwise

`le` - replaces first operand by 1 if it is less than second operand, with 0 otherwise

`leq` - replaces first operand by 1 if it is less than or equal to the second operand

`jnz` - moves the instruction pointer to the address supplied by the second operand if the first operand is not 0

`in` - reads a word from the input port specified by the second operand and stores the value into the first operand

`out` - outputs the second operand to the output port specified by the first operand

## I/O

I/O is blocking, performed by the `in` and `out` instructions as detailed above.

# Assembler

## Syntax

A line can be either blank, a comment, a label, an instruction, or data. 

A comment lines start with `#`:

```
# This is a comment
```

A label consists of any number of upper case letters and must end with `:`:

```
LABEL:
```

An instruction consists of an op code and two operands. An op code is any of the op codes supported by Pixie (lower case): `mov`, `add`, `sub`, `mul`, `div`, `rem`, `not`, `and`, `or`, `xor`, `eq`, `le`, `leq`, `jnz`, `in`, `out`. 
An operand can be either a register or a number. Registers (lower case) are `r0`, `r1`, `r2`, `r3`, `sb`, `sp`, `pc`. Numbers can be written in decimal, hexadecimal, or binary notation, eg: `42`, `0x2A`, `0b101010`. An operand can be optionally prefixed by `*` to mark it as a dereference:

```
mov *r0 0xBEEF
jnz r1 :LABEL
```

Data can be any sequence of numbers. These are simply inlined in the resulting program. Data can be used to store constant values:

```
1 2 3 4 5
```

## BNF Grammar
```
<program>         ::= {<line>}
<line>            ::= <comment> | <label> | <instruction> | <data>
<comment>         ::= "#" {any characters}
<label>           ::= <upcase-letters> ":"
<instruction>     ::= <opcode> <operand> <operand>
<data>            ::= <number>
<upcase-letters>  ::= <upcase-letter> [<upcase-letters>]
<operand>         ::= ["*"] <operand-value>
<operand-value>   ::= <register> | <number> | <label-reference>
<label-reference> ::= ":" <upcase-letters>
<number>          ::= <bin-number> | <dec-number> | <hex-number>
<bin-number>      ::= "0b" { "0" | "1" }
<dec-number>      ::= <dec-digit> {<dec-digit>}
<hex-number>      ::= "0x" <hex-digit> {<hex-digit>}
<register>        ::= "r0" | "r1" | "r2" | "r3" | "r4" | "sb" | "sp" | "pc"
<upcase-letter>   ::= "A" | "B" | "C" | "D" | "E" | "F" | "G" | "H" | "I" | 
                      "J" | "K" | "L" | "M" | "N" | "O" | "P" | "Q" | "R" | 
                      "S" | "T" | "U" | "V" | "W" | "X" | "Y" | "Z"
<opcode>          ::= "mov" | "add" | "sub" | "div" | "rem" | "not" | "and" |
                      "or" | "xor" | "eq" | "le" | "leq" | "jnz" | "in" | "out"
<dec-digit>       ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"   
<hex-digit>       ::= <dec-digit> | "A" | "B" | "C" | "D" | "E" | "F"
```

## Example

Following is an example program which counts down from 10, outputs numbers, then terminates:

```
# Countdown from 10
    mov r0 10
REPEAT:
    out r0 0
    sub r0 1
    jnz r0 :REPEAT
# Pixie terminates when PC is at 0xFFFF
    jnz 1 0xFFFF
    
```

# Implementation Notes

Pixie VM is fully implemented in the header `pixie.h`. `pixie.cpp` provides a simple way to load a program from a file. Expected format of a program is a text file consisting of any number of 16 bit unsigned integers separated by spaces. `pixie.cpp` instantiates a VM and connects stdin to input port 0 and stdout to output port 0.

The project comes with a VS solution file. Pixie uses C++17 `std::variant` so it requires a compiler/STL which supports this type.

The assembler is implemented in Python3. It runs several passes over the source text transforming it into the final program which is output as a sequence of space-separated numbers consumable by the pixie executable.

This project serves no practical purposes (see [Turing tarpit](https://en.wikipedia.org/wiki/Turing_tarpit)) and was not thoroughly tested.
