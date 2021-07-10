//
// Created by Snoopy on 2021/7/9.
//

#ifndef RISC_V_SIMULATOR_EXECUTE_HPP
#define RISC_V_SIMULATOR_EXECUTE_HPP

#include <iostream>
#include "decode.hpp"

extern unsigned int reg[32], pc;
extern unsigned char mem[500000];

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

#endif //RISC_V_SIMULATOR_EXECUTE_HPP
