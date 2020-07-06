//
// Created by xgladius on 2020-07-05.
//

#include "disassembler.h"

#define init(name) [name] = #name

std::string luau_opnames[0xFF + 1]
{
    [0] = "PSEUDO",
    init(R_OP_MUL),
    init(R_OP_MULK),
    init(R_OP_TFORPREP),
    init(R_OP_LEN),
    init(R_OP_SUB),
    init(R_OP_SUBK),
    init(R_OP_TESTN),
    init(R_OP_TESTM),
    init(R_OP_SETTABLEK),
    init(R_OP_GETENV),
    init(R_OP_OGETENV),
    init(R_OP_SETENV),
    init(R_OP_UNM),
    init(R_OP_ADD),
    init(R_OP_ADDK),
    init(R_OP_GETTABLEK),
    init(R_OP_MOVE),
    init(R_OP_NOT),
    init(R_OP_NLT),
    init(R_OP_JMP),
    init(R_OP_JMPBACK),
    init(R_OP_SETTABLE),
    init(R_OP_LONGJMP),
    init(R_OP_LOADK),
    init(R_OP_CONCAT),
    init(R_OP_NLE),
    init(R_OP_RETURN),
    init(R_OP_GETTABLE),
    init(R_OP_FORLOOP),
    init(R_OP_LOADNUMBER),
    init(R_OP_NEQ),
    init(R_OP_CALL),
    init(R_OP_CLEARSTACK),
    init(R_OP_CLEARSTACK_NVA),
    init(R_OP_FORPREP),
    init(R_OP_LOADBOOL),
    init(R_OP_POW),
    init(R_OP_POWK),
    init(R_OP_LT),
    init(R_OP_SELF),
    init(R_OP_CLOSE),
    init(R_OP_SETLIST),
    init(R_OP_LOADNIL),
    init(R_OP_MOD),
    init(R_OP_MODK),
    init(R_OP_LE),
    init(R_OP_SETUPVAL),
    init(R_OP_CLOSURE),
    init(R_OP_VARARG),
    init(R_OP_DIV),
    init(R_OP_DIVK),
    init(R_OP_EQ),
    init(R_OP_TFORLOOP),
    init(R_OP_GETUPVAL),
    init(R_OP_NEWTABLE)
};

template <typename Out>
void split(const std::string &s, char delim, Out result) {
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim)) {
        *result++ = item;
    }
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

int get_longest()
{
    int longest = 0;
    for (auto &t : luau_opnames)
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
    for(auto p = 0; p < protos_sz; p++)
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
            switch(k_type)
            {
                case 0:
                    k_idx++;
                    break;
                case 1:
                {
                    const auto val = std::stoi(toks[k_idx], nullptr, 16);
                    if (val)
                        k_table[k] = {1, true };
                    else
                        k_table[k] = {1, false };
                    k_idx++;
                    break;
                }
                case 2:
                {
                    unsigned char fpp[8];
                    double val = 0;
                    for (auto x=0; x < 8; x++)
                        fpp[x] = std::stoi(toks[k_idx + x], nullptr, 16);
                    memcpy(&val, fpp, sizeof(double));
                    k_table[k] = {2, val};
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
                    k_idx+=4;
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
            printf("[%03d] %s %*s %02d %02d %02d", i/4 + 1, name.c_str(), longest - name.size(), "", (char)a, (char)b, (char)c);
            switch(op)
            {
                case R_OP_LOADNUMBER:
                    printf(" ; %d", (char)b);
                    break;
                case R_OP_LOADK:
                    if (k_table[b].type == 3)
                        printf(" ; %s", str_table.at(k_table[a].value.s_idx).c_str());
                    else if (k_table[b].type == 2)
                        printf(" ; %f", k_table[b].value.dval);
                    break;
                case R_OP_FORLOOP:
                case R_OP_FORPREP:
                case R_OP_JMP:
                    printf(" ; to %d", i/4 + 1 + (char)b);
                    break;
                case R_OP_GETENV:
                case R_OP_SETENV:
                    printf(" ; %s", str_table.at(k_table[a].value.s_idx).c_str());
                    break;
                case R_OP_OGETENV:
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
            switch(k_type)
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