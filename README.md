# luau-diasm
roblox luau bytecode format disassembler

doesn't currently support all opcodes that can be mapped to constants but i will be pushing updates :)

also currently will not output bytecode members past lineinfo if proto size > 0 because i haven't reversed lineinfo yet!

enjoy

to be used with a std::string passed to disassembler::disassemble, for example (main.cpp)
```c
auto* d = new disassembler;
std::string bytecode = "01 02 05 70 72 69 6E 74 04 77 61 72 6E 01 05 00 00 01 0F A3 00 00 00 8C 02 01 00 8C 00 0A 00 8C 01 01 00 A8 00 09 00 A4 03 01 00 00 00 00 40 52 04 02 00 9F 03 02 01 A4 03 03 00 00 00 20 40 52 04 02 00 9F 03 02 01 8B 00 F7 FF 82 00 01 00 04 03 01 04 00 00 00 40 03 02 04 00 00 20 40 00 00 01 18 00 00 00 00 00 00 00 00 00 00 00 00 00 00 01 01 00 00 00 00 00";
d->disassemble(bytecode);
delete d;
```

example disassembly looks like
```
number of protos: 1
maxstacksize: 5
numparams: 0
nups: 0
isvararg: 1

[001] R_OP_CLEARSTACK      00 00 00
[002] R_OP_LOADNUMBER      02 01 00 ; 1
[003] R_OP_LOADNUMBER      00 10 00 ; 10
[004] R_OP_LOADNUMBER      01 01 00 ; 1
[005] R_OP_FORPREP         00 09 00 ; to 14
[006] R_OP_OGETENV         03 01 00 ; print
[007] PSEUDO               00 00 64
[008] R_OP_MOVE            04 02 00
[009] R_OP_CALL            03 02 01
[010] R_OP_OGETENV         03 03 00 ; print
[011] PSEUDO               00 32 64
[012] R_OP_MOVE            04 02 00
[013] R_OP_CALL            03 02 01
[014] R_OP_FORLOOP         00 -9 -1 ; to 5
[015] R_OP_RETURN          00 01 00

sizek: 4
sizep: 0
proto pool index: 0
lineinfo format: 1
lineinfo size: 24
should be 0: 0
head proto index: 0
```
