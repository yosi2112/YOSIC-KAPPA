"""Exercise numeric drive selection, prompts and rejection on the guest CPU/FDC."""
from support import DOS, OUT, run


result, _ = run('drive-verification', (DOS / 'tests/drive-input.txt').read_text(),
                disks={drive: OUT / 'dos.d88' for drive in range(1, 4)})
expected = '''KAPPA IPL
IPL: BOOT
YOSIC-DOS 1.0K
0:>DRIVE /?
DRIVE [0|1|2|3]
0:>DRIVE 1
1:>DRIVE
1:
1:>DRIVE 2
2:>DRIVE 3
3:>DRIVE 0
0:>DRIVE 4
Invalid drive specification
0:>DRIVE -1
Invalid drive specification
0:>DRIVE A:
Invalid drive specification
0:>DRIVE 1:
Invalid drive specification
0:>DRIVE 01
Invalid drive specification
0:>DRIVE 0 1
Invalid parameter
0:>DRIVE
0:
0:>'''
assert result.stdout == expected, repr(result.stdout)
result, _ = run('drive-unmounted', 'DRIVE 1\nDRIVE\n')
assert 'Drive not ready\n0:>DRIVE\n0:\n0:>' in result.stdout, result.stdout
print('PASS DRIVE 0..3, prompts, help, invalid arguments, preserved stack/registers')
