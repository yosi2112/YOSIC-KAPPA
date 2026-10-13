"""Regression checks for all eight builtins, including persisted FAT12 changes."""
import struct

from support import OUT, add_file, base, run, status


raw = base()
data = bytes(range(256)) * 3
add_file(raw, 0, 'SOURCE.DAT', data, [2, 100])
commands = '''DIR /B
DIR /?
MKDIR /?
RMDIR /?
DRIVE /?
COPY /?
MOVE /?
DELETE /?
SYS /?
MKDIR TEST
COPY SOURCE.DAT TEST\\COPY.DAT /Y
MOVE TEST\\COPY.DAT TEST\\MOVED.DAT
DIR TEST /B
RMDIR TEST
DELETE TEST\\MOVED.DAT
RMDIR TEST
COPY SOURCE.DAT FINAL.DAT /Y
DELETE SOURCE.DAT
DIR /B
DRIVE
'''
result, saved = run('command-regression', commands, raw, steps=500000000)
assert 'MOVED.DAT\n' in result.stdout, result.stdout
assert 'Access denied\n' in result.stdout, result.stdout
assert 'FINAL.DAT\n' in result.stdout, result.stdout
status(result, 0)
root = saved[181 * 512:195 * 512]
active = [root[i:i + 32] for i in range(0, len(root), 32)
          if root[i] not in (0, 229)]
assert len(active) == 1 and active[0][:11] == b'FINAL   DAT', active
cluster = struct.unpack_from('<H', active[0], 26)[0]
size = struct.unpack_from('<I', active[0], 28)[0]
recovered = bytearray()
seen = set()
while cluster < 0xff8:
    assert cluster >= 2 and cluster not in seen
    seen.add(cluster)
    offset = (195 + cluster - 2) * 512
    recovered += saved[offset:offset + 512]
    fat_offset = 163 * 512 + cluster + cluster // 2
    word = struct.unpack_from('<H', saved, fat_offset)[0]
    cluster = word >> 4 if cluster & 1 else word & 0xfff
assert recovered[:size] == data
assert saved[163 * 512:172 * 512] == saved[172 * 512:181 * 512]
assert 'SYS 1:|2:|3:\n' in result.stdout, result.stdout

# Numeric paths also govern builtins and MOVE's internally reconstructed source.
raw = base()
add_file(raw, 0, 'SOURCE.DAT', data, [2, 100])
other = OUT / 'command-numeric-drive1.img'
other.write_bytes(base())
other_saved = OUT / 'command-numeric-drive1-saved.img'
result, saved = run('command-numeric', '''COPY 0:\\SOURCE.DAT 1:\\COPY.DAT /Y
DIR 1:\\
MOVE 1:\\COPY.DAT 0:\\MOVED.DAT
DIR 1:\\ /B
DELETE 0:\\SOURCE.DAT
DIR 0:\\ /B
''', raw, disks={1: other}, copies={1: other_saved}, steps=500000000)
assert ' Directory of 1:\\\n' in result.stdout, result.stdout
assert 'MOVED.DAT\n' in result.stdout, result.stdout
status(result, 0)
root = saved[181 * 512:195 * 512]
active = [root[i:i + 32] for i in range(0, len(root), 32)
          if root[i] not in (0, 229)]
assert len(active) == 1 and active[0][:11] == b'MOVED   DAT', active
assert struct.unpack_from('<I', active[0], 28)[0] == len(data)
other_root = other_saved.read_bytes()[181 * 512:195 * 512]
assert all(other_root[i] in (0, 229) for i in range(0, len(other_root), 32))

for argument, value in [('0:', 5), ('4:', 15), ('A:', 15), ('B:', 15), ('1::', 15)]:
    result, _ = run(f'sys-invalid-{argument.replace(":", "")}', f'SYS {argument}\n')
    status(result, value)

target = base()
add_file(target, 0, 'KEEP.DAT', b'preserve target files')
target_path = OUT / 'sys-target.img'
target_path.write_bytes(target)
copy_path = OUT / 'sys-target-saved.img'
for drive in range(1, 4):
    result, _ = run(f'sys-regression-{drive}', f'SYS {drive}:\n', disks={drive: target_path},
                    copies={drive: copy_path}, steps=500000000)
    assert 'System transferred\n' in result.stdout, result.stdout
    status(result, 0)
    transferred = copy_path.read_bytes()
    assert transferred[:162 * 512] == base()[:162 * 512]
    assert transferred[162 * 512:] == target[162 * 512:]
    result, _ = run(f'sys-boot-{drive}', 'DIR /B\n', raw=transferred)
    assert 'KEEP.DAT\n' in result.stdout, result.stdout
print('PASS all 8 builtins, persisted binary copies/move/delete, FAT mirrors, SYS and reboot')
