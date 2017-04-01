# assembles and executes example files and compares expected and actual output
import subprocess

import sys

test_cases = {
    '42': [('', b'42')],
    'add': [(b'1\n1\n', b'2'),
            (b'5\n6\n', b'11')],
    'countdown': [(b'', b'10987654321')],
    'stack': [(b'1\n2\n3\n4\n5\n6\n0\n', b'0654321')],
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
