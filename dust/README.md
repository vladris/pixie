Dust is an assembler for Pixie. 

# Assembler Syntax

A line can be either blank, a comment, a label, an instruction, or data. 

A comment lines start with `#`:

```
# This is a comment
```

A label consists of any number of letters and must end with `:`:

```
LABEL:
```

An instruction consists of an op code and one or two operands. An op code is any of the op codes supported by Pixie (lower case): `mov`, `add`, `sub`, `mul`, `div`, `rem`, `not`, `and`, `or`, `xor`, `eq`, `le`, `leq`, `jnz`, `in`, `out`. 
An operand can be either a register or a number. Registers (lower case) are `r0`, `r1`, `r2`, `r3`, `sb`, `sp`, `pc`. Numbers can be written in decimal, hexadecimal, or binary notation, eg: 42, 0x2A, 0b101010. An operand can be optionally prefixed by `*` to mark it as a dereference:

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
<instruction>     ::= <opcode> <operand> [<operand>]
<data>            ::= <number>
<upcase-letters>  ::= <upcase-letter> [<upcase-letters>]
<upcase-letter>   ::= "A" | "B" | "C" | "D" | "E" | "F" | "G" | "H" | "I" | 
                      "J" | "K" | "L" | "M" | "N" | "O" | "P" | "Q" | "R" | 
                      "S" | "T" | "U" | "V" | "W" | "X" | "Y" | "Z"
<opcode>          ::= "mov" | "add" | "sub" | "div" | "rem" | "not" | "and" |
                      "or" | "xor" | "eq" | "le" | "leq" | "jnz" | "in" | "out"
<operand>         ::= ["*"] <operand-value>
<operand-value>   ::= <register> | <number> | <label-reference>
<register>        ::= "r0" | "r1" | "r2" | "r3" | "r4" | "sb" | "sp" | "pc"
<number>          ::= <bin-number> | <dec-number> | <hex-number>
<bin-number>      ::= "0b" { "0" | "1" }
<dec-number>      ::= <dec-digit> {<dec-digit>}
<dec-digit>       ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"   
<hex-number>      ::= "0x" <hex-digit> {<hex-digit>}
<hex-digit>       ::= <dec-digit> | "A" | "B" | "C" | "D" | "E" | "F"
<label-reference> ::= ":" <upcase-letters>
```

## Example

Following is an example program which counts down from 10, outputs numbers, then terminates:

```
# Countdown from 10
    mov r0 10
REPEAT:
    out r0
    sub r0 1
    jnz r0 :REPEAT
# Pixie terminates when PC is at 0xFFFF
    jnz 1 0xFFFF
    
```

# Implementation

The assembler is implemented in Python3. It runs several passes over the source text transforming it into the final program which is output as a sequence of space-separated numbers consumable by the pixie executable.
