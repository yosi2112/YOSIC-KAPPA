"""Wrap native KAPPA code in the independent YKE1 executable format."""
import argparse
import pathlib
import struct


def encode(payload, entry=0):
    if not 4 <= len(payload) <= 65528 or len(payload) % 4:
        raise ValueError('Payload must contain 4..65528 bytes, aligned to 4 bytes')
    if entry < 0 or entry >= len(payload) or entry % 4:
        raise ValueError('Entry must be aligned and inside the payload')
    checksum = sum(word[0] for word in struct.iter_unpack('<I', payload)) & 0xffffffff
    return struct.pack('<4s5I', b'YKE1', 1, len(payload), entry, checksum, 0) + payload


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('input', type=pathlib.Path)
    parser.add_argument('output', type=pathlib.Path)
    parser.add_argument('--entry', type=lambda value: int(value, 0), default=0)
    args = parser.parse_args()
    if args.output.suffix.upper() != '.YKE':
        parser.error('Output extension must be .YKE')
    try:
        image = encode(args.input.read_bytes(), args.entry)
    except ValueError as error:
        parser.error(str(error))
    args.output.write_bytes(image)


if __name__ == '__main__':
    main()
