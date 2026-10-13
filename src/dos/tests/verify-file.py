"""Guest handle I/O: binary contents, positions, FAT12 persistence and failures."""
import struct
import subprocess

from support import DOS, OUT, TOOLS, add_file, base, encode, fatset, run, status


def assemble(name, body, raw=None):
    source = OUT / f'{name}.asm'
    binary = OUT / f'{name}.bin'
    source.write_text(f'CPU YOSIC-KAPPA\nINCLUDE "{DOS / "api.inc"}"\n' +
                      '\n'.join('        ' + line for line in body.splitlines()) + '\nEND\n')
    subprocess.run([str(TOOLS / 'yosic-as.exe'), str(source), '-o', str(binary)], check=True)
    raw = base() if raw is None else bytearray(raw)
    add_file(raw, 0, 'FILE.YKE', encode(binary.read_bytes()))
    return raw


def check(register, value):
    prefix = 'ADD R5,R0,RZR\n' if register == 'R0' else ''
    register = 'R5' if register == 'R0' else register
    return prefix + f'DosConst R0,{value}\nSUB R0,{register},R0\nSRZ R0\nJMP bad\n'


def call(function, r2, r3=0, r4=0, error=0, result=None):
    text = ''
    for register, value in [('R2', r2), ('R3', r3), ('R4', r4)]:
        if str(value).startswith('R'):
            text += f'ADD {register},{value},RZR\n'
        else:
            text += f'DosConst {register},{value}\n'
    text += f'DosCall {function}\n' + check('R0', error)
    if result is not None:
        text += check('R2', result)
    return text


def finish(code=41):
    return (f'ADI R1,RZR,DOS_EXIT\nADI R2,RZR,{code}\nJMP 0fff0h\n'
            'bad:\nSRM R8,[RZR+1000h]\nSRM R9,[RZR+1004h]\nSRM R10,[RZR+1008h]\n'
            'SRM R1,[RZR+100ch]\nSRM R2,[RZR+1010h]\nSRM R0,[RZR+1014h]\n'
            'ADI R1,RZR,DOS_EXIT\nADI R2,RZR,99\nJMP 0fff0h\n')


def text(label, value):
    return label + ':\nDD ' + ','.join(str(ord(c)) for c in value) + ',0\n'


def data(label, value):
    return label + ':\nDD ' + ','.join(str(c) for c in value) + '\n'


def compare(label, expected, offset, count):
    return (f'DosConst R8,0d0000h\nDosConst R9,80000h+{expected}+{offset * 4}\n'
            f'DosConst R10,{count}\n{label}:\nLRM R0,[R8]\nLRM R5,[R9]\n'
            'SUB R0,R0,R5\nSRZ R0\nJMP bad\nADI R8,R8,4\nADI R9,R9,4\n'
            f'ADI R10,R10,-1\nSRZ R10\nJMP {label}\n')


def fatget(raw, cluster):
    word = struct.unpack_from('<H', raw, 163 * 512 + cluster + cluster // 2)[0]
    return word >> 4 if cluster & 1 else word & 0xfff


def file_bytes(raw, name):
    stem, _, ext = name.partition('.')
    key = (stem.ljust(8) + ext.ljust(3)).encode()
    root = raw[181 * 512:195 * 512]
    row = next(root[i:i + 32] for i in range(0, len(root), 32) if root[i:i + 11] == key)
    size = struct.unpack_from('<I', row, 28)[0]
    cluster = struct.unpack_from('<H', row, 26)[0]
    output = bytearray()
    seen = set()
    while cluster:
        assert 2 <= cluster < 4088 and cluster not in seen
        seen.add(cluster)
        start = (195 + cluster - 2) * 512
        output += raw[start:start + 512]
        cluster = fatget(raw, cluster)
        if cluster >= 0xff8:
            break
    assert len(output) >= size
    assert raw[163 * 512:172 * 512] == raw[172 * 512:181 * 512]
    return bytes(output[:size])


payload = bytes((i * 37) % 256 for i in range(1500))
replacement = bytes((i * 13) % 256 for i in range(64))
changed = payload[:100] + replacement + payload[164:]
body = call('DOS_OPEN', '80000h+input', 0, result=1)
body += 'ADD R6,R2,RZR\n'
offset = 0
for i, (requested, actual) in enumerate([(700, 700), (620, 620), (700, 180)]):
    body += call('DOS_READ', 'R6', '0d0000h', requested, result=actual)
    body += compare(f'compare_input_{i}', 'expected', offset, actual)
    offset += actual
body += call('DOS_READ', 'R6', '0d0000h', 700, result=0)
body += call('DOS_CLOSE', 'R6')
body += call('DOS_OPEN', '80000h+output', 1, result=1)
body += 'ADD R6,R2,RZR\n'
offset = 0
for count in (511, 2, 987):
    body += call('DOS_WRITE', 'R6', f'80000h+expected+{offset * 4}', count, result=count)
    offset += count
body += call('DOS_WRITE', 'R6', 0, 0, result=0)
body += call('DOS_CLOSE', 'R6')
body += call('DOS_OPEN', '80000h+output', 2, result=1)
body += call('DOS_READ', 1, '0d0000h', 100, result=100)
body += call('DOS_WRITE', 1, '80000h+replacement', 64, result=64)
body += call('DOS_CLOSE', 1)
body += call('DOS_OPEN', '80000h+output', 0, result=1)
body += call('DOS_READ', 1, '0d0000h', 1800, result=1500)
body += compare('compare_changed', 'changed', 0, 1500)
body += call('DOS_READ', 1, 0, 0, result=0)
body += call('DOS_CLOSE', 1)
body += finish()
body += text('input', '1:\\INPUT.DAT') + text('output', '0:\\OUTPUT.DAT')
body += data('expected', payload) + data('replacement', replacement) + data('changed', changed)
raw = assemble('file-roundtrip-app', body)
other = base()
add_file(other, 0, 'INPUT.DAT', payload, [600, 800, 601])
other_path = OUT / 'file-roundtrip-drive1.img'
other_path.write_bytes(other)
result, saved = run('file-roundtrip', 'FILE\n', raw, disks={1: other_path}, steps=500000000)
status(result, 41)
assert file_bytes(saved, 'OUTPUT.DAT') == changed

# Truncate and overwrite an existing fragmented file, releasing its chain.
body = call('DOS_OPEN', '80000h+path', 1, result=1)
body += call('DOS_WRITE', 1, '80000h+bytes', 17, result=17)
body += call('DOS_CLOSE', 1) + finish(42)
body += text('path', 'OUT.DAT') + data('bytes', payload[:17])
raw = assemble('file-truncate-app', body)
add_file(raw, 1, 'OUT.DAT', payload, [600, 800, 601])
result, saved = run('file-truncate', 'FILE\n', raw, steps=500000000)
status(result, 42)
assert file_bytes(saved, 'OUT.DAT') == payload[:17]
for cluster in (600, 800, 601):
    assert fatget(saved, cluster) == 0

# Handles are exclusive, bounded, reusable, and automatically closed by EXIT.
body = call('DOS_OPEN', '80000h+f1', 0, result=1)
body += call('DOS_OPEN', '80000h+f1', 1, error=5, result=0)
body += call('DOS_DELETE', '80000h+f1', error=5)
body += call('DOS_WRITE', 1, '80000h+f1', 1, error=5, result=0)
for number in range(2, 9):
    body += call('DOS_OPEN', f'80000h+f{number}', 0, result=number)
body += call('DOS_OPEN', '80000h+f9', 0, error=4, result=0)
body += call('DOS_CLOSE', 1)
body += call('DOS_OPEN', '80000h+f9', 0, result=1)
body += finish(43)
for number in range(1, 10):
    body += text(f'f{number}', f'F{number}.DAT')
raw = assemble('file-handles-app', body)
for number in range(1, 10):
    add_file(raw, number, f'F{number}.DAT', b'content')
result, saved = run('file-handles', 'FILE\nFILE\n', raw, steps=500000000)
status(result, 43)
assert saved == raw

# Parameter/access failures leave files and live handles intact.
body = ''
for number in (0, 9, '0ffffffffh'):
    body += call('DOS_CLOSE', number, error=6)
    body += call('DOS_READ', number, '0d0000h', 1, error=6, result=0)
for mode in (3, '0ffffffffh'):
    body += call('DOS_OPEN', '80000h+path', mode, error=87, result=0)
for pointer in (0, '0d0001h', '0fe0000h'):
    body += call('DOS_OPEN', pointer, 0, error=87, result=0)
body += call('DOS_OPEN', '80000h+missing', 0, error=3, result=0)
body += call('DOS_OPEN', '80000h+space', 0, error=87, result=0)
body += call('DOS_OPEN', '80000h+readonly', 1, error=5, result=0)
body += call('DOS_OPEN', '80000h+readonly', 2, error=5, result=0)
body += call('DOS_OPEN', '80000h+path', 2, result=1)
for pointer, count in [(0, 1), ('0d0001h', 1), ('8ffech', 2), ('0efffch', 2),
                       ('0fe0000h', 1), ('80000000h', 1), ('0d0000h', '0ffffffffh'),
                       ('0d0000h', 32769)]:
    body += call('DOS_READ', 1, pointer, count, error=87, result=0)
body += 'DosConst R8,0d0000h\nADI R0,RZR,256\nSRM R0,[R8]\n'
body += call('DOS_WRITE', 1, '0d0000h', 1, error=87, result=0)
body += call('DOS_READ', 1, '0d0000h', 17, result=17)
body += call('DOS_CLOSE', 1)
body += call('DOS_CLOSE', 1, error=6)
body += call('DOS_OPEN', '80000h+readonly', 0, result=1)
body += call('DOS_CLOSE', 1) + finish(44)
body += text('path', 'DATA.DAT') + text('missing', 'MISSING.DAT')
body += text('space', 'DATA.DAT EXTRA') + text('readonly', 'RO.DAT')
raw = assemble('file-errors-app', body)
add_file(raw, 1, 'DATA.DAT', payload)
add_file(raw, 2, 'RO.DAT', payload[:17])
raw[181 * 512 + 2 * 32 + 11] = 33
result, saved = run('file-errors', 'FILE\n', raw, steps=500000000)
status(result, 44)
assert saved == raw

# Write protection cannot truncate or modify an existing file on drive 1.
body = call('DOS_OPEN', '80000h+path', 1, error=19, result=0)
body += call('DOS_OPEN', '80000h+path', 2, result=1)
body += call('DOS_WRITE', 1, '80000h+bytes', 1, error=19, result=0)
body += call('DOS_CLOSE', 1, error=6) + finish(45)
body += text('path', '1:\\INPUT.DAT') + data('bytes', b'X')
result, _ = run('file-write-protect', 'FILE\n', assemble('file-write-protect-app', body),
                disks={1: other_path}, steps=500000000)
status(result, 45)
assert other_path.read_bytes() == other

# Disk full returns the committed prefix and leaves a usable handle/file.
body = call('DOS_OPEN', '80000h+path', 1, result=1)
body += call('DOS_WRITE', 1, '80000h+bytes', 700, error=112, result=512)
body += call('DOS_CLOSE', 1)
body += call('DOS_OPEN', '80000h+path', 0, result=1)
body += call('DOS_READ', 1, '0d0000h', 700, result=512)
body += compare('compare_prefix', 'bytes', 0, 512)
body += call('DOS_CLOSE', 1) + finish(46)
body += text('path', 'FULL.DAT') + data('bytes', payload[:700])
raw = assemble('file-full-app', body)
for cluster in range(2, 2687):
    if fatget(raw, cluster) == 0 and cluster != 600:
        fatset(raw, cluster, 0xfff)
result, saved = run('file-full', 'FILE\n', raw, steps=500000000)
status(result, 46)
assert file_bytes(saved, 'FULL.DAT') == payload[:512]

body = call('DOS_OPEN', '80000h+path', 0, error=13, result=0) + finish(47)
body += text('path', 'BAD.DAT')
raw = assemble('file-corrupt-app', body)
add_file(raw, 1, 'BAD.DAT', payload[:17], [600])
fatset(raw, 600, 600)
result, saved = run('file-corrupt', 'FILE\n', raw)
status(result, 47)
assert saved == raw
example_bin = OUT / 'file-example.bin'
subprocess.run([str(TOOLS / 'yosic-as.exe'), str(DOS / 'examples/file.asm'),
                '-o', str(example_bin)], check=True)
raw = base()
add_file(raw, 0, 'FILE.YKE', encode(example_bin.read_bytes()))
result, saved = run('file-example', 'FILE\n', raw)
status(result, 0)
assert file_bytes(saved, 'DEMO.DAT') == bytes([0, 1, 127, 128, 255])
print('PASS handle OPEN/CLOSE/READ/WRITE: fragmented/cross-drive binary I/O, EOF, '
      'positions, create/truncate/overwrite, FAT mirrors, handle lifecycle, '
      'bounds/permissions/write protection/corruption/full-disk prefix')
