#include <iostream>

using namespace std;

unsigned int reg[32], pc;
unsigned char mem[500000];

unsigned get(unsigned x, int low, int high) {
    x >>= low;
    return x & ((1u << (high - low + 1)) - 1);
}

void set(unsigned &x, int low, int high, unsigned y) {
    x &= ~(((1u << (high - low + 1)) - 1) << low);
    x += (y & ((1u << (high - low + 1)) - 1)) << low;
}

unsigned sext(unsigned x, unsigned n) {
    if (x & (1u << (n - 1))) {
        return x - (1u << n);
    } else return x;
}

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

unsigned fetch(unsigned pc) {
    unsigned inst = mem[pc] + (mem[pc + 1] << 8) + (mem[pc + 2] << 16) + (mem[pc + 3] << 24);
    return inst;
}

enum op_type{
    lui, auipc,
    jal,
    jalr,
    beq, bne, blt, bge, bltu, bgeu,
    lb, lh, lw, lbu, lhu,
    sb, sh, sw,
    addi, slti, sltiu, xori, ori, andi, slli, srli, srai,
    add, sub, sll, slt, sltu, xor_, srl, sra, or_, and_,
    fail
};

void R_type(unsigned inst, unsigned &rs1, unsigned &rs2, unsigned &rd) {
    rs1 = get(inst, 15, 19);
    rs2 = get(inst, 20, 24);
    rd = get(inst, 7, 11);
}

void I_type(unsigned inst, unsigned &rs1, unsigned &rd, unsigned &imm) {
    rs1 = get(inst, 15, 19);
    rd = get(inst, 7, 11);
    imm = get(inst, 20, 31);
    imm = sext(imm, 12);
}

void S_type(unsigned inst, unsigned &rs1, unsigned &rs2, unsigned &imm) {
    rs1 = get(inst, 15, 19);
    rs2 = get(inst, 20, 24);
    imm = 0;
    set(imm, 5, 11, get(inst, 25, 31));
    set(imm, 1, 4, get(inst, 8, 11));
    set(imm, 0, 0, get(inst, 7, 7));
    imm = sext(imm, 12);
}

void B_type(unsigned inst, unsigned &rs1, unsigned &rs2, unsigned &imm) {
    rs1 = get(inst, 15, 19);
    rs2 = get(inst, 20, 24);
    imm = 0;
    set(imm, 12, 12, get(inst, 31, 31));
    set(imm, 11, 11, get(inst, 7, 7));
    set(imm, 5, 10, get(inst, 25, 30));
    set(imm, 1, 4, get(inst, 8, 11));
    imm = sext(imm, 13);
}

void U_type(unsigned inst, unsigned &rd, unsigned &imm) {
    rd = get(inst, 7, 11);
    imm = 0;
    set(imm, 12, 31, get(inst, 12, 31));
}

void J_type(unsigned inst, unsigned &rd, unsigned &imm) {
    rd = get(inst, 7, 11);
    imm = 0;
    set(imm, 20, 20, get(inst, 31, 31));
    set(imm, 12, 19, get(inst, 12, 19));
    set(imm, 11, 11, get(inst, 20, 20));
    set(imm, 1, 10, get(inst, 21, 30));
    imm = sext(imm, 21);
}

void decode(unsigned inst, op_type &op, unsigned &rs1, unsigned &rs2, unsigned &rd, unsigned &imm) {
    unsigned opcode = get(inst, 0, 6);
    switch (opcode) {
        //U lui
        case 55: U_type(inst, rd, imm); op = lui; break;
            //U auipc
        case 23: U_type(inst, rd, imm); op = auipc; break;
            //J jal
        case 111: J_type(inst, rd, imm); op = jal; break;
            //I jalr
        case 103: I_type(inst, rs1, rd, imm); op = jalr; break;
            //B beq bne blt bge bltu bgeu
        case 99: {
            B_type(inst, rs1, rs2, imm);
            unsigned funct3 = get(inst, 12, 14);
            switch (funct3) {
                case 0: op = beq; break;
                case 1: op = bne; break;
                case 4: op = blt; break;
                case 5: op = bge; break;
                case 6: op = bltu; break;
                case 7: op = bgeu; break;
                default: op = fail;
            }
            break;
        }
            //I lb lh lw lbu lhu
        case 3: {
            I_type(inst, rs1, rd, imm);
            unsigned funct3 = get(inst, 12, 14);
            switch (funct3) {
                case 0: op = lb; break;
                case 1: op = lh; break;
                case 2: op = lw; break;
                case 4: op = lbu; break;
                case 5: op = lhu; break;
                default: op = fail;
            }
            break;
        }
            //S sb sh sw
        case 35: {
            S_type(inst, rs1, rs2, imm);
            unsigned funct3 = get(inst, 12, 14);
            switch (funct3) {
                case 0: op = sb; break;
                case 1: op = sh; break;
                case 2: op = sw; break;
                default: op = fail;
            }
            break;
        }
            //I addi slti sltiu xori ori andi slli srli srai
        case 19: {
            I_type(inst, rs1, rd, imm);
            unsigned funct3 = get(inst, 12, 14);
            switch (funct3) {
                case 0: op = addi; break;
                case 2: op = slti; break;
                case 3: op = sltiu; break;
                case 4: op = xori; break;
                case 6: op = ori; break;
                case 7: op = andi; break;
                case 1: op = slli; break;
                case 5: {
                    if (get(inst,30, 30)) op = srai;
                    else op = srli;
                    break;
                }
                default: op = fail;
            }
            break;
        }
            //R add sub sll slt sltu xor srl sra or and
        case 51: {
            R_type(inst, rs1, rs2, rd);
            unsigned funct3 = get(inst, 12, 14);
            switch (funct3) {
                case 0: {
                    if (get(inst, 30, 30)) op = sub;
                    else op = add;
                    break;
                }
                case 1: op = sll; break;
                case 2: op = slt; break;
                case 3: op = sltu; break;
                case 4: op = xor_; break;
                case 5: {
                    if (get(inst, 30, 30)) op = sra;
                    else op = srl;
                    break;
                }
                case 6: op = or_; break;
                case 7: op = and_; break;
                default: op = fail;
            }
            break;
        }
        default: op = fail;
    }
}

void addi_exe(unsigned rs1, unsigned rd, unsigned imm) {
    reg[rd] = reg[rs1] + imm;
    pc += 4;
}

void slti_exe(unsigned rs1, unsigned rd, unsigned imm) {
    if ((signed)reg[rs1] < (signed)imm) reg[rd] = 1;
    else reg[rd] = 0;
    pc += 4;
}

void sltiu_exe(unsigned rs1, unsigned rd, unsigned imm) {
    if ((unsigned)reg[rs1] < (unsigned)imm) reg[rd] = 1;
    else reg[rd] = 0;
    pc += 4;
}

void xori_exe(unsigned rs1, unsigned rd, unsigned imm) {
    reg[rd] = reg[rs1] ^ imm;
    pc += 4;
}

void ori_exe(unsigned rs1, unsigned rd, unsigned imm) {
    reg[rd] = reg[rs1] | imm;
    pc += 4;
}

void andi_exe(unsigned rs1, unsigned rd, unsigned imm) {
    reg[rd] = reg[rs1] & imm;
    pc += 4;
}

void slli_exe(unsigned rs1, unsigned rd, unsigned imm){
    unsigned shamt = get(imm, 0, 4);
    reg[rd] = (reg[rs1] << shamt);
    pc += 4;
}

void srli_exe(unsigned rs1, unsigned rd, unsigned imm) {
    unsigned shamt = get(imm, 0, 4);
    reg[rd] = (reg[rs1] >> shamt);
    pc += 4;
}

void srai_exe(unsigned rs1, unsigned rd, unsigned imm) {
    unsigned shamt = get(imm, 0, 4);
    reg[rd] = sext(reg[rs1] >> shamt, 32 - shamt);
    pc += 4;
}

void lui_exe(unsigned rd, unsigned imm) {
    reg[rd] = imm;
    pc += 4;
}

void auipc_exe(unsigned rd, unsigned imm) {
    reg[rd] = pc + imm;
    pc += 4;
}

void add_exe(unsigned rs1, unsigned rs2, unsigned rd) {
    reg[rd] = reg[rs1] + reg[rs2];
    pc += 4;
}

void sub_exe(unsigned rs1, unsigned rs2, unsigned rd) {
    reg[rd] = reg[rs1] - reg[rs2];
    pc += 4;
}

void slt_exe(unsigned rs1, unsigned rs2, unsigned rd) {
    if ((signed)reg[rs1] < (signed)reg[rs2]) reg[rd] = 1;
    else reg[rd] = 0;
    pc += 4;
}

void sltu_exe(unsigned rs1, unsigned rs2, unsigned rd) {
    if ((unsigned)reg[rs1] < (unsigned)reg[rs2]) reg[rd] = 1;
    else reg[rd] = 0;
    pc += 4;
}

void and_exe(unsigned rs1, unsigned rs2, unsigned rd) {
    reg[rd] = reg[rs1] & reg[rs2];
    pc += 4;
}

void or_exe(unsigned rs1, unsigned rs2, unsigned rd) {
    reg[rd] = reg[rs1] | reg[rs2];
    pc += 4;
}

void xor_exe(unsigned rs1, unsigned rs2, unsigned rd) {
    reg[rd] = reg[rs1] ^ reg[rs2];
    pc += 4;
}

void sll_exe(unsigned rs1, unsigned rs2, unsigned rd) {
    unsigned shamt = get(rs2, 0, 4);
    reg[rd] = (reg[rs1] << shamt);
    pc += 4;
}

void srl_exe(unsigned rs1, unsigned rs2, unsigned rd) {
    unsigned shamt = get(rs2, 0, 4);
    reg[rd] = (reg[rs1] >> shamt);
    pc += 4;
}

void sra_exe(unsigned rs1, unsigned rs2, unsigned rd) {
    unsigned shamt = get(rs2, 0, 4);
    reg[rd] = sext(reg[rs1] >> shamt, 32 - shamt);
    pc += 4;
}

void jal_exe(unsigned rd, unsigned imm) {
    if (rd) reg[rd] = pc + 4;
    pc += imm;
}

void jalr_exe(unsigned rs1, unsigned rd, unsigned imm) {
    unsigned t = pc + 4;
    pc = (reg[rs1] + imm) & (~1);
    if (rd) reg[rd] = t;
}

void beq_exe(unsigned rs1, unsigned rs2, unsigned imm) {
    if (reg[rs1] == reg[rs2]) pc += imm;
    else pc += 4;
}

void bne_exe(unsigned rs1, unsigned rs2, unsigned imm) {
    if (reg[rs1] != reg[rs2]) pc += imm;
    else pc += 4;
}

void blt_exe(unsigned rs1, unsigned rs2, unsigned imm) {
    if ((signed)reg[rs1] < (signed)reg[rs2]) pc += imm;
    else pc += 4;
}

void bltu_exe(unsigned rs1, unsigned rs2, unsigned imm) {
    if ((unsigned)reg[rs1] < (unsigned)reg[rs2]) pc += imm;
    else pc += 4;
}

void bge_exe(unsigned rs1, unsigned rs2, unsigned imm) {
    if ((signed)reg[rs1] >= (signed)reg[rs2]) pc += imm;
    else pc += 4;
}

void bgeu_exe(unsigned rs1, unsigned rs2, unsigned imm) {
    if ((unsigned)reg[rs1] >= (unsigned)reg[rs2]) pc += imm;
    else pc += 4;
}

void lw_exe(unsigned rs1, unsigned rd, unsigned imm) {
    unsigned pos = reg[rs1] + imm;
    reg[rd] = (unsigned)mem[pos] + ((unsigned)mem[pos + 1] << 8) + ((unsigned)mem[pos + 2] << 16) + ((unsigned)mem[pos + 3] << 24);
    pc += 4;
}

void lh_exe(unsigned rs1, unsigned rd, unsigned imm) {
    unsigned pos = reg[rs1] + imm;
    reg[rd] = sext((unsigned)mem[pos] + ((unsigned)mem[pos + 1] << 8), 16);
    pc += 4;
}

void lhu_exe(unsigned rs1, unsigned rd, unsigned imm) {
    unsigned pos = reg[rs1] + imm;
    reg[rd] = (unsigned)mem[pos] + ((unsigned)mem[pos + 1] << 8);
    pc += 4;
}

void lb_exe(unsigned rs1, unsigned rd, unsigned imm) {
    unsigned pos = reg[rs1] + imm;
    reg[rd] = sext((unsigned)mem[pos], 8);
    pc += 4;
}

void lbu_exe(unsigned rs1, unsigned rd, unsigned imm) {
    unsigned pos = reg[rs1] + imm;
    reg[rd] = (unsigned)mem[pos];
    pc += 4;
}

void sb_exe(unsigned rs1, unsigned rs2, unsigned imm) {
    unsigned pos = reg[rs1] + imm;
    mem[pos] = (unsigned char)get(reg[rs2], 0, 7);
    pc += 4;
}

void sh_exe(unsigned rs1, unsigned rs2, unsigned imm) {
    unsigned pos = reg[rs1] + imm;
    mem[pos] = (unsigned char)get(reg[rs2], 0, 7);
    mem[pos + 1] = (unsigned char)get(reg[rs2], 8, 15);
    pc += 4;
}

void sw_exe(unsigned rs1, unsigned rs2, unsigned imm) {
    unsigned pos = reg[rs1] + imm;
    mem[pos] = (unsigned char)get(reg[rs2], 0, 7);
    mem[pos + 1] = (unsigned char)get(reg[rs2], 8, 15);
    mem[pos + 2] = (unsigned char)get(reg[rs2], 16, 23);
    mem[pos + 3] = (unsigned char)get(reg[rs2], 24, 31);
    pc += 4;
}

void execute(unsigned inst) {
    unsigned rs1, rs2, rd, imm;
    op_type op;
    decode(inst, op, rs1, rs2, rd, imm);
    switch (op) {
        case lui: lui_exe(rd, imm); break;
        case auipc: auipc_exe(rd, imm); break;
        case jal: jal_exe(rd, imm); break;
        case jalr: jalr_exe(rs1, rd, imm); break;
        case beq: beq_exe(rs1, rs2, imm); break;
        case bne: bne_exe(rs1, rs2, imm); break;
        case blt: blt_exe(rs1, rs2, imm); break;
        case bge: bge_exe(rs1, rs2, imm); break;
        case bltu: bltu_exe(rs1, rs2, imm); break;
        case bgeu: bgeu_exe(rs1, rs2, imm); break;
        case lb: lb_exe(rs1, rd, imm); break;
        case lh: lh_exe(rs1, rd, imm); break;
        case lw: lw_exe(rs1, rd, imm); break;
        case lbu: lbu_exe(rs1, rd, imm); break;
        case lhu: lhu_exe(rs1, rd, imm); break;
        case sb: sb_exe(rs1, rs2, imm); break;
        case sh: sh_exe(rs1, rs2, imm); break;
        case sw: sw_exe(rs1, rs2, imm); break;
        case addi: addi_exe(rs1, rd, imm); break;
        case slti: slti_exe(rs1, rd, imm); break;
        case sltiu: sltiu_exe(rs2, rd, imm); break;
        case xori: xori_exe(rs1, rd, imm); break;
        case ori: ori_exe(rs1, rd, imm); break;
        case andi: andi_exe(rs1, rd, imm); break;
        case slli: slli_exe(rs1, rd, imm); break;
        case srli: srli_exe(rs1, rd, imm); break;
        case srai: srai_exe(rs1, rd, imm); break;
        case add: add_exe(rs1, rs2, rd); break;
        case sub: sub_exe(rs1, rs2, rd); break;
        case sll: sll_exe(rs1, rs2, rd); break;
        case slt: slt_exe(rs1, rs2, rd); break;
        case sltu: sltu_exe(rs1, rs2, rd); break;
        case xor_: xor_exe(rs1, rs2, rd); break;
        case srl: srl_exe(rs1, rs2, rd); break;
        case sra: sra_exe(rs1, rs2, rd); break;
        case or_: or_exe(rs1, rs2, rd); break;
        case and_: and_exe(rs1, rs2, rd); break;
        case fail: std::cout << "decode failed\n"; break;
        default: std::cout << "invalid\n";
    }
}

int j = 0;

int main() {
//    freopen("tak.data", "r", stdin);
    get_input();
    pc = 0;
    while (true) {
        unsigned inst = fetch(pc);
        if (inst == 267388179) {
            cout << (reg[10] & 255u);
            break;
        } else execute(inst);

//        if (pc < 4096 || j < 50){
//            for (int i = 0; i < 32; ++i)
//                cout << reg[i] << ' ';
//            cout << endl;
//            ++j;
//        } else break;
    }
    return 0;
}
