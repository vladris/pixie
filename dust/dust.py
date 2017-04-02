import sys

if len(sys.argv) != 3:
    print("Usage: dust <input> <output>")
    exit()

with open(sys.argv[1]) as f:
    content = f.readlines()

# strip whitespace
content = [' '.join(line.split()) for line in content]

# strip blank lines and comments, then split tokens
content = [line.split() for line in content if line and line[0] != '#']

op_codes = ['mov', 'add', 'sub', 'mul', 'div', 'rem', 'not', 'and', 'or',
    'xor', 'eq', 'le', 'leq', 'jnz', 'in', 'out']
registers = ['r0', 'r1', 'r2', 'r3', 'sb', 'sp', 'pc', '__value', '*r0',
    '*r1', '*r2', '*r3', '*sb', '*sp', '*pc', '*__value']

# encode operand
def op(token):
    if token not in registers:
        # if not a register, replace with '__value', keeping dereference prefix
        token = ('*' if token[0] == 0 else '') + '__value'
    # encoded operand is index in register list
    return registers.index(token)

# encode value
def value(token):
    # separate word value exists only if token is not a register
    return [token.strip('*')] if token not in registers else []

# parse line to its encoded representation
def parse(line):
    if line[0].isdigit():
        # for numbers, simply return them
        return line
    elif line[0] in op_codes:
        # for instruction, expand it to op code, encoded operands, and values
        expanded = [op_codes.index(line[0]), op(line[1]),
            op(line[2])] + value(line[1]) + value(line[2])
        # then encode op code and operands into a single word
        return [int(expanded[0]) << 8 | int(expanded[1]) << 4 |
            int(expanded[2])] + expanded[3:]
    else:
        # otherwise we have a label, leave it be for now 
        return [line[0]]

# parse lines, determine index of each label
program, labels, offset = [], {}, 0
for token in sum([parse(line) for line in content], []):
    if isinstance(token, str) and token[-1] == ':':
        labels[':' + token[:-1]] = offset
    else:
        offset += 1
        program.append(token)

# replace labels with their offset
program = [labels[token] if token in labels else token for token in program]

with open(sys.argv[2], "w") as f:
    f.write(' '.join([str(int(str(instr), 0)) for instr in program]))
