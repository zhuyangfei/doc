# gdb usage

## Table of contents
- [Table of contents](#table-of-contents)
- [debug with core dump](#debug-with-core-dump)
- [help](#help)
- [examine memory](#examine-memory)
- [run gdb with arguments](#run-gdb-with-arguments)
- [set shared library path](#set-shared-library-path)
- [REF](#ref)

## debug with core dump
gbd binFile coreFile

## help
print help information of gdb

## examine memory
Examine memory: x/FMT ADDRESS.
example:
x/8xg 0x7f535bc69000

## run gdb with arguments
(gdb)r args0 agrs1 argsn
[ref](https://cs50.stackexchange.com/questions/21681/how-do-you-pass-arguments-into-gdb-argv1-argv2)

## set shared library path
```bash
set environment LD_LIBRARY_PATH /path/to/library
```

## REF
[gdbonline](https://sourceware.org/gdb/current/onlinedocs/gdb/)
[gdb](https://visualgdb.com/gdbreference/commands/set_solib-search-path)


