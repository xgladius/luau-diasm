//
// Created by xgladius on 2020-07-05.
//

#include "disassembler.h"
#include <cassert>

#define init(name) luau_opnames[name] = #name

std::string luau_opnames[0xFF + 1]
{
    "PSEUDO"
};

void init_members()
{
    init(MUL);
    init(MULK);
    init(TFORPREP);
    init(LEN);
    init(SUB);
    init(SUBK);
    init(TESTN);
    init(TESTM);
    init(SETTABLEK);
    init(GETENV);
    init(OGETENV);
    init(SETENV);
    init(UNM);
    init(ADD);
    init(ADDK);
    init(GETTABLEK);
    init(MOVE);
    init(NOT);
    init(NLT);
    init(JMP);
    init(JMPBACK);
    init(SETTABLE);
    init(LONGJMP);
    init(LOADK);
    init(CONCAT);
    init(NLE);
    init(RETURN);
    init(GETTABLE);
    init(FORLOOP);
    init(LOADNUMBER);
    init(NEQ);
    init(CALL);
    init(CLEARSTACK);
    init(CLEARSTACK_NVA);
    init(FORPREP);
    init(LOADBOOL);
    init(POW);
    init(POWK);
    init(LT);
    init(SELF);
    init(CLOSE);
    init(SETLIST);
    init(LOADNIL);
    init(MOD);
    init(MODK);
    init(LE);
    init(SETUPVAL);
    init(CLOSURE);
    init(VARARG);
    init(DIV);
    init(DIVK);
    init(EQ);
    init(TFORLOOP);
    init(GETUPVAL);
    init(NEWTABLE);
}

disassembler::disassembler()
{
    init_members();
}


template <typename Out>
void split(const std::string& s, char delim, Out result)
{
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim)) {
        *result++ = item;
    }
}

std::vector<std::string> split(const std::string& s, char delim)
{
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

int get_longest()
{
    int longest = 0;
    for (auto& t : luau_opnames)
    {
        if (t.size() > longest)
            longest = t.size();
    }
    return longest;
}

int disassembler::get_and_pop()
{
    std::string ret = "-1";
    if (!toks.empty())
    {
        ret = toks[0];
        toks.erase(toks.begin());
    }
    return std::stoi(ret, nullptr, 16);
}

void disassembler::disassemble(std::string& bytecode)
{
    toks = split(bytecode, ' ');
    toks.erase(toks.begin()); // don't care about did error

    const int str_table_sz = get_and_pop();
    for (auto i = 0; i < str_table_sz; i++)
    {
        const int str_sz = get_and_pop();
        std::string str;
        for (auto s = 0; s < str_sz; s++)
        {
            const auto int_val = get_and_pop();
            str += (char)int_val;
        }
        str_table.push_back(str);
    }

    const int protos_sz = get_and_pop();
    printf("number of protos: %d\n", protos_sz);
    for (auto p = 0; p < protos_sz; p++)
    {
        printf("maxstacksize: %d\nnumparams: %d\nnups: %d\nisvararg: %d\n\n", get_and_pop(), get_and_pop(), get_and_pop(), get_and_pop());
        const auto instr_len = get_and_pop();

        // need to preload constants
        auto k_idx = instr_len * 4;
        const auto constant_size = std::stoi(toks[k_idx], nullptr, 16);
        k_idx++;
        for (auto k = 0; k < constant_size; k++)
        {
            const auto k_type = std::stoi(toks[k_idx], nullptr, 16);
            k_idx++;
            switch (k_type)
            {
            case 0:
                k_idx++;
                break;
            case 1:
            {
                const auto val = std::stoi(toks[k_idx], nullptr, 16);
                if (val)
                    k_table[k] = { 1, true };
                else
                    k_table[k] = { 1, false };
                k_idx++;
                break;
            }
            case 2:
            {
                unsigned char fpp[8];
                double val = 0;
                for (auto x = 0; x < 8; x++)
                    fpp[x] = std::stoi(toks[k_idx + x], nullptr, 16);
                memcpy(&val, fpp, sizeof(double));
                k_table[k] = { 2, val };
                k_idx += 8;
                break;
            }
            case 3:
            {
                const int s_idx = std::stoi(toks[k_idx], nullptr, 16);
                struct k_type val;
                val.type = 3;
                val.value.s_idx = s_idx;
                k_table[k] = val;
                k_idx++;
                break;
            }
            case 4:
            {
                k_idx += 4;
                break;
            }
            default:
                printf("unknwon %x\n", k_type);
            }
        }

        const auto longest = get_longest();
        for (auto i = 1; i < instr_len * 4; i += 4) // assume double width
        {
            const auto op = get_and_pop();
            const auto a = get_and_pop();
            const auto b = get_and_pop();
            const auto c = get_and_pop();
            auto name = luau_opnames[op];
            if (name.empty())
                name = "PSEUDO";
            printf("[%03d] %s %*s %02d %02d %02d", i / 4 + 1, name.c_str(), longest - name.size(), "", (char)a, (char)b, (char)c);
            switch (op)
            {
            case LOADNUMBER:
                printf(" ; %d", (char)b);
                break;
            case LOADK:
                if (k_table[b].type == 3)
                    printf(" ; %s", str_table.at(k_table[a].value.s_idx).c_str());
                else if (k_table[b].type == 2)
                    printf(" ; %f", k_table[b].value.dval);
                break;
            case FORLOOP:
            case FORPREP:
            case JMP:
                printf(" ; to %d", i / 4 + 1 + (char)b);
                break;
            case GETENV:
            case SETENV:
                printf(" ; %s", str_table.at(k_table[a].value.s_idx).c_str());
                break;
            case OGETENV:
                printf(" ; %s", str_table.at(k_table[b].value.s_idx).c_str());
                break;
            default:
                break;
            }
            printf("\n");
        }

        printf("\nsizek: %d\n", get_and_pop());
        for (auto k = 0; k < constant_size; k++)
        {
            const auto k_type = get_and_pop();
            switch (k_type)
            {
            case 0:
                get_and_pop();
                break;
            case 1:
                get_and_pop(); get_and_pop();
                break;
            case 2:
                for (auto x = 0; x < 8; x++)
                    get_and_pop();
                break;
            case 3:
                get_and_pop();
                break;
            case 4:
                for (auto x = 0; x < 4; x++)
                    get_and_pop();
                break;
            default:
                break;
            }
        }

        const auto sizep = get_and_pop();
        printf("sizep: %d\n", sizep);

        for (auto cp = 0; cp < sizep; cp++)
        {
            printf("child proto at index: %d\n", get_and_pop());
        }

        printf("proto pool index: %d\n", get_and_pop());
        printf("lineinfo format: %x\n", get_and_pop());

        const auto lineinfo_sz = get_and_pop();
        const auto u_sz = ((lineinfo_sz + 3) & 0x4) + 4 * (((lineinfo_sz - 1) >> 0x18) + 1);

        printf("lineinfo size: %d\n", lineinfo_sz);

        if (lineinfo_sz != 0)
        {
            for (auto l = 0; l < instr_len; l++) {
                get_and_pop();
            }

            for (auto l = 0; l < u_sz; l++) {
                get_and_pop();
            }
        }

        printf("should be 0: %d\n", get_and_pop());
        printf("head proto index: %d\n", get_and_pop());

        assert(toks.empty());
    }
}
