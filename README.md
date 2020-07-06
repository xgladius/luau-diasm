# luau-diasm
roblox luau bytecode format disassembler

doesn't currently support all opcodes that can be mapped to constants but i will be pushing updates :)

also currently will not output bytecode members past lineinfo if proto size > 0 because i haven't reversed lineinfo yet!

enjoy

to be used with a std::string passed to disassembler::disassemble, for example (main.cpp)
```
auto* d = new disassembler;
std::string bytecode = "01 02 05 70 72 69 6E 74 04 77 61 72 6E 01 05 00 00 01 0F A3 00 00 00 8C 02 01 00 8C 00 0A 00 8C 01 01 00 A8 00 09 00 A4 03 01 00 00 00 00 40 52 04 02 00 9F 03 02 01 A4 03 03 00 00 00 20 40 52 04 02 00 9F 03 02 01 8B 00 F7 FF 82 00 01 00 04 03 01 04 00 00 00 40 03 02 04 00 00 20 40 00 00 01 18 00 00 00 00 00 00 00 00 00 00 00 00 00 00 01 01 00 00 00 00 00";
d->disassemble(bytecode);
delete d;
```
