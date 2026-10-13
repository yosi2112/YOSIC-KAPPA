"""Exercise the public register/JMP ABI on the guest CPU and real FDC."""
import struct
import subprocess

from support import DOS, OUT, TOOLS, add_file, base, encode, run, status


def assemble(name, body, entry=0):
    source = OUT / f'{name}.asm'
    binary = OUT / f'{name}.bin'
    body = '\n'.join('        ' + line for line in body.splitlines())
    source.write_text(f'CPU YOSIC-KAPPA\nINCLUDE "{DOS / "api.inc"}"\n' + body + '\nEND\n')
    subprocess.run([str(TOOLS / 'yosic-as.exe'), str(source), '-o', str(binary)], check=True)
    raw = base()
    add_file(raw, 0, 'SC.YKE', encode(binary.read_bytes(), entry))
    return raw


def check(register, value):
    prefix = 'ADD R5,R0,RZR\n' if register == 'R0' else ''
    register = 'R5' if register == 'R0' else register
    return prefix + f'DosConst R0,{value}\nSUB R0,{register},R0\nSRZ R0\nJMP bad\n'


def call(function, argument, expected=0, result=None):
    source = f'DosConst R2,{argument}\nDosCall {function}\n' + check('R0', expected)
    if result is not None:
        source += check('R2', result)
    return source


failure = 'bad:\nADI R1,RZR,DOS_EXIT\nADI R2,RZR,99\nJMP 0fff0h\n'
body = 'DosConst SP,0e8000h\n'
for register, value in [('R3', 33), ('R4', 44), ('R6', 66), ('R8', 88), ('R9', 99),
                        ('R10', 100), ('R11', 111), ('R12', 122), ('R13', 133)]:
    body += f'ADI {register},RZR,{value}\n'
body += call('DOS_PUTS', '80000h+message')
body += check('R1', 2) + check('SP', '0e8000h')
for register, value in [('R3', 33), ('R4', 44), ('R6', 66), ('R8', 88), ('R9', 99),
                        ('R10', 100), ('R11', 111), ('R12', 122), ('R13', 133)]:
    body += check(register, value)
body += call('DOS_PUTCHAR', 64) + call('DOS_PUTCHAR', 13) + call('DOS_PUTCHAR', 10)
body += call('DOS_GETCHAR', 0, result=90)  # Consume Z after the command newline.
body += call('DOS_GETCHAR', 0, result='0ffffffffh')  # EOF, no blocking/fault.
body += call('DOS_GETDRIVE', 0, result=0)
body += call('DOS_SETDRIVE', 1)
body += call('DOS_GETDRIVE', 0, result=1)
body += call('DOS_SETDRIVE', 2, 21)  # Unmounted: keep drive 1 selected.
body += call('DOS_GETDRIVE', 0, result=1)
body += call('DOS_SETDRIVE', 4, 15)
body += call('DOS_SETDRIVE', 0)
for function, text, expected in [('DOS_MKDIR', 'mkdir', 0), ('DOS_COPY', 'copy', 0),
                                  ('DOS_MOVE', 'move', 0), ('DOS_DIR', 'directory', 0),
                                  ('DOS_RMDIR', 'mkdir', 5), ('DOS_DELETE', 'delete', 0),
                                  ('DOS_RMDIR', 'mkdir', 0), ('DOS_SYS', 'system', 0)]:
    body += call(function, f'80000h+{text}', expected)
for function in (6, 15, 23, 255, -1):
    body += call(function, 0, 1)
for pointer in ('0', '0d0001h', '8fff0h', '0f0000h', '0fe0000h', '80000000h', '0fffffffch'):
    body += call('DOS_PUTS', pointer, 87)
body += call('DOS_PUTCHAR', 256, 87) + call('DOS_PUTCHAR', '0ffffffffh', 87)
body += 'DosConst R3,0d0000h\nADI R0,RZR,256\nSRM R0,[R3]\n'
body += call('DOS_PUTS', '0d0000h', 87)
# Unterminated string at the last application word must not read the gates.
body += 'DosConst R3,8ffech\nADI R0,RZR,65\nSRM R0,[R3]\n'
body += call('DOS_PUTS', '8ffech', 87)
body += 'ADI R2,RZR,42\nDosCall DOS_EXIT\n' + failure
for label, text in [('message', 'SYSCALL OK\r\n'), ('mkdir', '0:\\TEMP'),
                    ('copy', '0:\\SOURCE.DAT 0:\\TEMP\\COPY.DAT /Y'),
                    ('move', '0:\\TEMP\\COPY.DAT 0:\\TEMP\\MOVED.DAT'),
                    ('directory', '0:\\TEMP /B'), ('delete', '0:\\TEMP\\MOVED.DAT'),
                    ('system', '1:')]:
    body += label + ':\nDD ' + ','.join(str(ord(c)) for c in text) + ',0\n'
raw = assemble('syscall-main', body)
add_file(raw, 1, 'SOURCE.DAT', bytes(range(256)) * 3)
target = base()
add_file(target, 0, 'KEEP.DAT', b'keep target contents')
target_path = OUT / 'syscall-drive1.img'
target_path.write_bytes(target)
target_saved = OUT / 'syscall-drive1-saved.img'
result, saved = run('syscall-main-run', 'SC\nZ', raw, disks={1: target_path},
                    copies={1: target_saved}, steps=500000000)
assert 'SYSCALL OK\n@\n' in result.stdout, result.stdout
assert 'MOVED.DAT\n' in result.stdout, result.stdout
assert 'System transferred\n' in result.stdout, result.stdout
status(result, 42)
root = saved[181 * 512:195 * 512]
assert not any(root[i:i + 8] == b'TEMP    ' for i in range(0, len(root), 32))
transferred = target_saved.read_bytes()
assert transferred[:162 * 512] == base()[:162 * 512]
assert transferred[162 * 512:] == target[162 * 512:]
result, _ = run('syscall-reboot', 'DIR /B\n', transferred)
assert 'KEEP.DAT\n' in result.stdout, result.stdout

# Segment changes are normalized; physical pointers and stack are preserved.
body = '''ADI R0,RZR,3
LSEG DS,R0
LSEG SS,R0
LSEG ES,R0
DosConst SP,0e8000h
''' + call('DOS_GETDRIVE', 0, result=0) + check('SP', '0e8000h')
body += 'ADI R2,RZR,23\nDosCall DOS_EXIT\n' + failure
result, _ = run('syscall-segments', 'SC\nDRIVE\n', assemble('syscall-segments-app', body))
assert result.stdout.endswith('0:>DRIVE\n0:\n0:>'), result.stdout

# Filesystem string limit, PUTS limit, NUL requirement and no partial output.
body = call('DOS_DIR', '80000h+longfs', 87)
body += call('DOS_PUTS', '80000h+longputs', 87)
body += call('DOS_PUTS', '80000h+maximumputs')
body += 'ADI R2,RZR,31\nDosCall DOS_EXIT\n' + failure
body += 'longfs:\nREPT 128\nDD 65\nENDM\nDD 0\n'
body += 'longputs:\nREPT 4096\nDD 81\nENDM\nDD 0\n'
body += 'maximumputs:\nREPT 4095\nDD 80\nENDM\nDD 0\n'
result, _ = run('syscall-text-limits', 'SC\n', assemble('syscall-text-limits-app', body))
assert 'QQQQ' not in result.stdout, result.stdout
assert 'P' * 4095 in result.stdout, result.stdout
status(result, 31)

body = 'ADD R2,R1,RZR\nDosCall DOS_PUTS\nADI R2,RZR,19\nDosCall DOS_EXIT\n'
result, _ = run('syscall-argument-text', 'SC MiXeD "arg"\n',
                assemble('syscall-argument-text-app', body))
assert '\nMiXeD "arg"0:>' in result.stdout, result.stdout
status(result, 19)

# A resume target can be the last payload word, including a nonzero entry.
body = '''ORG 4
DosConst R7,0ffech
ADI R1,RZR,DOS_GETDRIVE
JMP 0fff0h
bad:
ADI R1,RZR,DOS_EXIT
ADI R2,RZR,55
JMP 0fff0h
ORG 0ffech
JMP bad
'''
result, _ = run('syscall-last-return', 'SC\n', assemble('syscall-last-app', body, 4))
status(result, 55)

for name, resume in [('unaligned', 1), ('negative', '0fffffffch'),
                     ('gate', '0fff0h'), ('past-payload', '1000h')]:
    body = f'DosConst R7,{resume}\nADI R1,RZR,DOS_GETDRIVE\nJMP 0fff0h\nJMP $\n'
    result, _ = run(f'syscall-return-{name}', 'SC\nDRIVE\n',
                    assemble(f'syscall-return-{name}-app', body))
    status(result, 0)  # Subsequent DRIVE still works and becomes last shell status.
    assert result.stdout.endswith('0:>DRIVE\n0:\n0:>'), result.stdout
    result, _ = run(f'syscall-return-status-{name}', 'SC\n',
                    assemble(f'syscall-return-status-{name}-app', body))
    status(result, 87)

body = 'ADI R7,RZR,-1\nADI R1,RZR,DOS_EXIT\nADI R2,RZR,41\nJMP 0fff0h\n'
result, _ = run('syscall-exit-no-return', 'SC\n', assemble('syscall-exit-app', body))
status(result, 41)

# Reject files that extend into the newly reserved syscall words.
payload = bytes(65524)
image = struct.pack('<4s5I', b'YKE1', 1, len(payload), 0, 0, 0) + payload
raw = base()
add_file(raw, 0, 'SC.YKE', image)
result, _ = run('syscall-gate-overlap', 'SC\n', raw)
assert 'Invalid YKE executable\n' in result.stdout, result.stdout
status(result, 11)

example_bin = OUT / 'syscall-example.bin'
subprocess.run([str(TOOLS / 'yosic-as.exe'), str(DOS / 'examples/syscall.asm'),
                '-o', str(example_bin)], check=True)
raw = base()
add_file(raw, 0, 'SC.YKE', encode(example_bin.read_bytes()))
result, _ = run('syscall-example', 'SC\n', raw)
assert 'HELLO SYSCALL\n' in result.stdout, result.stdout
status(result, 7)
print('PASS syscall R1/JMP ABI: console/drive/command functions, registers/stack, pointers, '
      'input/EOF, filesystem/SYS/reboot, return bounds and EXIT')
