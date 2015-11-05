/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   CS311 KAIST                                               */
/*   run.c                                                     */
/*                                                             */
/***************************************************************/

#include <stdio.h>

#include "util.h"
#include "run.h"
#include "operation.h"

/***************************************************************/
/*                                                             */
/* Procedure: process_instruction                              */
/*                                                             */
/* Purpose: Process one instrction                             */
/*                                                             */
/***************************************************************/
void process_instruction(){

	//1. Fetch instruction
	instruction instr = INST_INFO[(CURRENT_STATE.PC - MEM_TEXT_START)/4];
	//printf("opcode: %x\n",instr.opcode);
	//2. PC update
	CURRENT_STATE.PC += 4;
	//3. Perform operation
	//3-1. R-type running
	if(instr.opcode == 0x0)
	{
		unsigned char rd, rs, rt, shamt;
		shamt = instr.r_t.r_i.r_i.r.shamt;
		rd = instr.r_t.r_i.r_i.r.rd;
		rs = instr.r_t.r_i.rs;
		rt = instr.r_t.r_i.rt;
		void (*operation)(unsigned char, unsigned char, unsigned char);
		if(instr.r_t.r_i.r_i.r.shamt == 0)
		{
			switch(instr.func_code)
			{
				case 0x21:
					operation = op_addu;
					break;
				case 0x24:
					operation = op_and;
					break;
				case 0x8:
					operation = op_jr;
					break;
				case 0x27:
					operation = op_nor;
					break;
				case 0x25:
					operation = op_or;
					break;
				case 0x23:
					operation = op_subu;
					break;
				case 0x2B:
					operation = op_sltu;
					break;
			};
			operation(rd, rs, rt);
		}
		else
		{
			if(instr.func_code == 0) operation = op_sll;
			else op_srl;
			operation(rd, rt, shamt);
		}
	}
	//3-2. J-type running
	else if(instr.opcode == 0x2 || instr.opcode == 0x3)
	{
		void (*operation)(uint32_t);
		uint32_t target = instr.r_t.target;
		if(instr.opcode == 0x2)
		{
			op_j(target);
		}
		else
		{
			op_jal(target);
		}
	}
	//3-3. I-type running
	else
	{
		void (*operation)(unsigned char, unsigned char, short);
		unsigned char rt, rs;
		short imm;
		rt = instr.r_t.r_i.rt;
		rs = instr.r_t.r_i.rs;
		imm = instr.r_t.r_i.r_i.imm;
		switch(instr.opcode)
		{
			case 0x9:
				operation = op_addiu;
				break;
			case 0x4:
				operation = op_beq;
				break;
			case 0x5:
				operation = op_bne;
				break;
			case 0x23:
				operation = op_lw;
				break;
			case 0xB:
				operation = op_sltiu;
				break;
			case 0x2B:
				operation = op_sw;
				break;
			case 0xF:
				operation = op_lui;
				break;
			case 0xD:
				operation = op_ori;
				break;
			case 0xC:
				operation = op_andi;
				break;
		};
		operation(rt, rs, imm);
	}
	if(CURRENT_STATE.PC - MEM_TEXT_START == NUM_INST * 4) {
		RUN_BIT = false;
		return;
	}
}
