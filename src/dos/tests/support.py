"""CPU/FDC integration test helpers; fixtures never enter the public tree."""
import pathlib
import re
import struct
import subprocess
import sys

ROOT = pathlib.Path(__file__).resolve().parents[3]
DOS = ROOT / 'src/dos'
OUT = ROOT / 'out/dos'
TOOLS = ROOT / 'out/build'
sys.path.insert(0, str(DOS / 'tools'))
from make_yke import encode


def symbol(name):
    symbols = (OUT / 'dos.map').read_text()
    return 0x20000 + int(re.search(rf'{name.upper()} = (0x[0-9a-fA-F]+)', symbols)[1], 16)


def fatset(raw, cluster, value):
    for start in (163, 172):
        offset = start * 512 + cluster + cluster // 2
        old = struct.unpack_from('<H', raw, offset)[0]
        new = (old & 15) | (value << 4) if cluster & 1 else (old & 0xf000) | value
        struct.pack_into('<H', raw, offset, new)


def entry(name, cluster, size, attr=32):
    stem, _, ext = name.upper().partition('.')
    result = bytearray(32)
    result[:11] = stem.ljust(8).encode() + ext.ljust(3).encode()
    result[11] = attr
    struct.pack_into('<H', result, 26, cluster)
    struct.pack_into('<I', result, 28, size)
    return result


def add_file(raw, index, name, data, clusters=None, directory=0):
    count = (len(data) + 511) // 512
    if clusters is None:
        clusters = list(range(2 + index * 140, 2 + index * 140 + count))
    assert len(clusters) == count
    for i, cluster in enumerate(clusters):
        fatset(raw, cluster, clusters[i + 1] if i + 1 < count else 0xfff)
        offset = (195 + cluster - 2) * 512
        chunk = data[i * 512:(i + 1) * 512]
        raw[offset:offset + len(chunk)] = chunk
    offset = ((195 + directory - 2) if directory else 181) * 512 + index * 32
    raw[offset:offset + 32] = entry(name, clusters[0] if count else 0, len(data))
    return clusters


def base():
    return bytearray((OUT / 'dos.img').read_bytes())


def run(name, commands, raw=None, disks=None, dump=0x13f00, words=1,
        copies=None, steps=100000000):
    media = OUT / f'{name}.img'
    media.write_bytes(base() if raw is None else raw)
    input_file = OUT / f'{name}-input.txt'
    input_file.write_bytes(commands.encode('ascii'))
    command = [str(TOOLS / 'yosic-sim.exe'), '--no-monitor', '--rom', str(TOOLS / 'boot/ipl.rom'),
               '--geometry', '80', '2', '18', '2', '--disk', '0', str(media)]
    if disks:
        for drive, path in disks.items():
            command += ['--disk', str(drive), str(path)]
    if copies:
        for drive, path in copies.items():
            path.unlink(missing_ok=True)
            command += ['--disk-copy', str(drive), str(path)]
    saved = OUT / f'{name}-saved.img'
    saved.unlink(missing_ok=True)
    command += ['--disk-copy', '0', str(saved), '--input', str(input_file),
                '--break', hex(symbol('INPUT_DONE')), '--steps', str(steps),
                '--registers', '--dump', hex(dump), str(words)]
    result = subprocess.run(command, capture_output=True, text=True, timeout=60)
    (OUT / f'{name}.log').write_text(result.stdout + result.stderr)
    assert result.returncode == 0, result.stderr
    assert 'stop=breakpoint' in result.stderr, result.stderr
    assert 'R14=0xf000' in result.stderr, result.stderr
    assert 'R10=0xfe0000' in result.stderr, result.stderr
    assert 'R12=0x28000' in result.stderr, result.stderr
    return result, saved.read_bytes()


def status(result, value):
    assert f'mem[0x13f00]=0x{value:x}' in result.stderr, result.stderr
