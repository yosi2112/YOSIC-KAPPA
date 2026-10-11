"""Run disk-loaded YKE1 code on the actual CPU and FDC, including rejection paths."""
import struct
import subprocess

from support import DOS, OUT, TOOLS, add_file, base, encode, entry, fatset, run, status


def main():
    hello_bin = OUT / 'hello.bin'
    subprocess.run([str(TOOLS / 'yosic-as.exe'), str(DOS / 'examples/hello.asm'),
                    '-o', str(hello_bin)], check=True)
    payload = hello_bin.read_bytes()
    image = encode(payload)
    (OUT / 'HELLO.YKE').write_bytes(image)
    raw = base()
    add_file(raw, 0, 'HELLO.YKE', image)
    result, saved = run('yke-normal', 'hello\r\nDRIVE\r\nHELLO.YKE\r\n', raw)
    assert result.stdout.count('HELLO YKE\n') == 2, result.stdout
    assert '0:>DRIVE\n0:\n0:>HELLO.YKE' in result.stdout, result.stdout
    assert saved == raw, 'Executing a file must not change the disk'
    status(result, 7)

    # Enter after an illegal first word, span non-contiguous clusters and tracks.
    raw = base()
    offset_source = OUT / 'offset.asm'
    offset_source.write_text('ORG 4\n' + (DOS / 'examples/hello.asm').read_text())
    offset_bin = OUT / 'offset.bin'
    subprocess.run([str(TOOLS / 'yosic-as.exe'), str(offset_source), '-o', str(offset_bin)], check=True)
    large = b'\xff' * 4 + offset_bin.read_bytes()[4:] + bytes(1536)
    add_file(raw, 0, 'FRAG.YKE', encode(large, 4), [2, 100, 3, 200])
    result, _ = run('yke-fragmented', 'FRAG\n', raw)
    assert 'HELLO YKE\n' in result.stdout, result.stdout
    status(result, 7)

    # Largest accepted payload; its trailing words remain available as code/data.
    raw = base()
    maximum = payload + bytes(65528 - len(payload))
    add_file(raw, 0, 'MAX.YKE', encode(maximum))
    result, _ = run('yke-maximum', 'MAX\n', raw)
    assert 'HELLO YKE\n' in result.stdout, result.stdout
    status(result, 7)

    # Guest copies the untouched tail and length/drive into diagnostic RAM.
    args_source = OUT / 'args.asm'
    args_source.write_text('''CPU YOSIC-KAPPA
LRM R0,[R1]
SRM R0,[RZR+1000h]
SRM R2,[RZR+1004h]
SRM R3,[RZR+1008h]
ADI R0,RZR,0
ADI R5,RZR,2
JMP 0fff8h
END
''')
    args_bin = OUT / 'args.bin'
    subprocess.run([str(TOOLS / 'yosic-as.exe'), str(args_source), '-o', str(args_bin)], check=True)
    raw = base()
    add_file(raw, 0, 'ARGS.YKE', encode(args_bin.read_bytes()))
    result, _ = run('yke-arguments', '"args.yke" MiXeD "arg"\n', raw, dump=0x1000, words=3)
    for address, value in ((0x1000, 77), (0x1004, 11), (0x1008, 0)):
        assert f'mem[0x{address:x}]=0x{value:x}' in result.stderr, result.stderr

    # A returning application may change DS/SS/ES, SP and console/bias registers.
    state_source = OUT / 'state.asm'
    state_source.write_text('''CPU YOSIC-KAPPA
ADI R0,RZR,3
LSEG DS,R0
LSEG SS,R0
LSEG ES,R0
ADI SP,RZR,0
ADI R10,RZR,0
ADI R11,RZR,0
ADI R12,RZR,0
ADI R0,RZR,23
ADI R5,RZR,2
JMP 0fff8h
END
''')
    state_bin = OUT / 'state.bin'
    subprocess.run([str(TOOLS / 'yosic-as.exe'), str(state_source), '-o', str(state_bin)], check=True)
    raw = base()
    add_file(raw, 0, 'STATE.YKE', encode(state_bin.read_bytes()))
    result, _ = run('yke-state', 'STATE\r\nDRIVE\r\nSTATE\r\n', raw)
    assert '0:>DRIVE\n0:\n0:>STATE\n0:>' in result.stdout, result.stdout
    status(result, 23)
    for segment in range(1, 4):
        assert f'SEG{segment}=0x0' in result.stderr, result.stderr

    raw = base()
    # Subdirectory lookup and explicit drive selection.
    fatset(raw, 600, 0xfff)
    raw[181 * 512:181 * 512 + 32] = entry('SUB', 600, 0, 16)
    add_file(raw, 0, 'HELLO.YKE', image, [601], directory=600)
    other = OUT / 'yke-other.img'
    other.write_bytes(raw)
    result, _ = run('yke-path', 'B:\\SUB\\HELLO.YKE\nDRIVE 1\n\\SUB\\HELLO\n',
                    disks={1: other})
    assert result.stdout.count('HELLO YKE\n') == 2, result.stdout
    assert result.stdout.endswith('1:>'), result.stdout
    status(result, 7)

    broken = {}
    for name, offset, value in [('magic', 0, 0), ('version', 4, 2),
                                ('length', 8, len(payload) + 4), ('entry-align', 12, 1),
                                ('entry-bound', 12, len(payload)), ('entry-negative', 12, 0xffffffff),
                                ('checksum', 16, 0), ('reserved', 20, 1)]:
        bad = bytearray(image)
        struct.pack_into('<I', bad, offset, value)
        broken[name] = bad
    broken['empty'] = b''
    broken['short'] = b'YKE1'
    broken['unaligned'] = image + b'x'
    broken['oversize'] = image + bytes(65556 - len(image))
    for name, bad in broken.items():
        raw = base()
        add_file(raw, 0, 'BAD.YKE', bad)
        result, _ = run('yke-reject-' + name, 'BAD\nDRIVE\nBAD\n', raw)
        assert result.stdout.count('Invalid YKE executable\n') == 2, result.stdout
        assert 'HELLO YKE' not in result.stdout, result.stdout
        status(result, 11)

    for name, change in [('cycle', lambda raw: fatset(raw, 2, 2)),
                          ('short-chain', lambda raw: fatset(raw, 2, 0xfff)),
                          ('free-chain', lambda raw: fatset(raw, 2, 0)),
                          ('bad-cluster', lambda raw: fatset(raw, 2, 0xff7)),
                          ('extra-chain', lambda raw: (fatset(raw, 2, 3), fatset(raw, 3, 4),
                                                      fatset(raw, 4, 5), fatset(raw, 5, 6),
                                                      fatset(raw, 6, 0xfff)))]:
        raw = base()
        add_file(raw, 0, 'BAD.YKE', encode(payload + bytes(1536)))
        change(raw)
        result, _ = run('yke-chain-' + name, 'BAD\n', raw)
        assert 'Invalid disk or data error\n' in result.stdout, result.stdout
        assert 'HELLO YKE' not in result.stdout, result.stdout
        status(result, 13)

    for name, line, error, value in [('missing', 'MISSING\n', 'File or path not found', 3),
                                    ('unmounted', 'B:\\HELLO\n', 'Drive not ready', 21),
                                    ('extension', 'HELLO.BIN\n', 'Invalid YKE executable', 11),
                                    ('directory', 'DIR.YKE\n', 'Access denied', 5),
                                    ('quote', '"HELLO\n', 'Invalid parameter', 87)]:
        raw = base()
        raw[181 * 512:181 * 512 + 32] = entry('DIR.YKE', 2, 0, 16)
        result, _ = run('yke-' + name, line, raw)
        assert error + '\n' in result.stdout, result.stdout
        status(result, value)
    print('PASS YKE1/.YKE: native launch/return, arguments, fragmented/max-size files, '
          'entry offsets, paths/drives, header/chain errors, missing media, shell state')


if __name__ == '__main__':
    main()
