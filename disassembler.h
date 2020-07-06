//
// Created by xgladius on 2020-07-05.
//

#include <string>
#include <vector>
#include <cmath>
#include <unordered_map>
#include <sstream>

#ifndef LUAU_DISSASEMBLER_DISASSEMBLER_H
#define LUAU_DISSASEMBLER_DISASSEMBLER_H

#endif //LUAU_DISSASEMBLER_DISASSEMBLER_H

enum luau_op
{
    R_OP_NLE = 0x7D,
    R_OP_LT = 0xB7,
    R_OP_LOADNUMBER = 0x8C,
    R_OP_LONGJMP = 0x69,
    R_OP_LEN = 0x1C,
    R_OP_POWK = 0x4,
    R_OP_UNM = 0x39,
    R_OP_SELF = 0xBC,
    R_OP_SETUPVAL = 0xDE,
    R_OP_EQ = 0xF1,
    R_OP_NEQ = 0x9A,
    R_OP_JMPBACK = 0x48,
    R_OP_FORPREP = 0xA8,
    R_OP_CLOSURE = 0xD9,
    R_OP_OGETENV = 0xA4,
    R_OP_MOVE = 0x52,
    R_OP_TESTN = 0xe,
    R_OP_NOT = 0x56,
    R_OP_ADD = 0x43,
    R_OP_CLEARSTACK = 0xA3,
    R_OP_TESTM = 0x1A,
    R_OP_NEWTABLE = 0xFF,
    R_OP_GETUPVAL = 0xFB,
    R_OP_CALL = 0x9F,
    R_OP_MULK = 0x5b,
    R_OP_CONCAT = 0x73,
    R_OP_RETURN = 0x82,
    R_OP_VARARG = 0xDD,
    R_OP_LOADNIL = 0xC6,
    R_OP_GETENV = 0x35,
    R_OP_ADDK = 0x95,
    R_OP_GETTABLEK = 0x4D,
    R_OP_DIVK = 0x3E,
    R_OP_TFORPREP = 0x17,
    R_OP_GETTABLE = 0x87,
    R_OP_MODK = 0x21,
    R_OP_POW = 0xB2,
    R_OP_CLOSE = 0xC1,
    R_OP_LOADBOOL = 0xA9,
    R_OP_JMP = 0x65,
    R_OP_DIV = 0xEC,
    R_OP_FORLOOP = 0x8B,
    R_OP_MOD = 0xCF,
    R_OP_LE = 0xD4,
    R_OP_SETENV = 0x18,
    R_OP_CLEARSTACK_NVA = 0xC0,
    R_OP_MUL = 0x9,
    R_OP_SETTABLEK = 0x30,
    R_OP_SUBK = 0x78,
    R_OP_SETLIST = 0xC5,
    R_OP_TFORLOOP = 0xFA,
    R_OP_LOADK = 0x6F,
    R_OP_NLT = 0x60,
    R_OP_SUB = 0x26,
    R_OP_SETTABLE = 0x6A
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
    void disassemble(std::string& bytecode);
private:
    int get_and_pop();
    std::vector<std::string> toks;
    std::vector<std::string> str_table;
    std::unordered_map<int, k_type> k_table;
};