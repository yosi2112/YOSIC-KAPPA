"""Generate bootable KAPPA-DOS FAT12 media for the built-in disk profiles."""
import pathlib
import struct
import sys


def checksum(data):
    assert len(data) % 4 == 0
    return sum(word[0] for word in struct.iter_unpack('<I', data)) & 0xffffffff


def fat12_set(fat, cluster, value):
    offset = cluster + cluster // 2
    if cluster & 1:
        fat[offset] = (fat[offset] & 0x0f) | ((value << 4) & 0xf0)
        fat[offset + 1] = (value >> 4) & 0xff
    else:
        fat[offset] = value & 0xff
        fat[offset + 1] = (fat[offset + 1] & 0xf0) | ((value >> 8) & 0x0f)


def fat12_chain(fat, first, count):
    for i in range(count):
        fat12_set(fat, first + i, first + i + 1 if i + 1 < count else 0xfff)


def make_entry(name, extension, attributes, first_cluster, size):
    entry = bytearray(32)
    entry[:8] = name.encode('ascii').ljust(8, b' ')
    entry[8:11] = extension.encode('ascii').ljust(3, b' ')
    entry[11] = attributes
    struct.pack_into('<H', entry, 26, first_cluster)
    struct.pack_into('<I', entry, 28, size)
    return entry


def build_profile(directory, cylinders, sectors_per_track, sector_size,
                  bytes_per_sector, sectors_per_fat, root_entries, media_id):
    kernel = (directory / 'dos.bin').read_bytes()
    loader = (directory / 'loader.bin').read_bytes()
    assert 4 <= len(loader) <= 8192 and len(loader) % 4 == 0
    assert 4 <= len(kernel) <= 65528 and len(kernel) % 4 == 0

    total_sectors = cylinders * 2 * sectors_per_track
    raw = bytearray(total_sectors * bytes_per_sector)
    root_sectors = (root_entries * 32 + bytes_per_sector - 1) // bytes_per_sector
    fat_start = 1
    root_start = fat_start + 2 * sectors_per_fat
    data_start = root_start + root_sectors
    cluster_bytes = bytes_per_sector
    loader_sectors = (len(loader) + 511) // 512
    payload = loader + bytes(loader_sectors * 512 - len(loader)) + kernel
    payload_clusters = (len(payload) + cluster_bytes - 1) // cluster_bytes

    # Place the boot payload at the first sector of cylinder 1. Clusters before
    # it are accounted for by a hidden reservation file, keeping the FAT valid.
    payload_start = 2 * sectors_per_track
    assert payload_start >= data_start
    payload_cluster = 2 + payload_start - data_start
    reservation_clusters = payload_cluster - 2
    assert payload_cluster + payload_clusters < 4085
    assert payload_cluster + payload_clusters <= total_sectors - data_start + 2

    bpb = memoryview(raw)[:bytes_per_sector]
    bpb[:3] = b'\xeb\x3c\x90'
    bpb[3:11] = b'KAPPA   '
    struct.pack_into('<HBHBHHBHHHII', bpb, 11, bytes_per_sector, 1, 1, 2,
                     root_entries, total_sectors, media_id, sectors_per_fat,
                     sectors_per_track, 2, 0, 0)
    bpb[36] = 0
    bpb[38] = 0x29
    struct.pack_into('<I', bpb, 39, 0x4b415050)
    bpb[43:54] = b'KAPPA-DOS  '
    bpb[54:62] = b'FAT12   '

    # KAPPA's IPL extension lives in the boot-code area after the standard
    # extended BPB. The filesystem BPB itself remains at byte offset zero.
    payload_cylinder, payload_head, payload_sector = 1, 0, 1
    virtual_spt = sectors_per_track * bytes_per_sector // 512
    payload_lba = payload_start * bytes_per_sector // 512
    struct.pack_into('<12I', bpb, 64, 0x3150424b, 1, len(loader), 0x2000,
                     0x2000, checksum(loader), loader_sectors,
                     virtual_spt, payload_lba, payload_cylinder, payload_head,
                     payload_sector)
    bpb[510:512] = b'\x55\xaa'

    fat_length = sectors_per_fat * bytes_per_sector
    fat0 = bytearray(fat_length)
    fat0[:3] = bytes((media_id, 0xff, 0xff))
    if reservation_clusters:
        fat12_chain(fat0, 2, reservation_clusters)
    fat12_chain(fat0, payload_cluster, payload_clusters)
    for copy in range(2):
        start = (fat_start + copy * sectors_per_fat) * bytes_per_sector
        raw[start:start + fat_length] = fat0

    root_offset = root_start * bytes_per_sector
    root_entries_data = bytearray(root_sectors * bytes_per_sector)
    if reservation_clusters:
        reserved_size = reservation_clusters * cluster_bytes
        root_entries_data[:32] = make_entry('KAPPA', 'RSV', 0x06, 2,
                                             reserved_size)
        payload_entry = 1
    else:
        payload_entry = 0
    payload_entry_offset = payload_entry * 32
    root_entries_data[payload_entry_offset:payload_entry_offset + 32] = make_entry(
        'KAPPA', 'SYS', 0x06, payload_cluster, len(payload))
    raw[root_offset:root_offset + len(root_entries_data)] = root_entries_data

    payload_offset = payload_start * bytes_per_sector
    raw[payload_offset:payload_offset + len(payload)] = payload

    physical_sectors_per_track = sectors_per_track
    physical_tracks = cylinders * 2
    header = bytearray(0x2b0)
    header[:14] = b'YOSIC-DOS 1.0K'
    header[0x1b] = 0x20
    tracks = bytearray()
    for track in range(physical_tracks):
        struct.pack_into('<I', header, 0x20 + track * 4, len(header) + len(tracks))
        for sector in range(physical_sectors_per_track):
            sector_header = bytearray(16)
            struct.pack_into('<4BH', sector_header, 0, track // 2, track % 2,
                             sector + 1, 2 if sector_size == 512 else 3,
                             physical_sectors_per_track)
            struct.pack_into('<H', sector_header, 14, sector_size)
            tracks += sector_header
            offset = (track * sectors_per_track + sector) * bytes_per_sector
            tracks += raw[offset:offset + sector_size]
    struct.pack_into('<I', header, 0x1c, len(header) + len(tracks))
    d88 = header + tracks

    # Check the BIOS-visible virtual-sector view and the physical D88 layout.
    assert struct.unpack_from('<HBHBHHBHHHII', raw, 11) == (
        bytes_per_sector, 1, 1, 2, root_entries, total_sectors,
        media_id, sectors_per_fat, sectors_per_track, 2, 0, 0)
    assert raw[64:112] == struct.pack(
        '<12I', 0x3150424b, 1, len(loader), 0x2000, 0x2000,
        checksum(loader), loader_sectors, virtual_spt,
        payload_lba, payload_cylinder, payload_head, payload_sector)
    assert raw[510:512] == b'\x55\xaa'
    assert raw[payload_offset:payload_offset + len(loader)] == loader
    kernel_offset = payload_offset + loader_sectors * 512
    assert raw[kernel_offset:kernel_offset + len(kernel)] == kernel
    assert len(d88) == 0x2b0 + physical_tracks * physical_sectors_per_track * (16 + sector_size)
    for copy in range(2):
        offset = (fat_start + copy * sectors_per_fat) * bytes_per_sector
        assert raw[offset:offset + 3] == bytes((media_id, 0xff, 0xff))

    return raw, d88


def build(directory):
    raw_144, d88_144 = build_profile(directory, 80, 18, 512, 512, 9, 224, 0xf0)
    (directory / 'dos.img').write_bytes(raw_144)
    (directory / 'dos.d88').write_bytes(d88_144)

    raw_125, d88_125 = build_profile(directory, 77, 8, 1024, 1024, 2, 192, 0xfe)
    (directory / 'dos-1.25.img').write_bytes(raw_125)
    (directory / 'dos-1.25.d88').write_bytes(d88_125)
    print('Generated bootable L3DiskEx FAT12 media: 1.25 MB PC-98 and 1.44 MB PC-AT')


if __name__ == '__main__':
    build(pathlib.Path(sys.argv[1]))
