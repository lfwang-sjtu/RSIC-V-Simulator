#include <iostream>
#include "decode.hpp"
#include "execute.hpp"

using namespace std;

unsigned int reg[32], pc;
unsigned char mem[500000];

unsigned char hex(char ch) {
    if (ch >= '0' && ch <= '9') return ch - '0';
    else return ch - 'A' + 10;
}

void get_input() {
    unsigned pos = 0;
    char ch;
    cin >> ch;
    while (ch != EOF) {
        if (ch == '@') cin >> hex >> pos;
        else {
            unsigned char x = hex(ch) * 16;
            cin >> ch;
            x += hex(ch);
            mem[pos++] = x;
        }
        ch = cin.get();
        while (ch == ' ' || ch == '\n') ch = cin.get();
    }
}

struct IF_to_ID_type {
    bool flag;
    unsigned inst, pc;
} IF_to_ID;

struct ID_to_EX_type {
    bool flag;
    op_type op;
    unsigned rs1_value, rs2_value, rd, imm, pc, reg10_value;
} ID_to_EX;

struct EX_to_MEM_type {
    bool flag;
    op_type op;
    unsigned pos, rd, rs2, write_to_rd;     //write_to_rd为WB写入reg[rd]的值, pos为MEM步骤中Load或Store的内存地址
} EX_to_MEM;

struct MEM_to_WB_type {
    bool flag;
    op_type op;
    unsigned rd, write_to_rd;
} MEM_to_WB;

void init() {
    pc = 0;
    IF_to_ID.flag = ID_to_EX.flag = EX_to_MEM.flag = MEM_to_WB.flag = false;
}

void instruction_fetch() {
    if (!IF_to_ID.flag) {
        IF_to_ID.flag = true;
        IF_to_ID.inst = (unsigned)mem[pc] + ((unsigned)mem[pc + 1] << 8) + ((unsigned)mem[pc + 2] << 16) + ((unsigned)mem[pc + 3] << 24);
        IF_to_ID.pc = pc;
        pc += 4;
    }
}

void instruction_decode() {
    if (IF_to_ID.flag && (!ID_to_EX.flag)) {
        unsigned _rs1, _rs2, _rd, _imm;
        op_type _op;
        decode(IF_to_ID.inst, _op, _rs1, _rs2, _rd, _imm);
        ID_to_EX.op = _op;
        ID_to_EX.rd = _rd, ID_to_EX.imm = _imm, ID_to_EX.pc = IF_to_ID.pc;

        //上上条指令的WB还没有完成
        if (_op >= jalr && _op <= and_) {
            if (MEM_to_WB.flag && MEM_to_WB.rd == _rs1) ID_to_EX.rs1_value = MEM_to_WB.write_to_rd;
            else ID_to_EX.rs1_value = reg[_rs1];
            if (EX_to_MEM.flag && EX_to_MEM.rd == _rs1) ID_to_EX.rs1_value =
        }
        if (_op >= beq && _op <= and_) {
            if (MEM_to_WB.flag && MEM_to_WB.rd == _rs2) ID_to_EX.rs2_value = MEM_to_WB.write_to_rd;
            else ID_to_EX.rs2_value = reg[_rs2];
        }
        if (MEM_to_WB.flag && MEM_to_WB.rd == 10) ID_to_EX.reg10_value = MEM_to_WB.write_to_rd;
        else ID_to_EX.reg10_value = reg[10];

        //上条指令的MEM和WB还没有完成

        IF_to_ID.flag = false;
        ID_to_EX.flag = true;
    }
}

void execute() {
    if (ID_to_EX.flag && (!EX_to_MEM.flag)) {
        op_type _op = ID_to_EX.op;
        unsigned rs1_value, rs2_value, _imm = ID_to_EX.imm, _pc = ID_to_EX.pc, _write_to_rd, _pos;
        if (_op >= jalr && _op <= and_) {
            if (ID_to_EX.rs1_h.flag) {
                rs1_value = ID_to_EX.rs1_h.value;
                ID_to_EX.rs1_h.flag = false;
            }
            else rs1_value = reg[ID_to_EX.rs1];
        }
        if (_op >= beq && _op <= and_) {
            if (ID_to_EX.rs2_h.flag) {
                rs2_value = ID_to_EX.rs2_h.value;
                ID_to_EX.rs2_h.flag = false;
            }
            else rs2_value = reg[ID_to_EX.rs2];
        }
        execute_command(_op, rs1_value, rs2_value, _imm, _pc, _write_to_rd, _pos);
        EX_to_MEM.op = _op;
        EX_to_MEM.rs2 = ID_to_EX.rs2, EX_to_MEM.rd = ID_to_EX.rd, EX_to_MEM.pos = _pos, EX_to_MEM.write_to_rd = _write_to_rd;
        ID_to_EX.flag = false;
        EX_to_MEM.flag = true;
        class jump{};
        class no_jump{};
        try {
            switch (_op) {
                case jal: throw jump();
                case jalr: _imm = ((rs1_value + _imm) & (~1)) - ID_to_EX.pc; throw jump();
                case beq: {
                    if (rs1_value == rs2_value) throw jump();
                    else throw no_jump();
                }
                case bne: {
                    if (rs1_value != rs2_value) throw jump();
                    else throw no_jump();
                }
                case blt: {
                    if ((signed)rs1_value < (signed)rs2_value) throw jump();
                    else throw no_jump();
                }
                case bltu: {
                    if ((unsigned)rs1_value < (unsigned)rs2_value) throw jump();
                    else throw no_jump();
                }
                case bge: {
                    if ((signed)rs1_value >= (signed)rs2_value) throw jump();
                    else throw no_jump();
                }
                case bgeu: {
                    if ((unsigned)rs1_value >= (unsigned)rs2_value) throw jump();
                    else throw no_jump();
                }
                case END: throw end_exception();
            }
        } catch (jump) {
            pc = ID_to_EX.pc += _imm;
            IF_to_ID.flag = false;
        } catch (no_jump) {

        } catch (end_exception) {
            if (ID_to_EX.reg10.flag) {
                cout << (ID_to_EX.reg10.value & 255u);
            } else cout << (reg[10] & 255u);
            throw end_exception();
        }
    }
}

void memory_access() {
    if (EX_to_MEM.flag && (!MEM_to_WB.flag)) {
        MEM_to_WB.rd = EX_to_MEM.rd, MEM_to_WB.op = EX_to_MEM.op, MEM_to_WB.write_to_rd = EX_to_MEM.write_to_rd;
        switch (EX_to_MEM.op) {
            //load指令从mem[pos]读出write_to_rd
            case lb: {
                MEM_to_WB.write_to_rd = sext((unsigned)mem[EX_to_MEM.pos], 8);
                break;
            }
            case lh: {
                MEM_to_WB.write_to_rd = sext((unsigned)mem[EX_to_MEM.pos] + ((unsigned)mem[EX_to_MEM.pos + 1] << 8), 16);
                break;
            }
            case lw: {
                MEM_to_WB.write_to_rd = (unsigned)mem[EX_to_MEM.pos] + ((unsigned)mem[EX_to_MEM.pos + 1] << 8) + ((unsigned)mem[EX_to_MEM.pos + 2] << 16) + ((unsigned)mem[EX_to_MEM.pos + 3] << 24);
                break;
            }
            case lbu: {
                MEM_to_WB.write_to_rd = (unsigned)mem[EX_to_MEM.pos];
                break;
            }
            case lhu: {
                MEM_to_WB.write_to_rd = (unsigned)mem[EX_to_MEM.pos] + ((unsigned)mem[EX_to_MEM.pos + 1] << 8);
                break;
            }
            //store指令从reg[rs2]读到mem[pos]
            case sb: {
                mem[EX_to_MEM.pos] = (unsigned char)get(reg[EX_to_MEM.rs2], 0, 7);
                break;
            }
            case sh: {
                mem[EX_to_MEM.pos] = (unsigned char)get(reg[EX_to_MEM.rs2], 0, 7);
                mem[EX_to_MEM.pos + 1] = (unsigned char)get(reg[EX_to_MEM.rs2], 8, 15);
                break;
            }
            case sw: {
                mem[EX_to_MEM.pos] = (unsigned char)get(reg[EX_to_MEM.rs2], 0, 7);
                mem[EX_to_MEM.pos + 1] = (unsigned char)get(reg[EX_to_MEM.rs2], 8, 15);
                mem[EX_to_MEM.pos + 2] = (unsigned char)get(reg[EX_to_MEM.rs2], 16, 23);
                mem[EX_to_MEM.pos + 3] = (unsigned char)get(reg[EX_to_MEM.rs2], 24, 31);
                break;
            }
            default:;
        }
        EX_to_MEM.flag = false;
        MEM_to_WB.flag = true;
    }
}

void write_back() {
    if (MEM_to_WB.flag) {
        if (MEM_to_WB.op >= beq && MEM_to_WB.op <= sw) {
            
        } else {
            if (MEM_to_WB.rd) reg[MEM_to_WB.rd] = MEM_to_WB.write_to_rd;
        }
        MEM_to_WB.flag = false;
    }
}

int main() {
    freopen("naive.data", "r", stdin);
    get_input();
    init();
    while (true) {
        try {
//            instruction_fetch();
//            instruction_decode();
//            execute();
//            memory_access();
//            write_back();
            write_back();
            memory_access();
            execute();
            instruction_decode();
            instruction_fetch();
        } catch (end_exception) {
            break;
        }
    }
    return 0;
}
