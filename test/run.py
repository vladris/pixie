# assembles and executes example files and compares expected and actual output
import subprocess

import sys

# prep input to make it consumable by the pixie executable
def prep(s):
    return ('\n'.join(s) + '\n').encode('ascii')

test_cases = {
    '42':
        [('', b'42')],

    'add':
        [(prep('11'), b'2'),
         (prep('56'), b'11')],

    'countdown':
        [(b'', b'10987654321')],

    'stack':
        [(prep('1234560'), b'0654321')],

    'brainfuck':
         # 2 + 5 in brainfuck
        [(prep([str(ord(c)) for c in '++>+++++[<+>-]<.'] + ['0']), b'7'),
         # Hello world in brainfuck
         (prep([str(ord(c)) for c in '++++++++[>++++[>++>+++>+++>+<<<<-'
                                     ']>+>+>->>+[<]<-]>>.>---.+++++++..+'
                                     '++.>>.<-.<.+++.------.--------.>>+'
                                     '.>++.'] + ['0']),
                ''.join([str(ord(c)) for c in 'Hello World!\n']).encode('ascii'))]
}

failures = []

for program in test_cases:
    # compile program
    subprocess.call(['python', 
        '..\\dust\\dust.py',
        f'..\\examples\\{program}.dust',
        f'{program}.pixie'])

    for program_in, program_out in test_cases[program]:
        p = subprocess.Popen(['..\\pixie\\vs\\Debug\\pixie.exe',
              f'{program}.pixie'], stdin=subprocess.PIPE, stdout=subprocess.PIPE)

        actual, _ = p.communicate(program_in)

        if actual == program_out:
            print('.', end='')
        else:
            failures.append({'program': program,
                             'input': program_in,
                             'expected output': program_out,
                             'actual output': actual})

print()
print('PASS' if not failures else f'{len(failures)} failures:')

for failure in failures:
    print(failure)
