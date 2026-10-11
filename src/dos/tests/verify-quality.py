"""Check public DOS source syntax, includes, whitespace and documentation links."""
import ast
import re

from support import DOS, ROOT


files = list(DOS.rglob('*')) + list((ROOT / 'doc/dos').rglob('*'))
count = 0
for path in files:
    if not path.is_file():
        continue
    assert path.suffix not in ('.bin', '.pyc', '.exe', '.img', '.d88'), path
    source = path.read_text(encoding='utf-8-sig')
    assert source.endswith('\n'), path
    assert not re.search(r'[ \t]+$', source, re.MULTILINE), path
    if path.suffix == '.py':
        compile(ast.parse(source, filename=str(path)), str(path), 'exec')
    if path.suffix in ('.inc', '.asm'):
        for name in re.findall(r'^\s*INCLUDE\s+([^\s;]+)', source, re.MULTILINE):
            assert name == 'kernel.inc' or (path.parent / name).exists(), (path, name)
    if path.suffix == '.md':
        for link in re.findall(r'\]\(([^)]+)\)', source):
            target = link.split('#')[0]
            if target and '://' not in target:
                assert (path.parent / target).exists(), (path, target)
        assert not re.search(r'cmake|build\.ps1|make_yke\.py\s+\S+\s+\S+', source), path
    count += 1
assert 'YKE1' in (DOS / 'exec.inc').read_text()
assert '.YKE' in (ROOT / 'doc/dos/commands.md').read_text(encoding='utf-8')
print(f'PASS source syntax/includes, whitespace, YKE naming, {count} files, documentation links')
