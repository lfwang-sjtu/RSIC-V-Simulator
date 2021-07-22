//
// Created by Snoopy on 2021/7/9.
//

#ifndef RISC_V_SIMULATOR_EXECUTE_HPP
#define RISC_V_SIMULATOR_EXECUTE_HPP

#include <iostream>

extern unsigned int reg[32];



void addi_exe(unsigned rs1_value, unsigned imm, unsigned &write_to_rd) {
    write_to_rd = rs1_value + imm;
}

void slti_exe(unsigned rs1_value, unsigned imm, unsigned &write_to_rd) {
    if ((signed)rs1_value < (signed)imm) write_to_rd = 1;
    else write_to_rd = 0;
}

void sltiu_exe(unsigned rs1_value, unsigned imm, unsigned &write_to_rd) {
    if ((unsigned)rs1_value < (unsigned)imm) write_to_rd = 1;
    else write_to_rd = 0;
}

void xori_exe(unsigned rs1_value, unsigned imm, unsigned &write_to_rd) {
    write_to_rd = rs1_value ^ imm;
}

void ori_exe(unsigned rs1_value, unsigned imm, unsigned &write_to_rd) {
    write_to_rd = rs1_value | imm;
}

void andi_exe(unsigned rs1_value, unsigned imm, unsigned &write_to_rd) {
    write_to_rd = rs1_value & imm;
}

void slli_exe(unsigned rs1_value, unsigned imm, unsigned &write_to_rd){
    unsigned shamt = get(imm, 0, 4);
    write_to_rd = (rs1_value << shamt);
}

void srli_exe(unsigned rs1_value, unsigned imm, unsigned &write_to_rd) {
    unsigned shamt = get(imm, 0, 4);
    write_to_rd = (rs1_value >> shamt);
}

void srai_exe(unsigned rs1_value, unsigned imm, unsigned &write_to_rd) {
    unsigned shamt = get(imm, 0, 4);
    write_to_rd = sext(rs1_value >> shamt, 32 - shamt);
}

void lui_exe(unsigned imm, unsigned &write_to_rd) {
    write_to_rd = imm;
}

void auipc_exe(unsigned imm, unsigned pc, unsigned &write_to_rd) {
    write_to_rd = pc + imm;
}

void add_exe(unsigned rs1_value, unsigned rs2_value, unsigned &write_to_rd) {
    write_to_rd = rs1_value + rs2_value;
}

void sub_exe(unsigned rs1_value, unsigned rs2_value, unsigned &write_to_rd) {
    write_to_rd = rs1_value - rs2_value;
}

void slt_exe(unsigned rs1_value, unsigned rs2_value, unsigned &write_to_rd) {
    if ((signed)rs1_value < (signed)rs2_value) write_to_rd = 1;
    else write_to_rd = 0;
}

void sltu_exe(unsigned rs1_value, unsigned rs2_value, unsigned &write_to_rd) {
    if ((unsigned)rs1_value< (unsigned)rs2_value) write_to_rd = 1;
    else write_to_rd = 0;
}

void and_exe(unsigned rs1_value, unsigned rs2_value, unsigned &write_to_rd) {
    write_to_rd = rs1_value & rs2_value;
}

void or_exe(unsigned rs1_value, unsigned rs2_value, unsigned &write_to_rd) {
    write_to_rd = rs1_value | rs2_value;
}

void xor_exe(unsigned rs1_value, unsigned rs2_value, unsigned &write_to_rd) {
    write_to_rd = rs1_value ^ rs2_value;
}

void sll_exe(unsigned rs1_value, unsigned rs2_value, unsigned &write_to_rd) {
    unsigned shamt = get(rs2_value, 0, 4);
    write_to_rd = (rs1_value << shamt);
}

void srl_exe(unsigned rs1_value, unsigned rs2_value, unsigned &write_to_rd) {
    unsigned shamt = get(rs2_value, 0, 4);
    write_to_rd = (rs1_value >> shamt);
}

void sra_exe(unsigned rs1_value, unsigned rs2_value, unsigned &write_to_rd) {
    unsigned shamt = get(rs2_value, 0, 4);
    write_to_rd = sext(rs1_value >> shamt, 32 - shamt);
}

void jal_exe(unsigned imm, unsigned pc, unsigned &write_to_rd) {
    write_to_rd = pc + 4;
}

void jalr_exe(unsigned pc, unsigned &write_to_rd) {
    write_to_rd = pc + 4;
}

void beq_exe() {

}

void bne_exe() {

}

void blt_exe() {

}

void bltu_exe() {

}

void bge_exe() {

}

void bgeu_exe() {

}

void lw_exe(unsigned rs1_value, unsigned imm, unsigned &pos) {
    pos = rs1_value + imm;
}

void lh_exe(unsigned rs1_value, unsigned imm, unsigned &pos) {
    pos = rs1_value + imm;
}

void lhu_exe(unsigned rs1_value, unsigned imm, unsigned &pos) {
    pos = rs1_value + imm;
}

void lb_exe(unsigned rs1_value, unsigned imm, unsigned &pos) {
    pos = rs1_value + imm;
}

void lbu_exe(unsigned rs1_value, unsigned imm, unsigned &pos) {
    pos = rs1_value + imm;
}

void sb_exe(unsigned rs1_value, unsigned imm, unsigned &pos) {
    pos = rs1_value + imm;
}

void sh_exe(unsigned rs1_value, unsigned imm, unsigned &pos) {
    pos = rs1_value + imm;
}

void sw_exe(unsigned rs1_value, unsigned imm, unsigned &pos) {
    pos = rs1_value + imm;
}

class end_exception{};

void execute_command(op_type op, unsigned rs1_value, unsigned rs2_value, unsigned imm, unsigned pc, unsigned &write_to_rd, unsigned &pos) {
    switch (op) {
        case lui: lui_exe(imm, write_to_rd); break;
        case auipc: auipc_exe(imm, pc, write_to_rd); break;
        case jal: jal_exe(imm, pc, write_to_rd); break;
        case jalr: jalr_exe(rs1_value, imm); break;
        case beq: beq_exe(); break;
        case bne: bne_exe(); break;
        case blt: blt_exe(); break;
        case bge: bge_exe(); break;
        case bltu: bltu_exe(); break;
        case bgeu: bgeu_exe(); break;
        case lb: lb_exe(rs1_value, imm, pos); break;
        case lh: lh_exe(rs1_value, imm, pos); break;
        case lw: lw_exe(rs1_value, imm, pos); break;
        case lbu: lbu_exe(rs1_value, imm, pos); break;
        case lhu: lhu_exe(rs1_value, imm, pos); break;
        case sb: sb_exe(rs1_value, imm, pos); break;
        case sh: sh_exe(rs1_value, imm, pos); break;
        case sw: sw_exe(rs1_value, imm, pos); break;
        case addi: addi_exe(rs1_value, imm, write_to_rd); break;
        case slti: slti_exe(rs1_value, imm, write_to_rd); break;
        case sltiu: sltiu_exe(rs1_value, imm, write_to_rd); break;
        case xori: xori_exe(rs1_value, imm, write_to_rd); break;
        case ori: ori_exe(rs1_value, imm, write_to_rd); break;
        case andi: andi_exe(rs1_value, imm, write_to_rd); break;
        case slli: slli_exe(rs1_value, imm, write_to_rd); break;
        case srli: srli_exe(rs1_value, imm, write_to_rd); break;
        case srai: srai_exe(rs1_value, imm, write_to_rd); break;
        case add: add_exe(rs1_value, rs2_value, write_to_rd); break;
        case sub: sub_exe(rs1_value, rs2_value, write_to_rd); break;
        case sll: sll_exe(rs1_value, rs2_value, write_to_rd); break;
        case slt: slt_exe(rs1_value, rs2_value, write_to_rd); break;
        case sltu: sltu_exe(rs1_value, rs2_value, write_to_rd); break;
        case xor_: xor_exe(rs1_value, rs2_value, write_to_rd); break;
        case srl: srl_exe(rs1_value, rs2_value, write_to_rd); break;
        case sra: sra_exe(rs1_value, rs2_value, write_to_rd); break;
        case or_: or_exe(rs1_value, rs2_value, write_to_rd); break;
        case and_: and_exe(rs1_value, rs2_value, write_to_rd); break;
        case fail: std::cout << "decode failed\n"; break;
        case END: break;
        default: std::cout << "invalid\n";
    }
}

#endif //RISC_V_SIMULATOR_EXECUTE_HPP
