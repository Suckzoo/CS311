#include "operation.h"

//R-type operations: (rd, rs, rt)
void op_addu(R_PARAM)
{
	CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
}
void op_and(R_PARAM)
{
	CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] & CURRENT_STATE.REGS[rt];
}
void op_jr(R_PARAM)
{
	CURRENT_STATE.PC = CURRENT_STATE.REGS[rs];
}
void op_nor(R_PARAM)
{
	CURRENT_STATE.REGS[rd] = ~(CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt]);	
}
void op_or(R_PARAM)
{
	CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt];
}
void op_subu(R_PARAM)
{
	CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
}
void op_sltu(R_PARAM)
{
	CURRENT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] < CURRENT_STATE.REGS[rt];
}

//R-type operations with shift: (rd, rt, sa)

void op_sll(R_PARAM_SHIFT)
{
	CURRENT_STATE.REGS[rd] = (CURRENT_STATE.REGS[rt] << sa);
}
void op_srl(R_PARAM_SHIFT)
{
	CURRENT_STATE.REGS[rd] = (CURRENT_STATE.REGS[rt] >> sa);
}

//I-type operations: (rt, rs, imm)
void op_addiu(I_PARAM)
{
	CURRENT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + imm;
}
void op_beq(I_PARAM)
{
	if(CURRENT_STATE.REGS[rs] == CURRENT_STATE.REGS[rt])
	{
		CURRENT_STATE.PC += 4*imm;
	}
}
void op_bne(I_PARAM)
{
	if(CURRENT_STATE.REGS[rs] != CURRENT_STATE.REGS[rt])
	{
		CURRENT_STATE.PC += 4*imm;
	}
}
void op_lw(I_PARAM)
{
	CURRENT_STATE.REGS[rt] = mem_read_32(rs + imm);
}
void op_sltiu(I_PARAM)
{
	CURRENT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] < imm;
}
void op_sw(I_PARAM)
{
	mem_write_32(rs + imm, CURRENT_STATE.REGS[rt]);
}
void op_lui(I_PARAM)
{
	CURRENT_STATE.REGS[rt] = imm << 16;
}
void op_ori(I_PARAM)
{
	CURRENT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] | imm;
}

//J-type operations: (target)
void op_j(J_PARAM)
{
	CURRENT_STATE.PC &= 0xF0000000;
	CURRENT_STATE.PC |= target * 4;
}
void op_jal(J_PARAM)
{
	CURRENT_STATE.REG[31] = CURRENT_STATE.PC + 4;
	CURRENT_STATE.PC &= 0xF0000000;
	CURRENT_STATE.PC |= target * 4;
}
