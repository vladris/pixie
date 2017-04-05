Pixie is a 16-bit word-addressable register VM with [7 registers](https://github.com/vladris/pixie/wiki/Pixie-Architecture#registers), [16 op codes](https://github.com/vladris/pixie/wiki/Pixie-Architecture#instructions), and [128 KB of memory](https://github.com/vladris/pixie/wiki/Pixie-Architecture#memory), with [port-mapped I/O](https://github.com/vladris/pixie/wiki/Pixie-Architecture#io).

[Dust](https://github.com/vladris/pixie/wiki/Dust-Assembler) is an assembler for the VM.

A program which counts down from 10, outputs numbers, then terminates:

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

For a more complex example, check out the [Brainfuck](https://en.wikipedia.org/wiki/Brainfuck) interpreter [here](https://github.com/vladris/pixie/blob/master/examples/brainfuck.dust).

# Implementation Notes

Pixie VM is fully implemented in the header `pixie/pixie.h`. `pixie/pixie.cpp` provides a simple way to load a program from a file. Expected format of a program is a text file consisting of any number of 16 bit unsigned integers separated by spaces. `pixie/pixie.cpp` instantiates a VM and connects stdin to input port 0 and stdout to output port 0.

The project comes with a VS solution file. Pixie uses C++17 `std::variant` so it requires a compiler/STL which supports this type.

The assembler is implemented in Python3. It runs several passes over the source text transforming it into the final program which is output as a sequence of space-separated numbers consumable by the pixie executable.

This project serves no practical purposes (see [Turing tarpit](https://en.wikipedia.org/wiki/Turing_tarpit)) and was not thoroughly tested.
