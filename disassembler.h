//
// Created by xgladius on 2020-07-05.
//

#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>

#ifndef LUAU_DISSASEMBLER_DISASSEMBLER_H
#define LUAU_DISSASEMBLER_DISASSEMBLER_H

#endif //LUAU_DISSASEMBLER_DISASSEMBLER_H

enum luau_op
{
    NLE = 0x7D,
    LT = 0xB7,
    LOADNUMBER = 0x8C,
    LONGJMP = 0x69,
    LEN = 0x1C,
    POWK = 0x4,
    UNM = 0x39,
    SELF = 0xBC,
    SETUPVAL = 0xDE,
    EQ = 0xF1,
    NEQ = 0x9A,
    JMPBACK = 0x48,
    FORPREP = 0xA8,
    CLOSURE = 0xD9,
    OGETENV = 0xA4,
    MOVE = 0x52,
    TESTAND = 0xe,
    NOT = 0x56,
    ADD = 0x43,
    CLEARSTACK = 0xA3,
    TESTOR = 0x1A,
    NEWTABLE = 0xFF,
    GETUPVAL = 0xFB,
    CALL = 0x9F,
    MULK = 0x5b,
    CONCAT = 0x73,
    RETURN = 0x82,
    VARARG = 0xDD,
    LOADNIL = 0xC6,
    GETENV = 0x35,
    ADDK = 0x95,
    GETTABLEK = 0x4D,
    DIVK = 0x3E,
    TFORPREP = 0x17,
    GETTABLE = 0x87,
    MODK = 0x21,
    POW = 0xB2,
    CLOSE = 0xC1,
    LOADBOOL = 0xA9,
    JMP = 0x65,
    DIV = 0xEC,
    FORLOOP = 0x8B,
    MOD = 0xCF,
    LE = 0xD4,
    SETENV = 0x18,
    CLEARSTACK_NVA = 0xC0,
    MUL = 0x9,
    SETTABLEK = 0x30,
    SUBK = 0x78,
    SETLIST = 0xC5,
    TFORLOOP = 0xFA,
    LOADK = 0x6F,
    NLT = 0x60,
    SUB = 0x26,
    SETTABLE = 0x6A
};

struct k_type
{
    int type;
    union
    {
        double dval;
        int s_idx;
        bool bval;
    } value;
};

class disassembler
{
public:
    disassembler();
    void disassemble(std::string& bytecode);
private:
    int get_and_pop();
    std::vector<std::string> toks;
    std::vector<std::string> str_table;
    std::unordered_map<int, k_type> k_table;
};
