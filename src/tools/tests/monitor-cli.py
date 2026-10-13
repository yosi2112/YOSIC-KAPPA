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
child=subprocess.Popen([str(root/'out/build/yosic-sim.exe'),'--rom',str(root/'out/build/boot/ipl.rom'),'--disk','0',str(root/'out/dos/dos.d88'),'--interactive'],stdin=None,stdout=subprocess.PIPE,stderr=subprocess.PIPE,creationflags=subprocess.CREATE_NEW_PROCESS_GROUP)
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
    send('?\r');expect('BP[n]');expect('\n-')
    send('r r1=1234\r');expect('R1=00001234');expect('\n-')
    send('r r2\r');expect('\n:')
    send('5678\r');expect('-')
    send('r\r');expect('R2=00005678');expect('\n-')
    send('ed 0:4000 DEADBEEF\r');expect('-')
    send('dd 0:4000 L1\r');expect('DEADBEEF');expect('\n-')
    boot_start=time.perf_counter()
    send('t\rg\r')
    startup=expect('0:>',20)
    duration=time.perf_counter()-boot_start
    assert 'YOSIC-DOS 1.0K' in startup,startup
    send('DIR\rCHDIR\r');expect('0:>CHDIR\n0:\\\n0:>')
    assert k.GenerateConsoleCtrlEvent(1,child.pid),ctypes.get_last_error()
    expect('Break\n');expect('PAUSED');expect('\n-')
    send('q\r')
    result=child.wait(timeout=5)
    errors=child.stderr.read().decode()
    assert result==0 and 'stop=monitor-quit' in errors,(result,errors)
    (root/'out/monitor-cli-result.txt').write_text(f'PASS CLI startup, SYMDEB commands, queued T/G, DOS input, Ctrl+Break and Q\nBoot after G: {duration:.3f} seconds\n{errors}')
    print(f'PASS CLI monitor and DOS input; boot after G: {duration:.3f} seconds',flush=True)
finally:
    if child.poll() is None:child.kill();child.wait()
