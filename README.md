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

This project serves no practical purposes (see [Turing tarpit](https://en.wikipedia.org/wiki/Turing_tarpit)) and was not thoroughly tested. For more details, check out the [wiki](https://github.com/vladris/pixie/wiki/).
