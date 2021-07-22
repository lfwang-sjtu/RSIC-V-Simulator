//
// Created by Snoopy on 2021/7/10.
//

#ifndef RISC_V_SIMULATOR_DECODE_HPP
#define RISC_V_SIMULATOR_DECODE_HPP

#include "byte_code.hpp"

//jalr ~ and_的数据使用了reg[rs1], beq ~ and_的数据使用了reg[rs2]的数据
enum op_type{
    lui, auipc,
    jal,
    jalr,
    lb, lh, lw, lbu, lhu,
    addi, slti, sltiu, xori, ori, andi, slli, srli, srai,
    beq, bne, blt, bge, bltu, bgeu,
    sb, sh, sw,
    add, sub, sll, slt, sltu, xor_, srl, sra, or_, and_,
    fail, END
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
    if (inst == 267388179) {op = END; return;}
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


#endif //RISC_V_SIMULATOR_DECODE_HPP
