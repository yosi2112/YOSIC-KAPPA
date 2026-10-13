"""CHDIR on the guest CPU: per-drive paths, relative I/O, safety and ABI."""
import struct
import subprocess

from support import DOS, OUT, TOOLS, add_file, base, encode, entry, fatset, run, status


def row_at(raw, directory, name):
    start = (195 + directory - 2) * 512 if directory else 181 * 512
    end = start + 512 if directory else 195 * 512
    stem, _, ext = name.partition('.')
    key = (stem.ljust(8) + ext.ljust(3)).encode()
    return next(raw[i:i + 32] for i in range(start, end, 32) if raw[i:i + 11] == key)


def cluster_of(row):
    return struct.unpack_from('<H', row, 26)[0]


raw = base()
add_file(raw, 0, 'SOURCE.DAT', b'relative file contents')
other = OUT / 'chdir-drive1.img'
other.write_bytes(base())
other_saved = OUT / 'chdir-drive1-saved.img'
commands = r'''CHDIR /?
CHDIR
MKDIR TOOLS
MKDIR TOOLS\INNER
CHDIR tools
CHDIR
DIR /B
MKDIR REL
COPY ..\SOURCE.DAT LOCAL.DAT /Y
MOVE LOCAL.DAT REL\MOVED.DAT
DIR REL /B
CHDIR INNER
CHDIR .
CHDIR
CHDIR ..
CHDIR
MKDIR 1:\DATA
CHDIR 1:\DATA
CHDIR
CHDIR 1:
DRIVE 1
CHDIR
COPY 0:\SOURCE.DAT COPY.DAT /Y
DIR /B
COPY 0:\SOURCE.DAT 0:REL\CROSS.DAT /Y
CHDIR 0:
DRIVE 0
CHDIR
CHDIR \
CHDIR ..
CHDIR
'''
# The explicit 0: relative destination starts at drive 0's TOOLS.
result, saved = run('chdir-paths', commands, raw, disks={1: other},
                    copies={1: other_saved}, steps=500000000)
status(result, 0)
assert 'CHDIR [path]\n' in result.stdout, result.stdout
assert '0:>CHDIR\n0:\\TOOLS\n' in result.stdout, result.stdout
assert '0:>CHDIR\n0:\\TOOLS\\INNER\n' in result.stdout, result.stdout
assert '0:>CHDIR 1:\n1:\\DATA\n0:>' in result.stdout, result.stdout
assert '1:>CHDIR\n1:\\DATA\n' in result.stdout, result.stdout
assert '1:>CHDIR 0:\n0:\\TOOLS\n1:>' in result.stdout, result.stdout
assert '0:>CHDIR\n0:\\\n0:>' in result.stdout, result.stdout
assert 'MOVED.DAT\n' in result.stdout and 'COPY.DAT\n' in result.stdout
assert 'not found' not in result.stdout and 'error' not in result.stdout, result.stdout
# DIR without a path must list TOOLS rather than repeat SOURCE.DAT from root.
listing = result.stdout.split('0:>DIR /B\n', 1)[1].split('0:>', 1)[0]
assert 'INNER\n' in listing and 'SOURCE.DAT' not in listing, listing
assert saved[163 * 512:172 * 512] == saved[172 * 512:181 * 512]
tools = cluster_of(row_at(saved, 0, 'TOOLS'))
relative = cluster_of(row_at(saved, tools, 'REL'))
for filename in ('MOVED.DAT', 'CROSS.DAT'):
    row = row_at(saved, relative, filename)
    start = (195 + cluster_of(row) - 2) * 512
    assert saved[start:start + 22] == b'relative file contents'
other_raw = other_saved.read_bytes()
directory = cluster_of(row_at(other_raw, 0, 'DATA'))
row = row_at(other_raw, directory, 'COPY.DAT')
start = (195 + cluster_of(row) - 2) * 512
assert other_raw[start:start + 22] == b'relative file contents'


setup = 'MKDIR TOOLS\nMKDIR TOOLS\\INNER\nCHDIR TOOLS\n'
for index, (argument, code) in enumerate([
        ('MISSING', 3), ('..\\SOURCE.DAT', 5), ('*', 3), ('/D TOOLS', 87),
        ('TOOLS EXTRA', 87), ('""', 87), ('"TOOLS', 87), ('4:\\', 15), ('4:', 15),
        ('A:\\', 15), ('2:\\', 21), ('/BAD', 87)]):
    bad = 'CHDIR ' + argument + '\n'
    result, _ = run(f'chdir-invalid-{index}', setup + bad + 'CHDIR\n' + bad, raw)
    status(result, code)
    assert '0:>CHDIR\n0:\\TOOLS\n' in result.stdout, result.stdout

for index, (command, code) in enumerate([
        ('RMDIR 0:\\TOOLS\\INNER', 16),
        ('MOVE 0:\\TOOLS\\INNER 0:\\RENAMED', 5),
        ('MOVE 0:\\TOOLS 0:\\RENAMED', 5)]):
    result, _ = run(f'chdir-protect-{index}', setup + 'CHDIR INNER\n' + command + '\n', raw)
    status(result, code)
result, _ = run('chdir-prefix-boundary',
                'MKDIR A\nMKDIR AB\nCHDIR AB\nMOVE \\A \\RENAMED\nCHDIR\n', raw)
status(result, 0)
assert '0:\\AB\n' in result.stdout, result.stdout

# The maximum stored path is 63 characters including its initial backslash.
commands = ''
for index in range(7):
    commands += f'MKDIR LEVEL00{index}\nCHDIR LEVEL00{index}\n'
longpath = '\\' + '\\'.join(f'LEVEL00{i}' for i in range(7))
assert len(longpath) == 63
commands += 'CHDIR\nCHDIR A\nCHDIR\nCHDIR A\n'
result, _ = run('chdir-path-limit', commands, steps=500000000)
status(result, 206)
assert result.stdout.count(longpath + '\n') == 2, result.stdout

for name, cluster, following in [('zero', 0, 0), ('free', 2, 0),
                                  ('reserved', 2, 0xff0), ('cycle', 2, 2)]:
    broken = base()
    broken[181 * 512:181 * 512 + 32] = entry('BROKEN', cluster, 0, 16)
    if cluster:
        fatset(broken, cluster, following)
    result, _ = run('chdir-corrupt-' + name, 'CHDIR BROKEN\nCHDIR\nCHDIR BROKEN\n', broken)
    status(result, 13)
    assert '0:>CHDIR\n0:\\\n' in result.stdout, result.stdout


def check(register, value):
    prefix = 'ADD R5,R0,RZR\n' if register == 'R0' else ''
    register = 'R5' if register == 'R0' else register
    return prefix + f'DosConst R0,{value}\nSUB R0,{register},R0\nSRZ R0\nJMP bad\n'


def call(function, argument, error=0):
    return f'DosConst R2,{argument}\nDosCall {function}\n' + check('R0', error)


# Change cwd with a handle open; its file identity and position must survive.
body = 'ADI R3,RZR,1\n' + call('DOS_OPEN', '80000h+filename') + check('R2', 1)
body += call('DOS_CHDIR', '80000h+parent')
body += check('R1', 'DOS_CHDIR') + check('R2', '80000h+parent') + check('R3', 1)
body += 'ADI R2,RZR,1\nDosConst R3,80000h+bytes\nADI R4,RZR,3\nDosCall DOS_WRITE\n'
body += check('R0', 0) + check('R2', 3) + call('DOS_CLOSE', 1)
body += call('DOS_CHDIR', '80000h+folder')
body += 'ADI R3,RZR,0\n' + call('DOS_OPEN', '80000h+filename')
body += 'ADI R2,RZR,1\nDosConst R3,0d0000h\nADI R4,RZR,3\nDosCall DOS_READ\n'
body += check('R0', 0) + check('R2', 3)
for offset, value in [(0, 0), (4, 128), (8, 255)]:
    body += f'DosConst R0,0d0000h\nLRM R6,[R0+{offset}]\n' + check('R6', value)
body += call('DOS_CHDIR', '80000h+empty') + call('DOS_CLOSE', 1)
body += call('DOS_CHDIR', '80000h+missing', 3)
body += call('DOS_CHDIR', '0d0001h', 87)
body += 'ADI R1,RZR,DOS_EXIT\nADI R2,RZR,61\nJMP 0fff0h\n'
body += 'bad:\nADI R1,RZR,DOS_EXIT\nADI R2,RZR,99\nJMP 0fff0h\n'
for label, text in [('filename', 'LOCAL.DAT'), ('parent', '..'), ('folder', 'TOOLS'),
                    ('empty', ''), ('missing', 'MISSING')]:
    body += label + ':\nDD ' + ','.join([*(str(ord(c)) for c in text), '0']) + '\n'
body += 'bytes:\nDD 0,128,255\n'
source = OUT / 'chdir-app.asm'
binary = OUT / 'chdir-app.bin'
source.write_text(f'CPU YOSIC-KAPPA\nINCLUDE "{DOS / "api.inc"}"\n' +
                  '\n'.join('        ' + line for line in body.splitlines()) + '\nEND\n')
subprocess.run([str(TOOLS / 'yosic-as.exe'), str(source), '-o', str(binary)], check=True)
app_raw = base()
add_file(app_raw, 0, 'APP.YKE', encode(binary.read_bytes()))
result, saved = run('chdir-app-run', 'MKDIR TOOLS\nCOPY APP.YKE TOOLS /Y\nCHDIR TOOLS\nAPP\n',
                    app_raw, steps=500000000)
status(result, 61)
assert '0:\\TOOLS\n' in result.stdout, result.stdout
root = saved[181 * 512:195 * 512]
row = next(root[i:i + 32] for i in range(0, len(root), 32)
           if root[i:i + 11] == b'TOOLS      ')
directory = struct.unpack_from('<H', row, 26)[0]
start = (195 + directory - 2) * 512
row = next(saved[i:i + 32] for i in range(start, start + 512, 32)
           if saved[i:i + 11] == b'LOCAL   DAT')
cluster = struct.unpack_from('<H', row, 26)[0]
assert struct.unpack_from('<I', row, 28)[0] == 3
start = (195 + cluster - 2) * 512
assert saved[start:start + 3] == bytes((0, 128, 255))
print('PASS CHDIR: per-drive/relative paths, DIR/COPY/MOVE, bounds/errors, cwd protection, YKE and handle/syscall I/O')
