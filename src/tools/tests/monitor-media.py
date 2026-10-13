"""Native-console integration check; synthetic keys target this test's console."""
import ctypes
from ctypes import wintypes
import pathlib
import queue
import subprocess
import threading
import time
root=pathlib.Path(__file__).resolve().parents[3]
k=ctypes.WinDLL('kernel32',use_last_error=True)
class Key(ctypes.Structure):
    _fields_=[('down',wintypes.BOOL),('repeat',wintypes.WORD),('vk',wintypes.WORD),('scan',wintypes.WORD),('char',wintypes.WCHAR),('control',wintypes.DWORD)]
class Event(ctypes.Union):
    _fields_=[('key',Key),('padding',ctypes.c_byte*16)]
class Record(ctypes.Structure):
    _fields_=[('kind',wintypes.WORD),('event',Event)]
k.GetStdHandle.restype=wintypes.HANDLE
k.WriteConsoleInputW.argtypes=[wintypes.HANDLE,ctypes.POINTER(Record),wintypes.DWORD,ctypes.POINTER(wintypes.DWORD)]
k.GenerateConsoleCtrlEvent.argtypes=[wintypes.DWORD,wintypes.DWORD]
stdin=k.GetStdHandle(-10)
def send(text):
    records=(Record*len(text))()
    for row,ch in zip(records,text):
        row.kind=1
        row.event.key=Key(True,1,13 if ch=='\r' else 0,0,ch,0)
    written=wintypes.DWORD()
    assert k.WriteConsoleInputW(stdin,records,len(records),ctypes.byref(written)),ctypes.get_last_error()
    assert written.value==len(records)

import re
import sys
sys.path.insert(0,str(root/'src/dos/tests'))
from support import add_file, base, encode, entry, fatset
old=base()
add_file(old,0,'OLDDATA.DAT',b'old')
fatset(old,3,0xfff)
old[181*512+32:181*512+64]=entry('OLD',3,0,16)
new=base()
add_file(new,0,'NEWDATA.DAT',b'new')
old_path=root/'out/Swap Old.img'
new_path=root/'out/Swap New.img'
old_path.write_bytes(old)
new_path.write_bytes(new)
def check_code(register,value):
    prefix='ADD R5,R0,RZR\n' if register=='R0' else ''
    register='R5' if register=='R0' else register
    return prefix+f'DosConst R0,{value}\nSUB R0,{register},R0\nSRZ R0\nJMP bad\n'
def api(function,pointer,r3=0,r4=0,error=0):
    return (f'DosConst R2,{pointer}\nDosConst R3,{r3}\nDosConst R4,{r4}\nDosCall {function}\n'+check_code('R0',error))
body=api('DOS_OPEN','80000h+oldname')+check_code('R2',1)
body+='halt:\n'
body+=api('DOS_READ',1,'0d0000h',1,6)+api('DOS_CLOSE',1,error=6)
body+=api('DOS_OPEN','80000h+newname')+check_code('R2',1)
body+=api('DOS_READ',1,'0d0000h',1)+check_code('R2',1)
body+='DosConst R0,0d0000h\nLRM R6,[R0]\n'+check_code('R6',ord('n'))
body+=api('DOS_CLOSE',1)+api('DOS_PUTS','80000h+message')
body+='ADI R1,RZR,DOS_EXIT\nADI R2,RZR,61\nJMP 0fff0h\nbad:\nADI R1,RZR,DOS_EXIT\nADI R2,RZR,99\nJMP 0fff0h\n'
for label,text in [('oldname','1:\\OLDDATA.DAT'),('newname','1:\\NEWDATA.DAT'),('message','MEDIA HANDLES OK\r\n')]:
    body+=label+':\nDD '+','.join([*(str(ord(c)) for c in text),'0'])+'\n'
source=root/'out/media-app.asm'
binary=root/'out/media-app.bin'
symbols=root/'out/media-app.map'
source.write_text(f'CPU YOSIC-KAPPA\nINCLUDE "{root / "src/dos/api.inc"}"\n'+'\n'.join('        '+line for line in body.splitlines())+'\nEND\n')
subprocess.run([str(root/'out/build/yosic-as.exe'),str(source),'-o',str(binary),'--map',str(symbols)],check=True,capture_output=True)
halt=int(re.search(r'HALT = (0x[0-9a-fA-F]+)',symbols.read_text())[1],16)
kernel_symbols=(root/'out/dos/dos.map').read_text()
directory=int(re.search(r'CORE_DIRECTORY_COMMAND = (0x[0-9a-fA-F]+)',kernel_symbols)[1],16)
boot=base()
add_file(boot,0,'MEDIA.YKE',encode(binary.read_bytes()))
boot_path=root/'out/Swap Boot.img'
boot_path.write_bytes(boot)
child=subprocess.Popen([str(root/'out/build/yosic-sim.exe'),'--rom',str(root/'out/build/boot/ipl.rom'),'--disk','0',str(boot_path),'--geometry','80','2','18','2','--interactive'],stdin=None,stdout=subprocess.PIPE,stderr=subprocess.PIPE,creationflags=subprocess.CREATE_NEW_PROCESS_GROUP)

chunks=queue.Queue()
def read_output():
    while True:
        data=child.stdout.read(1)
        if not data:break
        chunks.put(data)
threading.Thread(target=read_output,daemon=True).start()
seen=''
def expect(marker,timeout=10):
    global seen
    deadline=time.monotonic()+timeout
    while marker not in seen:
        assert child.poll() is None,(child.returncode,seen,child.stderr.read())
        remaining=deadline-time.monotonic()
        assert remaining>0,(marker,seen)
        try:seen+=chunks.get(timeout=min(remaining,0.2)).decode('ascii').replace('\r','')
        except queue.Empty:pass
    found=seen
    seen=''
    return found
try:
    expect('\n-')
    send(f'AT 1 "{old_path}"\r');expect('Attached FD1:');expect('\n-')
    send('AT\r');expect('FD1:');expect('\n-')
    send('G\r');expect('0:>',20)
    send('DIR 1:\\ /B\r');listing=expect('0:>');assert 'OLDDATA.DAT' in listing,listing
    send('CHDIR 1:\\OLD\rCHDIR 1:\r');expect('1:\\OLD\n0:>')
    assert k.GenerateConsoleCtrlEvent(1,child.pid)
    expect('Break\n');expect('\n-')
    send(f'BP0 2:{directory:X}\rG\r')
    send('DIR 1:\\ /B\r');expect('Break\n');expect('\n-')
    send(f'AT 1 "{new_path}"\r');expect('Error: drive busy or locked');expect('\n-')
    send('BC 0\rG\r');listing=expect('0:>');assert 'OLDDATA.DAT' in listing,listing
    assert k.GenerateConsoleCtrlEvent(1,child.pid)
    expect('Break\n');expect('\n-')
    send(f'AT 1 "{new_path}"\r');expect('Attached FD1:');expect('\n-')
    send('G\r')
    send('DIR 1:\\ /B\rCHDIR 1:\r');listing=expect('1:\\\n0:>')
    assert 'NEWDATA.DAT' in listing and 'OLDDATA.DAT' not in listing,listing
    assert k.GenerateConsoleCtrlEvent(1,child.pid)
    expect('Break\n');expect('\n-')
    send('DT 1\r');expect('Detached FD1');expect('\n-')
    send('G\r')
    send('DIR 1:\\ /B\r');expect('Drive not ready\n0:>')
    assert k.GenerateConsoleCtrlEvent(1,child.pid)
    expect('Break\n');expect('\n-')
    send(f'AT 1 "{old_path}"\rBP0 8:{halt:X}\rG\r')
    send('MEDIA\r');expect('Break\n');expect('\n-')
    send(f'AT 1 "{new_path}"\r');expect('Attached FD1:');expect('\n-')
    send('BC 0\rG\r');expect('MEDIA HANDLES OK\n0:>')
    assert k.GenerateConsoleCtrlEvent(1,child.pid)
    expect('Break\n');expect('\n-')
    send('DD 1:3F00 L1\r');expect('0000003D');expect('\n-')
    send('DT ALL\r');expect('FD3: not attached');expect('\n-')
    send('Q\r')
    result=child.wait(timeout=5)
    errors=child.stderr.read().decode()
    assert result==0 and 'stop=monitor-quit' in errors,(result,errors)
    print('PASS AT/DT on guest: busy locks, changed FAT/current directory, detach/reattach and expired file handles',flush=True)

finally:
    if child.poll() is None:child.kill();child.wait()
