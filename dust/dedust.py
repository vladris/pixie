# disassembler for pixie programs
# note there is no way to disambiguate between code and data in a program, so
# dedust attempts to decode everything as instructions
import sys

op_codes = ['mov', 'add', 'sub', 'mul', 'div', 'rem', 'not', 'and', 'or',
    'xor', 'eq', 'le', 'leq', 'jnz', 'in', 'out']
registers = ['r0', 'r1', 'r2', 'r3', 'sb', 'sp', 'pc', '__value', '*r0',
    '*r1', '*r2', '*r3', '*sb', '*sp', '*pc', '*__value']

def decode_instructions(instr, offset=0):
    try:
        decoded = [op_codes[(instr[0] >> 8) & 0xFF],
                   registers[(instr[0] >> 4) & 0xF],
                   registers[instr[0] & 0xF]]
    except:
        # if we can't decode the instruction, it's likely raw data
        decoded = ['raw', 'data: ', '{0:#06x}'.format(instr[0])]
    instr, instr_start, raw = instr[1:], offset, [instr[0]]

    for operand in [1, 2]:
        if decoded[operand].strip('*') == '__value':
            if instr:
                decoded[operand] = '{0:#06x}'.format(instr[0])
                raw += [instr[0]]
                instr = instr[1:]
                offset += 1
            else:
                decoded[operand] = 'value?'

    raw = '{0: <17}'.format(' '.join(['{0:#05}'.format(word) for word in raw]))

    # prints offset: decoded instruction | raw words
    fmt = '{0:#06x}: {1: >3} {2: >6} {3: >6} | ' + raw
    print(fmt.format(instr_start, *decoded))
    if instr:
        decode_instructions(instr, offset + 1)

if len(sys.argv) < 2:
    print("Usage: dedust <program>")
    print("       dedust -i <instruction> [<instruction>]")
    exit()

if sys.argv[1] == '-i':
    content = sys.argv[2:]
else:
    with open(sys.argv[1]) as f:
        content = f.read().split()

decode_instructions([int(instr) for instr in content])
