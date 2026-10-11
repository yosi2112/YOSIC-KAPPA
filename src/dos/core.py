"""Eight-command DOS. Integer Python is compiled to native guest instructions."""
CONSOLE = 0xFE0008
DRIVE = 0x13FF0
FATDRIVE = 0x13FF4
DIRLBA = 0x13FF8
SELECT = 0x13FE0
ERROR = 0x13FE4
PARENT = 0x13FC0
ENTRY = 0x13FC4
TARGET = 0x13FC8
ISROOT = 0x13FCC
STATES = 0x50000
SECTOR = 0x30000
FAT = 0x34000
DIRECTORY = 0x40000
WORK = 0x42000
PATHBUF = 0x14000
NAMEBUF = 0x14800
ARGBUF = 0x14C00
PATTERN = 0x15000
RENPATTERN = 0x15040
TOKENS = 0x15400
SEARCHDIR = 0x13FB0
SRCMETA = 0x56000
SRCNAME = 0x56200
DSTNAME = 0x56400
DSTCHAIN = 0x60000
SRCCHAIN = 0x64000
OLDCHAIN = 0x68000
VISITED = 0x70000
VERIFYBUF = 0x44000
COPYBUF = 0x46000
MATCHLIST = 0x74000
SOURCES = 0x15800
MODES = 0x15880
LASTDST = 0x15C00
MOVEMETA = 0xC0000
MOVESRC = 0xC0200
SYSBPB = 0xC1000
SYSBOOT = 0xC2000
SYSFAT = 0xC3000

def putc(c):
    write(CONSOLE, c)
    return 0


def puts(p):
    while read(p) != 0:
        putc(read(p))
        p += 4
    return 0


def newline():
    putc(13)
    putc(10)
    return 0


def decimal(n):
    if n >= 10:
        decimal(n // 10)
    putc(48 + n % 10)
    return 0


def upper(c):
    if c >= 97 and c <= 122:
        return c - 32
    return c


def le16(p):
    return read(p) + (read(p + 4) << 8)


def le32(p):
    return le16(p) + (le16(p + 8) << 16)


def set16(p, n):
    write(p, n & 255)
    write(p + 4, (n >> 8) & 255)
    return 0


def clear(p, count):
    while count != 0:
        write(p, 0)
        p += 4
        count -= 1
    return 0


def copytext(dst, src):
    while read(src) != 0:
        write(dst, read(src))
        dst += 4
        src += 4
    write(dst, 0)
    return 0


def memcopy(dst, src, count):
    while count != 0:
        write(dst, read(src))
        dst += 4
        src += 4
        count -= 1
    return 0


def set32(p, n):
    set16(p, n & 65535)
    set16(p + 8, n >> 16)
    return 0


def equal(a, b):
    while read(a) == read(b):
        if read(a) == 0:
            return 1
        a += 4
        b += 4
    return 0


def init():
    drive = 0
    while drive < 4:
        state = STATES + drive * 512
        write(state, drive)
        write(state + 4, 0)
        write(state + 36, 0)
        write(state + 64, 92)
        write(state + 68, 0)
        drive += 1
    write(DRIVE, 0)
    write(FATDRIVE, -1)
    write(DIRLBA, -1)
    return 0


def select(drive):
    if drive < 0 or drive >= 4:
        return 15
    state = STATES + drive * 512
    write(SELECT, state)
    if read(state + 4) != 0 and read(FATDRIVE) == drive:
        return 0
    status = block(0, SECTOR, drive, 0)
    if status != 0:
        return status
    start = 0
    if read(SECTOR) == 75 and read(SECTOR + 4) == 66 and read(SECTOR + 8) == 80 and read(SECTOR + 12) == 49:
        start = 162
        status = block(start, SECTOR, drive, 0)
        if status != 0:
            return status
    if le16(SECTOR + 44) != 512 or read(SECTOR + 52) != 1 or read(SECTOR + 64) != 2:
        return 13
    reserved = le16(SECTOR + 56)
    entries = le16(SECTOR + 68)
    total = le16(SECTOR + 76)
    fatsize = le16(SECTOR + 88)
    if reserved < 1 or entries < 16 or entries > 512 or entries % 16 != 0:
        return 13
    if fatsize < 1 or fatsize > 9 or total + start > 2880 or total < 64:
        return 13
    if read(SECTOR + 2040) != 85 or read(SECTOR + 2044) != 170:
        return 13
    fatstart = start + reserved
    root = fatstart + fatsize * 2
    data = root + entries // 16
    clusters = total + start - data
    if clusters < 1 or clusters >= 4085 or (clusters + 2) * 3 > fatsize * 1024:
        return 13
    write(state + 8, start)
    write(state + 12, fatstart)
    write(state + 16, fatsize)
    write(state + 20, root)
    write(state + 24, entries)
    write(state + 28, data)
    write(state + 32, clusters)
    i = 0
    while i < fatsize:
        status = block(fatstart + i, FAT + i * 2048, drive, 0)
        if status != 0:
            return status
        i += 1
    if read(FAT) != read(SECTOR + 84) or read(FAT + 4) != 255 or read(FAT + 8) != 255:
        return 13
    write(state + 4, 1)
    write(FATDRIVE, drive)
    write(DIRLBA, -1)
    return 0


def fatget(cluster):
    state = read(SELECT)
    if cluster < 2 or cluster >= read(state + 32) + 2:
        write(ERROR, 13)
        return 0
    p = FAT + (cluster + cluster // 2) * 4
    value = read(p) + (read(p + 4) << 8)
    if cluster & 1:
        return value >> 4
    return value & 4095


def fatset(cluster, value):
    p = FAT + (cluster + cluster // 2) * 4
    if cluster & 1:
        write(p, (read(p) & 15) | ((value << 4) & 240))
        write(p + 4, (value >> 4) & 255)
    else:
        write(p, value & 255)
        write(p + 4, (read(p + 4) & 240) | ((value >> 8) & 15))
    return 0


def fatflush():
    state = read(SELECT)
    copy = 0
    while copy < 2:
        i = 0
        while i < read(state + 16):
            status = block(read(state + 12) + copy * read(state + 16) + i, FAT + i * 2048, read(state), 1)
            if status != 0:
                write(FATDRIVE, -1)
                return status
            i += 1
        copy += 1
    return 0


def allocate():
    cluster = 2
    state = read(SELECT)
    while cluster < read(state + 32) + 2:
        if fatget(cluster) == 0:
            fatset(cluster, 4095)
            return cluster
        cluster += 1
    write(ERROR, 112)
    return 0


def read_dir_sector(lba):
    if read(DIRLBA) != lba:
        status = block(lba, DIRECTORY, read(read(SELECT)), 0)
        if status != 0:
            write(ERROR, status)
            return status
        write(DIRLBA, lba)
    return 0


def dir_entry(directory, index):
    state = read(SELECT)
    if directory == 0:
        if index >= read(state + 24):
            return 0
        lba = read(state + 20) + index // 16
    else:
        budget = read(state + 32)
        while index >= 16:
            directory = fatget(directory)
            if directory >= 4088:
                return 0
            if directory < 2 or budget == 0:
                write(ERROR, 13)
                return 0
            budget -= 1
            index -= 16
        if directory >= read(state + 32) + 2:
            write(ERROR, 13)
            return 0
        lba = read(state + 28) + directory - 2
    if read_dir_sector(lba) != 0:
        return 0
    return DIRECTORY + (index % 16) * 128


def name_equal(entry):
    i = 0
    while i < 11:
        if read(entry + i * 4) != read(NAMEBUF + i * 4):
            return 0
        i += 1
    return 1


def find(directory, free):
    index = 0
    while index < 65536:
        entry = dir_entry(directory, index)
        if entry == 0:
            return 0
        first = read(entry)
        if free:
            if first == 0 or first == 229:
                return entry
        else:
            if first == 0:
                return 0
            if first != 229 and (read(entry + 44) & 8) == 0 and name_equal(entry):
                return entry
        index += 1
    write(ERROR, 13)
    return 0


def path(text):
    drive = read(DRIVE)
    if read(text + 4) == 58:
        drive = upper(read(text)) - 65
        text += 8
    status = select(drive)
    if status != 0:
        return status
    state = read(SELECT)
    size = 1
    write(PATHBUF, 92)
    if read(text) != 92:
        p = state + 68
        while read(p) != 0:
            write(PATHBUF + size * 4, read(p))
            p += 4
            size += 1
    while read(text) != 0 and read(text) != 32 and read(text) != 9:
        if read(text) == 92:
            text += 4
            continue
        start = text
        length = 0
        while read(text) != 0 and read(text) != 92 and read(text) != 32 and read(text) != 9:
            text += 4
            length += 1
        if length == 1 and read(start) == 46:
            continue
        if length == 2 and read(start) == 46 and read(start + 4) == 46:
            while size > 1 and read(PATHBUF + (size - 1) * 4) != 92:
                size -= 1
            if size > 1:
                size -= 1
            continue
        if size > 1:
            write(PATHBUF + size * 4, 92)
            size += 1
        if size + length >= 64:
            return 206
        i = 0
        while i < length:
            write(PATHBUF + size * 4, upper(read(start + i * 4)))
            size += 1
            i += 1
    write(PATHBUF + size * 4, 0)
    return 0


def make_name(start, length):
    i = 0
    while i < 11:
        write(NAMEBUF + i * 4, 32)
        i += 1
    offset = 0
    extension = 0
    i = 0
    while i < length:
        c = upper(read(start + i * 4))
        if c == 46:
            if extension or offset == 0:
                return 3
            extension = 1
            offset = 8
        else:
            if c < 33 or c == 34 or c == 42 or c == 43 or c == 44 or c == 47 or c == 58 or c == 59 or c == 60 or c == 61 or c == 62 or c == 63 or c == 91 or c == 93 or c == 124:
                return 3
            if (extension and offset >= 11) or (not extension and offset >= 8):
                return 3
            write(NAMEBUF + offset * 4, c)
            offset += 1
        i += 1
    return 0


def resolve(text, missing):
    status = path(text)
    if status != 0:
        return status
    return walk(missing)


def walk(missing):
    write(ISROOT, 0)
    write(ENTRY, 0)
    write(PARENT, 0)
    write(TARGET, 0)
    p = PATHBUF + 4
    directory = 0
    if read(p) == 0:
        write(ISROOT, 1)
        return 0
    while read(p) != 0:
        start = p
        length = 0
        while read(p) != 0 and read(p) != 92:
            length += 1
            p += 4
        status = make_name(start, length)
        if status != 0:
            return status
        entry = find(directory, 0)
        last = read(p) == 0
        if entry == 0:
            if read(ERROR) != 0:
                return read(ERROR)
            if missing and last:
                write(PARENT, directory)
                return 0
            return 3
        if last:
            write(ENTRY, entry)
            write(PARENT, directory)
            write(TARGET, le16(entry + 104))
            return 0
        if (read(entry + 44) & 16) == 0:
            return 3
        directory = le16(entry + 104)
        p += 4
    return 3


def print_name(entry):
    i = 0
    while i < 8 and read(entry + i * 4) != 32:
        putc(read(entry + i * 4))
        i += 1
    if read(entry + 32) != 32:
        putc(46)
        i = 8
        while i < 11 and read(entry + i * 4) != 32:
            putc(read(entry + i * 4))
            i += 1
    return 0


def directory_command(args):
    bare = 0
    count = tokens(args)
    if count < 0:
        return 87
    text = "."
    haspath = 0
    i = 0
    while i < count:
        token = read(TOKENS + i * 4)
        if equal(token, "/B"):
            bare = 1
        elif read(token) == 47 or haspath:
            return 87
        else:
            text = token
            haspath = 1
        i += 1
    status = search(text)
    if status != 0:
        return status
    directory = read(SEARCHDIR)
    if not bare:
        puts(" Directory of ")
        putc(65 + read(read(SELECT)))
        putc(58)
        puts(PATHBUF)
        newline()
    index = 0
    while index < 65536:
        entry = dir_entry(directory, index)
        if entry == 0 or read(entry) == 0:
            return read(ERROR)
        if read(entry) != 229 and (read(entry + 44) & 14) == 0 and matches(entry, PATTERN):
            print_name(entry)
            if not bare:
                if read(entry + 44) & 16:
                    puts("  <DIR>")
                else:
                    puts("  ")
                    decimal(le32(entry + 112))
            newline()
        index += 1
    return 13


def mkdir_command(args):
    if tokens(args) != 1:
        return 87
    args = read(TOKENS)
    status = resolve(args, 1)
    if status != 0:
        return status
    if read(ISROOT) or read(ENTRY) != 0:
        return 5
    parent = read(PARENT)
    entry = free_entry(parent)
    if entry == 0:
        if read(ERROR) != 0:
            return read(ERROR)
        return 112
    plba = read(DIRLBA)
    poffset = entry - DIRECTORY
    cluster = allocate()
    if cluster == 0:
        return read(ERROR)
    state = read(SELECT)
    clear(WORK, 512)
    i = 0
    while i < 11:
        write(WORK + i * 4, 32)
        write(WORK + 128 + i * 4, 32)
        i += 1
    write(WORK, 46)
    write(WORK + 44, 16)
    set16(WORK + 104, cluster)
    write(WORK + 128, 46)
    write(WORK + 132, 46)
    write(WORK + 172, 16)
    set16(WORK + 232, parent)
    status = block(read(state + 28) + cluster - 2, WORK, read(state), 1)
    if status != 0:
        write(FATDRIVE, -1)
        return status
    status = fatflush()
    if status != 0:
        return status
    if read_dir_sector(plba) != 0:
        return read(ERROR)
    entry = DIRECTORY + poffset
    clear(entry, 32)
    i = 0
    while i < 11:
        write(entry + i * 4, read(NAMEBUF + i * 4))
        i += 1
    write(entry + 44, 16)
    set16(entry + 104, cluster)
    status = block(plba, DIRECTORY, read(state), 1)
    if status != 0:
        fatset(cluster, 0)
        fatflush()
    return status


def rmdir_command(args):
    if tokens(args) != 1:
        return 87
    args = read(TOKENS)
    status = resolve(args, 0)
    if status != 0:
        return status
    entry = read(ENTRY)
    if read(ISROOT) or (read(entry + 44) & 16) == 0:
        return 5
    state = read(SELECT)
    if equal(PATHBUF, state + 64):
        return 16
    plba = read(DIRLBA)
    poffset = entry - DIRECTORY
    cluster = read(TARGET)
    following = cluster
    budget = read(state + 32)
    while following < 4088:
        if following < 2 or following >= read(state + 32) + 2 or budget == 0:
            return 13
        following = fatget(following)
        budget -= 1
    index = 2
    while index < 65536:
        current = dir_entry(cluster, index)
        if current == 0 or read(current) == 0:
            break
        if read(current) != 229:
            return 5
        index += 1
    if read(ERROR) != 0:
        return read(ERROR)
    if read_dir_sector(plba) != 0:
        return read(ERROR)
    write(DIRECTORY + poffset, 229)
    status = block(plba, DIRECTORY, read(state), 1)
    if status != 0:
        write(DIRLBA, -1)
        return status
    budget = read(state + 32)
    while cluster >= 2 and cluster < 4088:
        if budget == 0:
            return 13
        following = fatget(cluster)
        fatset(cluster, 0)
        cluster = following
        budget -= 1
    return fatflush()


def prompt_drive():
    putc(48 + read(DRIVE))
    return 0


def error_message(status):
    if status == 3 or status == 2:
        puts("File or path not found")
    elif status == 5:
        puts("Access denied")
    elif status == 15:
        puts("Invalid drive specification")
    elif status == 16:
        puts("Cannot remove current directory")
    elif status == 19:
        puts("Write protect error")
    elif status == 21:
        puts("Drive not ready")
    elif status == 112:
        puts("Insufficient disk space")
    elif status == 87:
        puts("Invalid parameter")
    elif status == 183:
        puts("Duplicate file name or file not found")
    elif status == 82:
        puts("File cannot be copied onto itself")
    elif status == 206:
        puts("Path too long")
    else:
        puts("Invalid disk or data error")
    newline()
    return 0


def fs_command(command, args):
    write(ERROR, 0)
    while read(args) == 32 or read(args) == 9:
        args += 4
    if equal(args, "/?"):
        if command == 1:
            puts("DIR [path] [/B]")
        elif command == 2:
            puts("MKDIR path")
        elif command == 3:
            puts("RMDIR path")
        elif command == 4:
            puts("DRIVE [0|1|2|3]")
        elif command == 5:
            puts("COPY source destination [/Y]")
        elif command == 6:
            puts("MOVE source destination")
        elif command == 7:
            puts("DELETE path [/P]")
        else:
            puts("SYS B:|C:|D:")
        newline()
        return 0
    status = 87
    if command == 1:
        status = directory_command(args)
    elif command == 2:
        status = mkdir_command(args)
    elif command == 3:
        status = rmdir_command(args)
    elif command == 4:
        status = drive_command(args)
    elif command == 5:
        status = copy_command(args)
    elif command == 6:
        status = move_command(args)
    elif command == 7:
        status = delete_command(args)
    elif command == 8:
        status = sys_command(args)
    if status != 0:
        error_message(status)
        write(FATDRIVE, -1)
        write(DIRLBA, -1)
    return status


def tokens(args):
    count = 0
    dst = ARGBUF
    while read(args) != 0:
        while read(args) == 32 or read(args) == 9:
            args += 4
        if read(args) == 0:
            break
        if count >= 32:
            return -1
        write(TOKENS + count * 4, dst)
        count += 1
        if read(args) == 43:
            write(dst, 43)
            write(dst + 4, 0)
            dst += 8
            args += 4
            continue
        quoted = 0
        while read(args) != 0:
            c = read(args)
            if c == 34:
                quoted = 1 - quoted
            elif (c == 32 or c == 9 or c == 43) and not quoted:
                break
            else:
                write(dst, upper(c))
                dst += 4
            args += 4
        if quoted:
            return -1
        write(dst, 0)
        dst += 4
    return count


def make_pattern(start, length):
    clear(NAMEBUF, 11)
    i = 0
    while i < 11:
        write(NAMEBUF + i * 4, 32)
        i += 1
    offset = 0
    extension = 0
    starred = 0
    i = 0
    while i < length:
        c = upper(read(start + i * 4))
        if c == 46:
            if extension or offset == 0:
                return 87
            extension = 1
            offset = 8
            starred = 0
        elif c == 42:
            limit = 8
            if extension:
                limit = 11
            while offset < limit:
                write(NAMEBUF + offset * 4, 63)
                offset += 1
            starred = 1
        elif not starred:
            if c < 33 or c == 34 or c == 43 or c == 44 or c == 47 or c == 58 or c == 59 or c == 60 or c == 61 or c == 62 or c == 91 or c == 93 or c == 124:
                return 87
            if (extension and offset >= 11) or (not extension and offset >= 8):
                return 87
            write(NAMEBUF + offset * 4, c)
            offset += 1
        i += 1
    if starred and not extension:
        while offset < 11:
            write(NAMEBUF + offset * 4, 63)
            offset += 1
    return 0


def matches(entry, pattern):
    i = 0
    while i < 11:
        c = read(pattern + i * 4)
        if c != 63 and c != read(entry + i * 4):
            return 0
        i += 1
    return 1


def search(text):
    status = resolve(text, 0)
    if status == 0 and (read(ISROOT) or (read(read(ENTRY) + 44) & 16)):
        write(SEARCHDIR, read(TARGET))
        i = 0
        while i < 11:
            write(PATTERN + i * 4, 63)
            i += 1
        return 0
    status = path(text)
    if status != 0:
        return status
    p = PATHBUF + 4
    last = p
    length = 0
    while read(p) != 0:
        if read(p) == 92:
            last = p + 4
            length = 0
        else:
            length += 1
        p += 4
    status = make_pattern(last, length)
    if status != 0:
        return status
    memcopy(PATTERN, NAMEBUF, 11)
    if last == PATHBUF + 4:
        write(last, 0)
    else:
        write(last - 4, 0)
    status = walk(0)
    if status != 0:
        return status
    if not read(ISROOT) and (read(read(ENTRY) + 44) & 16) == 0:
        return 3
    write(SEARCHDIR, read(TARGET))
    return 0


def free_entry(directory):
    entry = find(directory, 1)
    if entry != 0 or read(ERROR) != 0:
        return entry
    if directory == 0:
        write(ERROR, 112)
        return 0
    state = read(SELECT)
    tail = directory
    budget = read(state + 32)
    while fatget(tail) < 4088:
        tail = fatget(tail)
        budget -= 1
        if tail < 2 or tail >= read(state + 32) + 2 or budget == 0:
            write(ERROR, 13)
            return 0
    cluster = allocate()
    if cluster == 0:
        return 0
    clear(WORK, 512)
    lba = read(state + 28) + cluster - 2
    status = block(lba, WORK, read(state), 1)
    if status != 0:
        fatset(cluster, 0)
        write(ERROR, status)
        return 0
    fatset(tail, cluster)
    status = fatflush()
    if status != 0:
        write(ERROR, status)
        return 0
    if read_dir_sector(lba) != 0:
        return 0
    return DIRECTORY


def chain_load(cluster, size, destination):
    state = read(SELECT)
    limit = read(state + 32)
    if size < 0 or size > limit * 512:
        write(ERROR, 13)
        return -1
    if cluster == 0:
        if size == 0:
            return 0
        write(ERROR, 13)
        return -1
    if cluster < 2 or cluster >= limit + 2:
        write(ERROR, 13)
        return -1
    clear(VISITED, limit + 2)
    count = 0
    while cluster < 4088:
        if cluster < 2 or cluster >= limit + 2 or count >= limit or read(VISITED + cluster * 4):
            write(ERROR, 13)
            return -1
        write(VISITED + cluster * 4, 1)
        write(destination + count * 4, cluster)
        count += 1
        cluster = fatget(cluster)
    if count * 512 < size:
        write(ERROR, 13)
        return -1
    return count


def free_chain(array, count):
    i = 0
    while i < count:
        fatset(read(array + i * 4), 0)
        i += 1
    return 0


def unlink_file(entry):
    if read(entry + 44) & 17:
        return 5
    lba = read(DIRLBA)
    offset = entry - DIRECTORY
    count = chain_load(le16(entry + 104), le32(entry + 112), OLDCHAIN)
    if count < 0:
        return read(ERROR)
    write(DIRECTORY + offset, 229)
    status = block(lba, DIRECTORY, read(read(SELECT)), 1)
    if status != 0:
        write(DIRLBA, -1)
        return status
    free_chain(OLDCHAIN, count)
    return fatflush()


def delete_command(args):
    count = tokens(args)
    source = 0
    prompt = 0
    i = 0
    while i < count:
        token = read(TOKENS + i * 4)
        if equal(token, "/P"):
            prompt = 1
        elif source == 0 and read(token) != 47:
            source = token
        else:
            return 87
        i += 1
    if count < 1 or source == 0:
        return 87
    status = search(source)
    if status != 0:
        return status
    directory = read(SEARCHDIR)
    allfiles = 1
    i = 0
    while i < 11:
        if read(PATTERN + i * 4) != 63:
            allfiles = 0
        i += 1
    if allfiles and not prompt:
        puts("All files in directory will be deleted! Are you sure (Y/N)? ")
        if answer() != 1:
            return 0
    index = 0
    found = 0
    while index < 65536:
        entry = dir_entry(directory, index)
        if entry == 0 or read(entry) == 0:
            break
        if read(entry) != 229 and (read(entry + 44) & 30) == 0 and matches(entry, PATTERN):
            found = 1
            yes = 1
            if prompt:
                print_name(entry)
                puts(", Delete (Y/N)? ")
                yes = answer()
            if yes < 0:
                return 0
            if yes:
                status = unlink_file(entry)
                if status != 0:
                    return status
        index += 1
    if read(ERROR) != 0:
        return read(ERROR)
    if not found:
        return 2
    return 0


def mapped_name(entry, pattern):
    i = 0
    while i < 11:
        c = read(pattern + i * 4)
        if c == 63:
            c = read(entry + i * 4)
        write(NAMEBUF + i * 4, c)
        i += 1
    if read(NAMEBUF) == 32 or read(NAMEBUF) == 46:
        return 87
    return 0


def copy_one(srcdrive, directory, index, destination, prompt):
    status = select(srcdrive)
    if status != 0:
        return status
    entry = dir_entry(directory, index)
    if entry == 0:
        return 13
    memcopy(SRCMETA, entry, 32)
    memcopy(SRCNAME, entry, 11)
    srclba = read(DIRLBA)
    srcoffset = entry - DIRECTORY
    size = le32(SRCMETA + 112)
    if chain_load(le16(SRCMETA + 104), size, SRCCHAIN) < 0:
        return read(ERROR)
    srcstate = read(SELECT)
    status = destination_path(destination)
    if status != 0:
        return status
    entry = read(ENTRY)
    parent = read(PARENT)
    if read(ISROOT) or (entry != 0 and (read(entry + 44) & 16)):
        parent = read(TARGET)
        memcopy(NAMEBUF, SRCNAME, 11)
        entry = find(parent, 0)
    if read(ERROR) != 0:
        return read(ERROR)
    dststate = read(SELECT)
    dstdrive = read(dststate)
    oldcount = 0
    if entry != 0:
        if srcdrive == dstdrive and srclba == read(DIRLBA) and srcoffset == entry - DIRECTORY:
            return 82
        if read(entry + 44) & 17:
            return 5
        if prompt:
            puts("Overwrite ")
            print_name(entry)
            puts(" (Yes/No)? ")
            if answer() != 1:
                return -1
        oldcount = chain_load(le16(entry + 104), le32(entry + 112), OLDCHAIN)
        if oldcount < 0:
            return read(ERROR)
    else:
        entry = free_entry(parent)
        if entry == 0:
            return read(ERROR)
    memcopy(DSTNAME, NAMEBUF, 11)
    dlba = read(DIRLBA)
    doffset = entry - DIRECTORY
    count = (size + 511) // 512
    i = 0
    while i < count:
        cluster = allocate()
        if cluster == 0:
            free_chain(DSTCHAIN, i)
            return read(ERROR)
        write(DSTCHAIN + i * 4, cluster)
        if i > 0:
            fatset(read(DSTCHAIN + (i - 1) * 4), cluster)
        i += 1
    status = fatflush()
    if status != 0:
        return status
    i = 0
    while i < count:
        status = block(read(srcstate + 28) + read(SRCCHAIN + i * 4) - 2, COPYBUF, srcdrive, 0)
        if status == 0:
            status = block(read(dststate + 28) + read(DSTCHAIN + i * 4) - 2, COPYBUF, dstdrive, 1)
        if status != 0:
            failure = status
            if select(dstdrive) == 0:
                free_chain(DSTCHAIN, count)
                fatflush()
            return failure
        i += 1
    if read_dir_sector(dlba) != 0:
        return read(ERROR)
    entry = DIRECTORY + doffset
    memcopy(entry, SRCMETA, 32)
    memcopy(entry, DSTNAME, 11)
    write(entry + 44, 32)
    cluster = 0
    if count > 0:
        cluster = read(DSTCHAIN)
    set16(entry + 104, cluster)
    set32(entry + 112, size)
    status = block(dlba, DIRECTORY, dstdrive, 1)
    if status != 0:
        free_chain(DSTCHAIN, count)
        fatflush()
        write(DIRLBA, -1)
        return status
    free_chain(OLDCHAIN, oldcount)
    return fatflush()


def destination_path(text):
    status = path(text)
    if status != 0:
        return status
    p = PATHBUF + 4
    last = p
    length = 0
    wildcard = 0
    while read(p) != 0:
        c = read(p)
        if c == 92:
            last = p + 4
            length = 0
            wildcard = 0
        else:
            length += 1
            if c == 42 or c == 63:
                wildcard = 1
        p += 4
    if wildcard:
        status = make_pattern(last, length)
        if status != 0:
            return status
        memcopy(DSTNAME, NAMEBUF, 11)
        status = mapped_name(SRCNAME, DSTNAME)
        if status != 0:
            return status
        p = last
        i = 0
        while i < 11:
            if i == 8 and read(NAMEBUF + 32) != 32:
                write(p, 46)
                p += 4
            c = read(NAMEBUF + i * 4)
            if c != 32:
                write(p, c)
                p += 4
            i += 1
        write(p, 0)
    return walk(1)


def copy_command(args):
    count = tokens(args)
    if count < 2 or count > 3:
        return 87
    prompt = 1
    if count == 3:
        if not equal(read(TOKENS + 8), "/Y"):
            return 87
        prompt = 0
    source = read(TOKENS)
    destination = read(TOKENS + 4)
    status = search(source)
    if status != 0:
        return status
    directory = read(SEARCHDIR)
    drive = read(read(SELECT))
    matched = 0
    index = 0
    while index < 65536:
        entry = dir_entry(directory, index)
        if entry == 0 or read(entry) == 0:
            break
        if read(entry) != 229 and (read(entry + 44) & 30) == 0 and matches(entry, PATTERN):
            write(MATCHLIST + matched * 4, index)
            matched += 1
        index += 1
    if read(ERROR) != 0:
        return read(ERROR)
    if matched == 0:
        return 2
    if matched > 1:
        status = resolve(destination, 0)
        if status != 0 or (not read(ISROOT) and (read(read(ENTRY) + 44) & 16) == 0):
            return 87
    copied = 0
    i = 0
    while i < matched:
        status = copy_one(drive, directory, read(MATCHLIST + i * 4), destination, prompt)
        if status > 0:
            return status
        if status == 0:
            copied += 1
        i += 1
    puts("        ")
    decimal(copied)
    puts(" file(s) copied")
    newline()
    return 0


def drive_value(p):
    drive = upper(read(p)) - 65
    if drive < 0 or drive >= 4:
        return -1
    if read(p + 4) == 0 or (read(p + 4) == 58 and read(p + 8) == 0):
        return drive
    return -1


def drive_command(args):
    count = tokens(args)
    if count == 0:
        prompt_drive()
        putc(58)
        newline()
        return 0
    if count != 1:
        return 87
    p = read(TOKENS)
    drive = read(p) - 48
    if drive < 0 or drive >= 4 or read(p + 4) != 0:
        return 15
    status = select(drive)
    if status == 0:
        write(DRIVE, drive)
    return status


def move_command(args):
    if tokens(args) != 2:
        return 87
    source = read(TOKENS)
    destination = read(TOKENS + 4)
    status = resolve(source, 0)
    if status != 0:
        return status
    if read(ISROOT):
        return 5
    entry = read(ENTRY)
    if read(entry + 44) & 1:
        return 5
    memcopy(MOVEMETA, entry, 32)
    srcdrive = read(read(SELECT))
    parent = read(PARENT)
    cluster = read(TARGET)
    slba = read(DIRLBA)
    soffset = entry - DIRECTORY
    write(MOVESRC, 65 + srcdrive)
    write(MOVESRC + 4, 58)
    copytext(MOVESRC + 8, PATHBUF)
    status = resolve(destination, 1)
    if status != 0:
        return status
    entry = read(ENTRY)
    dstparent = read(PARENT)
    if read(ISROOT) or (entry != 0 and (read(entry + 44) & 16)):
        dstparent = read(TARGET)
        memcopy(NAMEBUF, MOVEMETA, 11)
        entry = find(dstparent, 0)
    if read(ERROR) != 0:
        return read(ERROR)
    dstdrive = read(read(SELECT))
    if entry != 0:
        if srcdrive == dstdrive and read(DIRLBA) == slba and entry - DIRECTORY == soffset:
            return 0
        return 183
    memcopy(DSTNAME, NAMEBUF, 11)
    if srcdrive != dstdrive:
        if read(MOVEMETA + 44) & 16:
            return 5
        status = select(srcdrive)
        if status != 0:
            return status
        index = 0
        while index < 65536:
            entry = dir_entry(parent, index)
            if entry == 0 or read(entry) == 0:
                return 2
            if read(DIRLBA) == slba and entry - DIRECTORY == soffset:
                break
            index += 1
        status = copy_one(srcdrive, parent, index, destination, 0)
        if status != 0:
            return status
        status = resolve(MOVESRC, 0)
        if status != 0:
            return status
        return unlink_file(read(ENTRY))
    if read(MOVEMETA + 44) & 16:
        ancestor = dstparent
        budget = read(read(SELECT) + 32)
        while ancestor != 0:
            if ancestor == cluster:
                return 5
            if budget == 0:
                return 13
            entry = dir_entry(ancestor, 1)
            if entry == 0 or read(entry) != 46 or read(entry + 4) != 46:
                return 13
            ancestor = le16(entry + 104)
            budget -= 1
    if parent == dstparent:
        status = read_dir_sector(slba)
        if status != 0:
            return status
        memcopy(DIRECTORY + soffset, DSTNAME, 11)
        return block(slba, DIRECTORY, srcdrive, 1)
    entry = free_entry(dstparent)
    if entry == 0:
        return read(ERROR)
    dlba = read(DIRLBA)
    doffset = entry - DIRECTORY
    memcopy(entry, MOVEMETA, 32)
    memcopy(entry, DSTNAME, 11)
    status = block(dlba, DIRECTORY, srcdrive, 1)
    if status != 0:
        return status
    if read(MOVEMETA + 44) & 16:
        entry = dir_entry(cluster, 1)
        if entry == 0:
            return read(ERROR)
        set16(entry + 104, dstparent)
        status = block(read(DIRLBA), DIRECTORY, srcdrive, 1)
        if status != 0:
            return status
    status = read_dir_sector(slba)
    if status != 0:
        return status
    write(DIRECTORY + soffset, 229)
    return block(slba, DIRECTORY, srcdrive, 1)


def sysfat(cluster):
    p = SYSFAT + (cluster + cluster // 2) * 4
    value = read(p) + read(p + 4) * 256
    if cluster & 1:
        return value >> 4
    return value & 4095


def sys_command(args):
    if tokens(args) != 1:
        return 87
    target = drive_value(read(TOKENS))
    if target < 0:
        return 15
    if target == 0:
        return 5
    status = select(target)
    if status != 0:
        return status
    state = read(SELECT)
    start = read(state + 8)
    if start != 0 and start != 162:
        return 13
    status = block(0, SYSBOOT, 0, 0)
    if status != 0:
        return status
    if le32(SYSBOOT) != 0x3150424B or le32(SYSBOOT + 16) != 1:
        return 13
    status = block(start, SYSBPB, target, 0)
    if status != 0:
        return status
    if start == 0:
        limit = read(state + 32) - 162
        if limit < 1:
            return 112
        memcopy(SYSFAT, FAT, read(state + 16) * 512)
        cluster = limit + 2
        while cluster < read(state + 32) + 2:
            if sysfat(cluster) != 0:
                return 112
            cluster += 1
        cluster = limit + 1
        while cluster >= 2:
            if sysfat(cluster) != 0:
                sector = read(state + 28) + cluster - 2
                status = block(sector, WORK, target, 0)
                if status == 0:
                    status = block(sector + 162, WORK, target, 1)
                if status != 0:
                    return status
            cluster -= 1
        sector = read(state + 20) + read(state + 24) // 16 - 1
        while sector >= read(state + 12):
            status = block(sector, WORK, target, 0)
            if status == 0:
                status = block(sector + 162, WORK, target, 1)
            if status != 0:
                return status
            sector -= 1
        set16(SYSBPB + 76, le16(SYSBPB + 76) - 162)
        set32(SYSBPB + 112, 162)
        status = block(162, SYSBPB, target, 1)
        if status != 0:
            return status
    sector = 1
    while sector < 162:
        status = block(sector, WORK, 0, 0)
        if status == 0:
            status = block(sector, WORK, target, 1)
        if status != 0:
            return status
        sector += 1
    status = block(0, SYSBOOT, target, 1)
    write(state + 4, 0)
    write(FATDRIVE, -1)
    write(DIRLBA, -1)
    if status == 0:
        puts("System transferred")
        newline()
    return status
