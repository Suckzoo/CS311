/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   CS311 KAIST                                               */
/*   parse.c                                                   */
/*                                                             */
/***************************************************************/

#include <stdio.h>

#include "util.h"
#include "parse.h"

int text_size;
int data_size;

/**************************************************************/
/*                                                            */
/* Procedure : parsing_instr                                  */
/*                                                            */
/* Purpose   : parse binary format instrction and return the  */
/*             instrcution data                               */
/*                                                            */
/* Argument  : buffer - binary string for the instruction     */
/*             index  - order of the instruction              */
/*                                                            */
/**************************************************************/
instruction parsing_instr(const char *buffer, const int index) {
    instruction instr = {0, };

	/* wow such reference much refence */
	unsigned char& rs = instr.r_t.r_i.rs;
	unsigned char& rt = instr.r_t.r_i.rt;
	unsigned char& rd = instr.r_t.r_i.r_i.r.rd;
	unsigned char& shamt = instr.r_t.r_i.r_i.r.shamt;
	short& imm = instr.r_t.r_i.r_i.imm;
	uint32_t& target = instr.r_t.target;
	
    /* Your code */
	int i;
	int pt = 6;
	for(i=0;i<6;i++)
	{
		instr.opcode = instr.opcode << 1;
		instr.opcode |= (buffer[i]-'0');
	}
	if(instr.opcode == 0) // R-type
	{
		rs=0;
		for(i=0;i<5;i++,pt++)
		{
			rs <<=1;
			rs |= (buffer[pt] - '0');
		}
		rt=0;
		for(i=0;i<5;i++,pt++)
		{
			rt <<= 1; 
			rt |= (buffer[pt] - '0');
		}
		rd=0;
		for(i=0;i<5;i++,pt++)
		{
			rd <<= 1;
			rd |= (buffer[pt] - '0');
		}
		shamt=0;
		for(i=0;i<5;i++,pt++)
		{
			shamt <<= 1;
			shamt |= (buffer[pt] - '0');
		}
		instr.func_code = 0;
		for(i=0;i<6;i++,pt++)
		{
			instr.func_code <<= 1;
			instr.func_code |= (buffer[pt] - '0');
		}
	}
	else if(instr.opcode == 2 || instr.opcode == 3)
	{
		target = 0;
		for(;pt<32;pt++)
		{
			target <<= 1;
			target |= (buffer[pt] - '0');
		}
	}
	else
	{
		rs=0;
		for(i=0;i<5;i++,pt++)
		{
			rs <<=1;
			rs |= (buffer[pt] - '0');
		}
		rt=0;
		for(i=0;i<5;i++,pt++)
		{
			rt <<= 1; 
			rt |= (buffer[pt] - '0');
		}
		imm=0;
		for(;pt<32;pt++)
		{
			imm <<= 1;
			imm |= (buffer[pt] - '0');			
		}
	}

    return instr;
}

/**************************************************************/
/*                                                            */
/* Procedure : parsing_data                                   */
/*                                                            */
/* Purpose   : parse binary format data and store data into   */
/*             the data region                                */
/*                                                            */
/* Argument  : buffer - binary string for data                */
/*             index  - order of data                         */
/*                                                            */
/**************************************************************/
void parsing_data(const char *buffer, const int index) {
    /* Your code */
	uint32_t word = 0;
	for(int k = 0; k < 4; k++)
	{
		uint8_t byte = 0; 
		for(int i = 8 * k + 0; i < 8 * k + 8; i++)
		{
			byte <<= 1;
			byte |= (buffer[i] - '0');
		}
		word |= (byte << (8 * (3-k)));
	}
	mem_write_32(MEM_DATA_START + index, word);
}
