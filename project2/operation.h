#ifndef _OPERATION_H_
#define _OPERATION_H_
#include "util.h"
#define R_PARAM unsigned char rd, unsigned char rs, unsigned char rt
#define R_PARAM_SHIFT unsigned char rd, unsigned char rt, unsigned char sa
#define I_PARAM unsigned char rt, unsigned char rs, short imm
#define J_PARAM uint32_t target

extern CPU_State CURRENT_STATE;

//R-type operations
void op_addu(R_PARAM);
void op_and(R_PARAM);
void op_jr(R_PARAM);
void op_nor(R_PARAM);
void op_or(R_PARAM);
void op_subu(R_PARAM);
void op_sltu(R_PARAM);

//R-type operations with shift
void op_sll(R_PARAM_SHIFT);
void op_srl(R_PARAM_SHIFT);

//I-type operations
void op_addiu(I_PARAM);
void op_andi(I_PARAM);
void op_beq(I_PARAM);
void op_bne(I_PARAM);
void op_lw(I_PARAM);
void op_sltiu(I_PARAM);
void op_sw(I_PARAM);
void op_lui(I_PARAM);
void op_ori(I_PARAM);

//J-type operations
void op_j(J_PARAM);
void op_jal(J_PARAM);
#endif
