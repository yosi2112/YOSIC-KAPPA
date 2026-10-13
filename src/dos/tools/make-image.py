"""Generate KPB1 + loader + native kernel + FAT12 raw/D88 media."""
import pathlib
import struct
import sys


def checksum(data):
    assert len(data) % 4 == 0
    return sum(word[0] for word in struct.iter_unpack('<I', data)) & 0xffffffff


def build(directory):
    kernel = (directory / 'dos.bin').read_bytes()
    loader = (directory / 'loader.bin').read_bytes()
    assert 4 <= len(loader) <= 8192 and len(loader) % 4 == 0
    assert 4 <= len(kernel) <= 65528 and len(kernel) % 4 == 0
    raw = bytearray(2880 * 512)
    struct.pack_into('<8I', raw, 0, 0x3150424b, 1, len(loader), 0x2000,
                     0x2000, checksum(loader), (len(loader) + 511) // 512, 0)
    raw[512:512 + len(loader)] = loader
    raw[18 * 512:18 * 512 + len(kernel)] = kernel
    # SYS and core.select use a fixed FAT volume start at LBA 162.
    start, total, fatsize, entries = 162, 2880 - 162, 9, 224
    bpb = bytearray(512)
    bpb[:11] = b'\xeb\x3c\x90YOSICDOS'
    struct.pack_into('<HBHBHHBHHHII', bpb, 11, 512, 1, 1, 2, entries,
                     total, 0xf0, fatsize, 18, 2, start, 0)
    bpb[38] = 0x29
    struct.pack_into('<I', bpb, 39, 0x4b415050)
    bpb[43:54] = b'YOSIC-DOS  '
    bpb[54:62] = b'FAT12   '
    bpb[510:] = b'\x55\xaa'
    raw[start * 512:(start + 1) * 512] = bpb
    for copy in range(2):
        offset = (start + 1 + copy * fatsize) * 512
        raw[offset:offset + 3] = b'\xf0\xff\xff'
    header = bytearray(0x2b0)
    header[:14] = b'YOSIC-DOS 1.0K'
    header[0x1b] = 0x20
    tracks = bytearray()
    for track in range(160):
        struct.pack_into('<I', header, 0x20 + track * 4, len(header) + len(tracks))
        for sector in range(18):
            sector_header = bytearray(16)
            struct.pack_into('<4BH', sector_header, 0, track // 2, track % 2,
                             sector + 1, 2, 18)
            struct.pack_into('<H', sector_header, 14, 512)
            tracks += sector_header
            offset = (track * 18 + sector) * 512
            tracks += raw[offset:offset + 512]
    struct.pack_into('<I', header, 0x1c, len(header) + len(tracks))
    d88 = header + tracks
    # Independently read back every sector and the IPL contract before publishing.
    recovered = bytearray()
    for track in range(160):
        offset = struct.unpack_from('<I', d88, 0x20 + track * 4)[0]
        for sector in range(18):
            pos = offset + sector * 528
            assert d88[pos:pos + 4] == bytes((track // 2, track % 2, sector + 1, 2))
            assert struct.unpack_from('<H', d88, pos + 14)[0] == 512
            recovered += d88[pos + 16:pos + 528]
    assert recovered == raw
    contract = struct.unpack_from('<8I', recovered)
    assert contract == (0x3150424b, 1, len(loader), 0x2000, 0x2000,
                        checksum(loader), (len(loader) + 511) // 512, 0)
    assert recovered[18 * 512:18 * 512 + len(kernel)] == kernel
    (directory / 'dos.img').write_bytes(raw)
    (directory / 'dos.d88').write_bytes(d88)
    print('PASS KPB1 header, checksum, kernel placement and all D88 sectors')


if __name__ == '__main__':
    build(pathlib.Path(sys.argv[1]))
