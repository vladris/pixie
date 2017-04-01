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

# token is number if a label or (ignoring dereferences) starts with a digit
def is_number(token):
    if token[0] == ':':
        # labels resolve to numbers
        return True
    if token[0] == '*':
        # ignore dereference
        token = token[1:]
    return token[0].isdigit()

# convert numbers to string in bases 10, 2, and 16
def atoi(token):
    if 'x' in token:
        return int(token, 16)
    elif 'b' in token:
        return int(token, 2)
    else:
        return int(token)

# compute offset of each label
labels, i, offset = { }, 0, -1
for line in content:
    if line[0][-1] == ':':
        # if label, associate it with offset of next instruction
        labels[':' + line[0][:-1]] = str(offset + 1)
    else:
        # else increase offset by number of non-register non-opcode tokens 
        offset += len([1 for token in line if is_number(token)])
        # if this starts with an opcode, add 1 for the instruction itself
        if not is_number(line[0]):
            offset += 1

# strip labels
content = [line for line in content if line[0][-1] != ':']

# replace labels in instructions by their actual offset
content = [
    [labels[token] if token in labels else token for token in line]
    for line in content]

op_codes = ['mov', 'add', 'sub', 'mul', 'div', 'rem', 'not', 'and', 'or',
    'xor', 'eq', 'le', 'leq', 'jnz', 'in', 'out']
registers = ['r0', 'r1', 'r2', 'r3', 'sb', 'sp', 'pc']

# add operand to encoded word
def encode(encoded, operand, shift=0):
    if operand[0] == '*':
        # mark dereference (0b1000)
        encoded[0] |= 8 << shift
        operand = operand[1:]

    if operand not in registers:
        # if this is a number, operand is 0b111 and add value as next word
        encoded[0] |= 7 << shift
        return encoded + [atoi(operand)]
    else:
        # else encode register
        encoded[0] |= registers.index(operand) << shift
        return encoded

# encode program
program = []
for line in content:
    if is_number(line[0]):
        # if line starts with a number it should only contain numbers 
        program += [atoi(token) for token in line]
    else:
        # encode opcode and operands
        code = [op_codes.index(line[0]) << 8]
        code = encode(code, line[1], 4)
        code = encode(code, line[2])
        program += code

with open(sys.argv[2], "w") as f:
    f.write(' '.join([str(instr) for instr in program]))
