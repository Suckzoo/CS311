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

extern int nobp_set;
extern int noforward_set;
extern int num_inst;
extern int num_inst_set;
extern int run_i;
char DEBUG = 0;
char start = 1;
/***************************************************************/
/*                                                             */
/* Procedure: get_inst_info                                    */
/*                                                             */
/* Purpose: Read insturction information                       */
/*                                                             */
/***************************************************************/
instruction* get_inst_info(uint32_t pc) 
{ 
	return &INST_INFO[(pc - MEM_TEXT_START) >> 2];
}

// FETCH
if_id_reg run_IF(){
	// TODO: Bubble & Flush
	if_id_reg reg;

	if ((CURRENT_STATE.PC < MEM_TEXT_START || CURRENT_STATE.PC >= (MEM_TEXT_START + (NUM_INST * 4)))) {
		memset(&reg, 0, sizeof(reg));
    	return reg;
    }
    
	// default
	reg.PC = CURRENT_STATE.PC;
	reg.NPC = CURRENT_STATE.PC + BYTES_PER_WORD;
	reg.instr = INST_INFO[(CURRENT_STATE.PC - MEM_TEXT_START) >> 2];
    if(DEBUG) printf("IF Stage PC : %x\n", reg.PC );
	return reg;
}

if_id_reg run_BUBBLE() {
    if(DEBUG) printf("Bubble\n");
	if_id_reg reg;
	memset(&reg, 0, sizeof(if_id_reg));
	return reg;
}

id_ex_reg run_ID(){
	// TODO bubble, hazard, flush, jump
	id_ex_reg reg;
	if_id_reg IF_ID = CURRENT_STATE.IF_ID;
	id_ex_reg ID_EX = CURRENT_STATE.ID_EX;
	ex_mem_reg EX_MEM = CURRENT_STATE.EX_MEM;
	instruction instr = IF_ID.instr;
	short op = instr.opcode;
	short func_code = instr.func_code;

	// fill register
	// TODO: NOOP
	
	reg.NPC = IF_ID.NPC;
	reg.rs = instr.r_t.r_i.rs;
	reg.rt = instr.r_t.r_i.rt;
	reg.rd = instr.r_t.r_i.r_i.r.rd;

	reg.imm = instr.r_t.r_i.r_i.imm;
	if (op == 0x0c || op == 0x0d)
		reg.imm &= 0x0000ffff;
	reg.shamt = instr.r_t.r_i.r_i.r.shamt;

	reg.readV1 = CURRENT_STATE.REGS[reg.rs];
	reg.readV2 = CURRENT_STATE.REGS[reg.rt];
    if(DEBUG) printf("ID Stage op : %x / rs : %d\n", op, reg.rs);
    CURRENT_STATE.jumphuh = 0;
    CURRENT_STATE.branchhuh = 0;

    if(noforward_set){
    	char a = 0,b = 0;
    	unsigned char ID_EX_target = ID_EX.cRegDst ? ID_EX.rd : ID_EX.rt;
		if (ID_EX.cRegWrt && ID_EX_target != 0 && (ID_EX_target == reg.rs))
	    	a = 2;
    	else if (EX_MEM.cRegWrt && EX_MEM.wrtReg != 0 && (EX_MEM.wrtReg == reg.rs))
	    	a = 1;
    
    	if (ID_EX.cRegWrt && ID_EX_target != 0 && (ID_EX_target == reg.rt))
	    	b = 2;
    	else if (EX_MEM.cRegWrt && EX_MEM.wrtReg != 0 && (EX_MEM.wrtReg == reg.rt))
	    	b = 1;
	    a = a>b?a:b;
	    CURRENT_STATE.EX_bubble_count = a;
	    if(a){
		    id_ex_reg rrr;
			memset(&rrr, 0, sizeof(rrr));
			return rrr;	
		}
	    // IF, ID stall max(a,b) cycle.
	    // this stage should return zero register
    }

	// control bit~
	switch(op){
		case 0x23: // lw
		case 0x2b: // sw
		case 0x09: // add
			reg.cALUOp = 0; // add
			break;
		case 0x0c: // andi
			reg.cALUOp = 1;
			break;
		case 0x0d: // ori
			reg.cALUOp = 2;
			break;
		case 0x04: // beq
			reg.cALUOp = 3; // subst.
			reg.NPC += (reg.imm<<2);
			if(!nobp_set)
            CURRENT_STATE.branchhuh = reg.NPC;
			break;
		case 0x05: // bne
			reg.cALUOp = 4; // bne?
			reg.NPC += (reg.imm<<2);
            if(!nobp_set)
            CURRENT_STATE.branchhuh = reg.NPC;
			break;
		case 0x0b: // sltiu
			reg.cALUOp = 5;
			break;
		case 0x0f: // lui
			reg.cALUOp = 6;
			break;
		case 0x03: // jal
			reg.cALUOp = 7;
			//CURRENT_STATE.REGS[31] = IF_ID.NPC;
			//TODO : take this to WB stage~
			reg.rd = 31;
			reg.imm = IF_ID.NPC;
			reg.NPC = (IF_ID.NPC & 0xF0000000);
			reg.NPC |= (instr.r_t.target << 2);
            CURRENT_STATE.jumphuh = reg.NPC;
			break;
		case 0x02: // j
			//TODO oh yeah
			reg.cALUOp = -1;
			reg.NPC = (IF_ID.NPC & 0xF0000000);
			reg.NPC |= (instr.r_t.target << 2);
            CURRENT_STATE.jumphuh = reg.NPC;
			break;
		case 0x00: // R
			if(func_code==0x00) // sll
				reg.cALUOp = 8;
			else if(func_code==0x02) // srl
				reg.cALUOp = 9;
			else if(func_code==0x21) // addu
				reg.cALUOp = 0;
			else if(func_code==0x23) // subu
				reg.cALUOp = 3;
			else if(func_code==0x24) // and
				reg.cALUOp = 1;
			else if(func_code==0x25) // or
				reg.cALUOp = 2;
			else if(func_code==0x27) // nor
				reg.cALUOp = 10;
			else if(func_code==0x2b) // sltu
				reg.cALUOp = 5;
			else if(func_code==0x8) // jr
			{
				reg.cALUOp = -1;
				reg.NPC = CURRENT_STATE.REGS[reg.rs];
                CURRENT_STATE.jumphuh = CURRENT_STATE.REGS[reg.rs];
			}
			else
				printf("INVALID FUNC CODE\n");
			break;
		default:
			printf("INVALID OP CODE\n");
	}

	// ALUSrc
	switch(op){
		case 0x04:
		case 0x05:
		case 0x09:
		case 0x0b:
		case 0x0c:
		case 0x0d:
		case 0x0f:
		case 0x23:
		case 0x2b: // addiu, sltiu, ori, andi, lui, lw, sw 
			reg.cALUSrc = 1;
			break;
		default:
			reg.cALUSrc = 0;
	}

	reg.cRegDst = (op == 0x00 || op == 0x03);
	reg.cBranch = (op == 0x04 || op == 0x05);
    reg.cRegWrt = (op == 0x00 || op == 0x09 || op == 0x0c || 
           op == 0x0f || op == 0x0d || op == 0x0b || op == 0x23 || op == 0x03);
	reg.cMem2Reg = (op == 0x23);
	reg.cMemRd = (op == 0x23);
	reg.cMemWrt = (op == 0x2b);

	reg.PC = IF_ID.PC;
	return reg;
}

ex_mem_reg run_EX()
{
	id_ex_reg ID_EX = CURRENT_STATE.ID_EX;
	ex_mem_reg reg;
	unsigned char rs = ID_EX.rs;
	unsigned char rt = ID_EX.rt;
	unsigned char shamt = ID_EX.shamt;
	uint32_t readV1, readV2;
	readV1 = ID_EX.readV1;
	readV2 = ID_EX.readV2;
	uint32_t imm = ID_EX.imm;
	ex_mem_reg EX_MEM = CURRENT_STATE.EX_MEM;
	mem_wb_reg MEM_WB = CURRENT_STATE.MEM_WB;
	//TODO: consider Load-Use hazard!
  if(EX_MEM.cRegWrt && (EX_MEM.wrtReg != 0) && (EX_MEM.wrtReg == ID_EX.rs))
	{  
		// if(noforward_set)
		// {
		// 	printf("PC : %x makes EX bubble\n", ID_EX.PC);
		// 	CURRENT_STATE.EX_bubble_count = 2;
		// 	memset(&reg, 0, sizeof(reg));
		// 	return reg;
		// }
		// else
			readV1 = EX_MEM.ALUResult;	
	}
	else if(MEM_WB.cRegWrt && (MEM_WB.wrtReg != 0) && (MEM_WB.wrtReg == ID_EX.rs))
	{
		// if(noforward_set)
		// {
		// 	CURRENT_STATE.EX_bubble_count = 1;
		// 	memset(&reg, 0, sizeof(reg));
		// 	return reg;
		// }
		// else
			readV1 = MEM_WB.ALUResult;		
	}
	if(EX_MEM.cRegWrt && (EX_MEM.wrtReg != 0) && (EX_MEM.wrtReg == ID_EX.rt))
	{
		// if(noforward_set)
		// {
		// 	CURRENT_STATE.EX_bubble_count = 2;
		// 	memset(&reg, 0, sizeof(reg));
		// 	return reg;
		// }
		// else
			readV2 = EX_MEM.ALUResult;		
	}
	else if(MEM_WB.cRegWrt && (MEM_WB.wrtReg != 0) && (MEM_WB.wrtReg == ID_EX.rt))
	{
		// if(noforward_set)
		// {
		// 	CURRENT_STATE.EX_bubble_count = 1;
		// 	memset(&reg, 0, sizeof(reg));
		// 	return reg;
		// }
		// else
			readV2 = MEM_WB.ALUResult;		
	}
	switch(ID_EX.cALUOp)
	{
		case 0: // add, lw, sw
			if(ID_EX.cALUSrc) // addi
			{
				reg.ALUResult = readV1 + imm;
			}
			else // add
			{
				reg.ALUResult = readV1 + readV2;
			}
			break;
		case 1: // and
			if(ID_EX.cALUSrc)
			{
				reg.ALUResult = readV1 & imm;
			}
			else
			{
				reg.ALUResult = readV1 & readV2;
			}
			break;
		case 2: // ori
			if(ID_EX.cALUSrc)
			{
				reg.ALUResult = readV1 | imm;
			}
			else
			{
				reg.ALUResult = readV1 | readV2;
			}
			break;
		case 3: // subu, beq
			reg.ALUResult = readV1 - readV2;
			reg.cALUBranch = !reg.ALUResult;
			break;
		case 4: // bne?
			reg.ALUResult = readV1 - readV2;
			reg.cALUBranch = !!reg.ALUResult;			
			break;
		case 5: // sltiu, sltu
			if(ID_EX.cALUSrc)
			{
				reg.ALUResult = readV1 < imm;
			}
			else
			{
				reg.ALUResult = readV1 < readV2;
			}
			break;
		case 6: // lui
			reg.ALUResult = imm << 16;
			break;
		case 7: // jal
			reg.ALUResult = imm;
			break;
		case 8: // sll
			reg.ALUResult = readV2 << shamt;
			break;
		case 9: // srl
			reg.ALUResult = readV2 >> shamt;
			break;
		case 10: // nor
			reg.ALUResult = ~(readV1 | readV2);
			break;
		default:
			break;
	};
	reg.PC = ID_EX.PC;
	reg.cMemWrt = ID_EX.cMemWrt;
	reg.cMemRd = ID_EX.cMemRd;
	reg.cBranch = ID_EX.cBranch;

	reg.cMem2Reg = ID_EX.cMem2Reg;
	reg.cRegWrt = ID_EX.cRegWrt;

	//wrtData: write data ($t) for sw
	//wrtReg: storing register!
	reg.wrtData = ID_EX.readV2;
	reg.wrtReg = ID_EX.cRegDst?ID_EX.rd:ID_EX.rt;

	if(ID_EX.cBranch && !reg.cALUBranch)
	{
		// if(nobp_set)
		// 	// CURRENT_STATE.PC = reg.PC + 4;
		if(!nobp_set){
			CURRENT_STATE.IF_ID_flush_count = 1;
            if(DEBUG) printf("TIME TO FLUSH@@@@@@\n");
        }
       	else{
       		CURRENT_STATE.branchhuh = reg.PC + 4;
       	}
	}
	else if(ID_EX.cBranch && reg.cALUBranch && nobp_set)
	{
		CURRENT_STATE.branchhuh = reg.PC + 4 + (imm<<2);
	}
    if(DEBUG) printf("EX Stage Result : %x/readV1 : %x, imm : %x\n", reg.ALUResult, readV1, imm);

	return reg;
}

mem_wb_reg run_MEM()
{
	ex_mem_reg EX_MEM = CURRENT_STATE.EX_MEM;
	mem_wb_reg reg;
	mem_wb_reg MEM_WB = CURRENT_STATE.MEM_WB;
	//handling lw, sw
	if(EX_MEM.cMemRd)
	{
		//TODO: read from memory!(hazardous!)
		if(DEBUG) printf("EX_MEM ALU: %d\n",EX_MEM.ALUResult);
		reg.memV = mem_read_32(EX_MEM.ALUResult);
		if(DEBUG) printf("reg.memV: %d\n",reg.memV);
	}
	if(EX_MEM.cMemWrt)
	{
		//TODO: write to memory!
		if(MEM_WB.cRegWrt && EX_MEM.cMemWrt && MEM_WB.wrtReg == EX_MEM.wrtReg)
		{
			if(noforward_set)
			{
				CURRENT_STATE.MEM_bubble_count = 1;
				memset(&reg, 0, sizeof(reg));
				return reg;
			}
			mem_write_32(EX_MEM.ALUResult, MEM_WB.memV);
		}
		else
			mem_write_32(EX_MEM.ALUResult, EX_MEM.wrtData);
	}
	reg.PC = EX_MEM.PC;
	reg.ALUResult = EX_MEM.ALUResult;
	reg.wrtReg = EX_MEM.wrtReg;
	reg.cMem2Reg = EX_MEM.cMem2Reg;
	reg.cRegWrt = EX_MEM.cRegWrt;
	return reg;
}

void run_WB()
{
	mem_wb_reg MEM_WB = CURRENT_STATE.MEM_WB;
	if(MEM_WB.cMem2Reg)
    {
        if(DEBUG) printf("Write back mem Result! : %x\n", MEM_WB.memV);
        CURRENT_STATE.REGS[MEM_WB.wrtReg] = MEM_WB.memV;
    }
    else if(MEM_WB.cRegWrt)
	{
        if(DEBUG) printf("Write back ALU Result! : %x\n", MEM_WB.ALUResult);
		CURRENT_STATE.REGS[MEM_WB.wrtReg] = MEM_WB.ALUResult;
	}
	
	num_inst--;
	run_i++;
}

/***************************************************************/
/*                                                             */
/* Procedure: process_instruction                              */
/*                                                             */
/* Purpose: Process one instrction                             */
/*                                                             */
/***************************************************************/
uint32_t get_PC(){
    uint32_t ret = CURRENT_STATE.PC;
    if (CURRENT_STATE.PC < MEM_TEXT_START || CURRENT_STATE.PC >= (MEM_TEXT_START + (NUM_INST * 4)))
        return ret;
    
    if (CURRENT_STATE.pc_hold && !CURRENT_STATE.branchhuh) {
        CURRENT_STATE.pc_hold = 0;
        return ret;
    }

    if (CURRENT_STATE.flushed){
        if(DEBUG) printf("###PMEM_PC : %x\n", CURRENT_STATE.EX_MEM.PC);
        ret = CURRENT_STATE.MEM_WB.PC + 4;
        CURRENT_STATE.flushed = 0;
    }
    else if (CURRENT_STATE.branchhuh){
        ret = CURRENT_STATE.branchhuh;
        CURRENT_STATE.branchhuh = 0;
    }
    else if (CURRENT_STATE.jumphuh){
        ret = CURRENT_STATE.jumphuh;
        CURRENT_STATE.jumphuh = 0;
    }
    else{
        ret += 4;
    }
    return ret;
}

void process_instruction(){
	instruction *inst;
	int i;      // for loop

	/* pipeline */
	CURRENT_STATE.PIPE[0] = CURRENT_STATE.PC;
	CURRENT_STATE.PIPE[1] = CURRENT_STATE.IF_ID.PC;
	CURRENT_STATE.PIPE[2] = CURRENT_STATE.ID_EX.PC;
	CURRENT_STATE.PIPE[3] = CURRENT_STATE.EX_MEM.PC;
	CURRENT_STATE.PIPE[4] = CURRENT_STATE.MEM_WB.PC;

	mem_wb_reg MEM_WB;
	ex_mem_reg EX_MEM; 
	id_ex_reg ID_EX;
	memset(&MEM_WB, 0, sizeof(mem_wb_reg));
	memset(&EX_MEM, 0, sizeof(ex_mem_reg));
	memset(&ID_EX, 0, sizeof(id_ex_reg));
	if(CURRENT_STATE.PIPE[4]) run_WB();
	if(CURRENT_STATE.PIPE[3]) MEM_WB = run_MEM();
	if(CURRENT_STATE.MEM_bubble_count)
	{
		if(DEBUG) printf("MEM BUBBLE\n");
		CURRENT_STATE.MEM_bubble_count--;
		CURRENT_STATE.pc_hold = 1;
	}
    else if(CURRENT_STATE.IF_ID_flush_count)
    {
        CURRENT_STATE.MEM_WB = MEM_WB;
        ex_mem_reg rr;
        memset(&rr, 0, sizeof(rr));
        CURRENT_STATE.EX_MEM = rr;
        id_ex_reg rrr;
        memset(&rrr, 0, sizeof(rrr));
        CURRENT_STATE.ID_EX = rrr;
        CURRENT_STATE.IF_ID = run_BUBBLE();
        CURRENT_STATE.IF_ID_flush_count = 0;
        CURRENT_STATE.flushed = 1;
    }
	else
	{
		if(CURRENT_STATE.PIPE[2]) EX_MEM = run_EX();
		// if(CURRENT_STATE.EX_bubble_count)
		// {
		// 	if(DEBUG) printf("EX BUBBLE\n");
		// 	CURRENT_STATE.MEM_WB = MEM_WB;
		// 	CURRENT_STATE.EX_bubble_count--;
		// 	CURRENT_STATE.pc_hold = 1;
		// }
        // 얘가 여기 있으면 안됨. 파이프라인은 다 같이 도는거기 때문에 이번 결과를 적용시켜버리면 안되지.
		// else if(CURRENT_STATE.IF_ID_flush_count)
		// {
		// 	CURRENT_STATE.MEM_WB = MEM_WB;
		// 	CURRENT_STATE.EX_MEM = EX_MEM;
		// 	id_ex_reg rrr;
		// 	memset(&rrr, 0, sizeof(rrr));
		// 	CURRENT_STATE.ID_EX = rrr;
		// 	CURRENT_STATE.IF_ID = run_BUBBLE();
		// 	CURRENT_STATE.IF_ID_flush_count = 0;
  //           CURRENT_STATE.flushed = 1;
		// }
	
		if(CURRENT_STATE.PIPE[1]) {
			ID_EX  = run_ID();
			// CURRENT_STATE.PC = ID_EX.NPC;
		}
		if_id_reg IF_ID = run_IF();
		// 포워드 없을때 기다리는거! EX버블은 아닙니다!
		if(CURRENT_STATE.EX_bubble_count){
			CURRENT_STATE.MEM_WB = MEM_WB;
			CURRENT_STATE.EX_MEM = EX_MEM;
			CURRENT_STATE.ID_EX = ID_EX;
			CURRENT_STATE.pc_hold = 1;
			CURRENT_STATE.EX_bubble_count--;
		}
		else{
	        // branch가 taken되면 IF_ID에 들어가있는걸 비운다!
	        if(!nobp_set && CURRENT_STATE.branchhuh){
	            IF_ID = run_BUBBLE();
	        }
			if(CURRENT_STATE.bubble_count)
			{
	            if(DEBUG) printf("bubble1\n");
	            CURRENT_STATE.pc_hold = 1;
				IF_ID = run_BUBBLE();
				CURRENT_STATE.bubble_count--;
			}
			if(ID_EX.cMemRd 
					&& (IF_ID.instr.r_t.r_i.rs == ID_EX.rt 
						|| IF_ID.instr.r_t.r_i.rs == ID_EX.rt)) // Load-use XXX 포워드 없을땐?
																// IF에 잡아두면 안되네 밀어줘야하네!
			{
	            CURRENT_STATE.pc_hold = 1;
	            if(DEBUG) printf("bubble2: load-use\n");
				IF_ID = run_BUBBLE();
			}	
			if(ID_EX.cALUOp == 7 || ID_EX.cALUOp == -1) // Jump
			{
	            // CURRENT_STATE.pc_hold = 1;
	            if(DEBUG) printf("bubble3\n");
				IF_ID = run_BUBBLE();
			}
			// if(nobp_set && (ID_EX.cALUOp == 3 || ID_EX.cALUOp == 4)) // nobp!
			if(nobp_set && (ID_EX.cBranch))
			{
	            if(DEBUG) printf("bubble4\n");
				IF_ID = run_BUBBLE();
				CURRENT_STATE.bubble_count = 2;
			}

	        // bubble + flush..? 


			//TODO: what if branch fucked up?
			CURRENT_STATE.IF_ID = IF_ID;
			CURRENT_STATE.ID_EX = ID_EX;
			CURRENT_STATE.EX_MEM = EX_MEM;
			CURRENT_STATE.MEM_WB = MEM_WB;
		}
	}
	CURRENT_STATE.PC = get_PC();
	// printf("PC : %x, end point : %x\n", CURRENT_STATE.PC, MEM_REGIONS[0].start + (NUM_INST * 4) );
	if (CURRENT_STATE.PC < MEM_REGIONS[0].start || 
		( CURRENT_STATE.PC >= (MEM_REGIONS[0].start + (NUM_INST * 4)) &&
			!CURRENT_STATE.IF_ID.PC&&!CURRENT_STATE.ID_EX.PC&&
			!CURRENT_STATE.EX_MEM.PC&&!CURRENT_STATE.MEM_WB.PC)
        || (num_inst_set && !num_inst))
		RUN_BIT = FALSE;
	else
	{
		//otherwise, why does it continue?
		//int ii;
		//for(ii=0;ii<5;ii++)
		//{
		//	//if(CURRENT_STATE.PIPE[ii]) break;
		//	printf("PIPE[%d]: %x\n",ii, CURRENT_STATE.PIPE[ii]);
		//}
		//if(1)
		//{
		//	printf("it's because: \n");
		//	printf("CURRENT_STATE.PC < MEM_REGIONS[0].start: %d\n",
		//			CURRENT_STATE.PC < MEM_REGIONS[0].start);
		//	printf("CURRENT_STATE.PC >= (MEM_REGIONS[0].start + (NUM_INST * 4)): %d\n",
		//			CURRENT_STATE.PC >= (MEM_REGIONS[0].start + (NUM_INST * 4)));
		//	printf("!CURRENT_STATE.IF_ID.PC: %d\n",
		//			!CURRENT_STATE.IF_ID.PC);
		//	printf("!CURRENT_STATE.ID_EX.PC: %d\n",
		//			!CURRENT_STATE.ID_EX.PC);
		//	printf("!CURRENT_STATE.EX_MEM.PC: %d\n",
		//			!CURRENT_STATE.EX_MEM.PC);
		//	printf("!CURRENT_STATE.MEM_WB.PC: %d\n",
		//			!CURRENT_STATE.MEM_WB.PC);
		//	printf("num_inst_set: %d\n",
		//			num_inst_set);
		//	printf("!num_inst: %d\n",
		//			!num_inst);
		//}
	}
}
