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

    // default
    reg.PC = CURRENT_STATE.PC;
    reg.NPC = CURRENT_STATE.PC + BYTES_PER_WORD;
    reg.instr = INST_INFO[(CURRENT_STATE.PC - MEM_TEXT_START) >> 2];
    return reg
}

id_ex_reg run_ID(){
    id_ex_reg reg;
    if_id_mem IF_ID = CURRENT_STATE.IF_ID;

    instruction instr = IF_ID.instr;
    short op = instr.opcode;
    short func_code = instr.func_code;

    // fill register
    reg.NPC = IF_ID.NPC + ;
    reg.rs = instr.r_t.r_i.rs;
    reg.rt = instr.r_t.r_i.rt;
    reg.rd = instr.r_t.r_i.r_i.r.rd;

    reg.imm = instr.r_t.r_i.r_i.imm;
    if (op == 0x0c || op == 0x0d)
        reg.imm &= 0x0000ffff;
    reg.shamt = instr.r_t.r_i.r_i.r.shamt;

    reg.readV1 = CURRENT_STATE.REGS[reg.rs];
    reg.readV2 = CURRENT_STATE.REGS[reg.rt];


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
            break;
        case 0x05: // bne
            reg.cALUOp = 4; // minus with inv 0
            break;
        case 0x0b: // sltiu
            reg.cALUOp = 5;
            break;
        case 0x0f: // lui
            reg.cALUOp = 6;
            break;
        case 0x03 // jal
            reg.cALUOp = 7;
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
            else
                printf("INVALID FUNC CODE\n");
            break;
        default:
            printf("INVALID OP CODE\n");
    }

    // ALUSrc
}


/***************************************************************/
/*                                                             */
/* Procedure: process_instruction                              */
/*                                                             */
/* Purpose: Process one instrction                             */
/*                                                             */
/***************************************************************/
void process_instruction(){
    instruction *inst;
    int i;      // for loop

    /* pipeline */
    CURRENT_STATE.PIPE[0] = CURRENT_STATE.PC;
    CURRENT_STATE.PIPE[1] = CURRENT_STATE.IF_ID.PC;
    CURRENT_STATE.PIPE[2] = CURRENT_STATE.ID_EX.PC;
    CURRENT_STATE.PIPE[3] = CURRENT_STATE.EX_MEM.PC;
    CURRENT_STATE.PIPE[4] = CURRENT_STATE.MEM_WB.PC;


    MEM_WB = execute_MEM();
    execute_WB();
    EX_MEM = execute_EX();
    ID_EX  = execute_ID();
    IF_ID  = execute_IF();

    CURRENT_STATE.PC = PC;
    CURRENT_STATE.IF_ID = IF_ID;
    CURRENT_STATE.ID_EX = ID_EX;
    CURRENT_STATE.EX_MEM = EX_MEM;
    CURRENT_STATE.MEM_WB = MEM_WB;
    

    
    // fetch
    inst = get_inst_info(CURRENT_STATE.PC);
    CURRENT_STATE.PC += BYTES_PER_WORD;

    switch (OPCODE(inst))
    {
    case 0x9:       //(0x001001)ADDIU
        CURRENT_STATE.REGS[RT (inst)] = CURRENT_STATE.REGS[RS (inst)] + (short) IMM (inst);
        break;
    case 0xc:       //(0x001100)ANDI
        CURRENT_STATE.REGS[RT (inst)] = CURRENT_STATE.REGS[RS (inst)] & (0xffff & IMM (inst));
        break;
    case 0xf:       //(0x001111)LUI 
        CURRENT_STATE.REGS[RT (inst)] = (IMM (inst) << 16) & 0xffff0000;
        break;
    case 0xd:       //(0x001101)ORI
        CURRENT_STATE.REGS[RT (inst)] = CURRENT_STATE.REGS[RS (inst)] | (0xffff & IMM (inst));
        break;
    case 0xb:       //(0x001011)SLTIU 
        {
        int x = (short) IMM (inst);

        if ((uint32_t) CURRENT_STATE.REGS[RS (inst)] < (uint32_t) x)
            CURRENT_STATE.REGS[RT (inst)] = 1;
        else
            CURRENT_STATE.REGS[RT (inst)] = 0;
        break;
        }
    case 0x23:      //(0x100011)LW  
        LOAD_INST (&CURRENT_STATE.REGS[RT (inst)], mem_read_32((CURRENT_STATE.REGS[BASE (inst)] + IOFFSET (inst))), 0xffffffff);
        break;
    case 0x2b:      //(0x101011)SW
        mem_write_32(CURRENT_STATE.REGS[BASE (inst)] + IOFFSET (inst), CURRENT_STATE.REGS[RT (inst)]);
        break;
    case 0x4:       //(0x000100)BEQ
        BRANCH_INST (CURRENT_STATE.REGS[RS (inst)] == CURRENT_STATE.REGS[RT (inst)], CURRENT_STATE.PC + IDISP (inst), 0);
        break;
    case 0x5:       //(0x000101)BNE
        BRANCH_INST (CURRENT_STATE.REGS[RS (inst)] != CURRENT_STATE.REGS[RT (inst)], CURRENT_STATE.PC + IDISP (inst), 0);
        break;

    case 0x0:       //(0x000000)ADDU, AND, NOR, OR, SLTU, SLL, SRL, SUBU  if JR
        {
        switch(FUNC (inst)){
            case 0x21:  //ADDU
            CURRENT_STATE.REGS[RD (inst)] = CURRENT_STATE.REGS[RS (inst)] + CURRENT_STATE.REGS[RT (inst)];
            break;
            case 0x24:  //AND
            CURRENT_STATE.REGS[RD (inst)] = CURRENT_STATE.REGS[RS (inst)] & CURRENT_STATE.REGS[RT (inst)];
            break;
            case 0x27:  //NOR
            CURRENT_STATE.REGS[RD (inst)] = ~ (CURRENT_STATE.REGS[RS (inst)] | CURRENT_STATE.REGS[RT (inst)]);
            break;
            case 0x25:  //OR
            CURRENT_STATE.REGS[RD (inst)] = CURRENT_STATE.REGS[RS (inst)] | CURRENT_STATE.REGS[RT (inst)];
            break;
            case 0x2B:  //SLTU
            if ( CURRENT_STATE.REGS[RS (inst)] <  CURRENT_STATE.REGS[RT (inst)])
                CURRENT_STATE.REGS[RD (inst)] = 1;
            else
                CURRENT_STATE.REGS[RD (inst)] = 0;
            break;
            case 0x0:   //SLL
            {
                int shamt = SHAMT (inst);

                if (shamt >= 0 && shamt < 32)
                CURRENT_STATE.REGS[RD (inst)] = CURRENT_STATE.REGS[RT (inst)] << shamt;
                else
                CURRENT_STATE.REGS[RD (inst)] = CURRENT_STATE.REGS[RT (inst)];
                break;
            }
            case 0x2:   //SRL
            {
                int shamt = SHAMT (inst);
                uint32_t val = CURRENT_STATE.REGS[RT (inst)];

                if (shamt >= 0 && shamt < 32)
                CURRENT_STATE.REGS[RD (inst)] = val >> shamt;
                else
                CURRENT_STATE.REGS[RD (inst)] = val;
                break;
            }
            case 0x23:  //SUBU
            CURRENT_STATE.REGS[RD(inst)] = CURRENT_STATE.REGS[RS(inst)]-CURRENT_STATE.REGS[RT(inst)];
            break;

            case 0x8:   //JR
            {
                uint32_t tmp = CURRENT_STATE.REGS[RS (inst)];
                JUMP_INST (tmp);

                break;
            }
            default:
            printf("Unknown function code type: %d\n", FUNC(inst));
            break;
        }
        }
        break;

    case 0x2:       //(0x000010)J
        JUMP_INST (((CURRENT_STATE.PC & 0xf0000000) | TARGET (inst) << 2));
        break;
    case 0x3:       //(0x000011)JAL
        CURRENT_STATE.REGS[31] = CURRENT_STATE.PC;
        JUMP_INST (((CURRENT_STATE.PC & 0xf0000000) | (TARGET (inst) << 2)));
        break;

    default:
        printf("Unknown instruction type: %d\n", OPCODE(inst));
        break;
    }
    */

    if (CURRENT_STATE.PC < MEM_REGIONS[0].start || CURRENT_STATE.PC >= (MEM_REGIONS[0].start + (NUM_INST * 4)))
    RUN_BIT = FALSE;
}
