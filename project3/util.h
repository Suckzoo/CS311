/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   CS311 KAIST                                               */
/*   util.h                                                    */
/*                                                             */
/***************************************************************/

#ifndef _UTIL_H_
#define _UTIL_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define FALSE 0
#define TRUE  1

/* Basic Information */
#define MEM_TEXT_START	0x00400000
#define MEM_TEXT_SIZE	0x00100000
#define MEM_DATA_START	0x10000000
#define MEM_DATA_SIZE	0x00100000
#define MIPS_REGS	32
#define BYTES_PER_WORD	4
#define PIPE_STAGE	5

typedef struct inst_s {
    short opcode;
    
    /*R-type*/
    short func_code;

    union {
        /* R-type or I-type: */
        struct {
        unsigned char rs;
        unsigned char rt;

        union {
            short imm;

            struct {
            unsigned char rd;
            unsigned char shamt;
        } r;
        } r_i;
    } r_i;
        /* J-type: */
        uint32_t target;
    } r_t;

    uint32_t value;
    
    //int32 encoding;
    //imm_expr *expr;
    //char *source_line;
} instruction;

// Register Design From 'Computer Organization And Design'(P&H) 5th edition
typedef struct if_id {
    uint32_t PC;
    uint32_t NPC;
    instruction instr;
} if_id_reg;

typedef struct id_ex {
    uint32_t PC;
    uint32_t NPC;

    uint32_t readV1;
    uint32_t readV2;

    unsigned char shamt;
    uint32_t imm;
    unsigned char rs;
    unsigned char rt;
    unsigned char rd;

    // c for 'Control'
    // EX Control
    char cALUSrc;
    char cALUOp;
    char cRegDst;

    // MEM Control
    char cMemWrt;
    char cMemRd;
    char cBranch;

    // WB Control
    char cMem2Reg;
    char cRegWrt;
} id_ex_reg;

typedef struct ex_mem {
    uint32_t PC;

    uint32_t ALUResult;
    uint32_t wrtData;
    unsigned char wrtReg;

    // MEM Control
    char cALUBranch;
    char cMemWrt;
    char cMemRd;
    char cBranch;

    // WB Control
    char cMem2Reg;
    char cRegWrt;
} ex_mem_reg;

typedef struct mem_wb
{
    uint32_t PC;

    uint32_t ALUResult;
    unsigned char wrtReg;
    uint32_t memV; // Read data from memory

    // WB Control
    char cMem2Reg;
    char cRegWrt;
} mem_wb_reg;

typedef struct CPU_State_Struct {
    uint32_t PC;        /* program counter */
    uint32_t REGS[MIPS_REGS];   /* register file */
    uint32_t PIPE[PIPE_STAGE];  /* pipeline stage */
    // Customize
    if_id_reg IF_ID;
    id_ex_reg ID_EX;
    ex_mem_reg EX_MEM;
    mem_wb_reg MEM_WB;
	int bubble_count; 
	int EX_bubble_count;
	int MEM_bubble_count;
	int IF_ID_flush_count;
    uint32_t branchhuh;
    uint32_t jumphuh;
    char pc_hold;
    char flushed;
} CPU_State;

typedef struct {
    uint32_t start, size;
    uint8_t *mem;
} mem_region_t;

/* For PC * Registers */
extern CPU_State CURRENT_STATE;

/* For Instructions */
extern instruction noop;
extern instruction *INST_INFO;
extern int NUM_INST;

/* For Memory Regions */
extern mem_region_t MEM_REGIONS[2];

/* For Execution */
extern int RUN_BIT;	/* run bit */
extern int INSTRUCTION_COUNT;

/* Functions */
char**		str_split(char *a_str, const char a_delim);
int		fromBinary(char *s);
uint32_t	mem_read_32(uint32_t address);
void		mem_write_32(uint32_t address, uint32_t value);
void		cycle();
void		run(int num_cycles);
void		go();
void		mdump(int start, int stop);
void		rdump();
void		init_memory();
void		init_inst_info();

/* YOU IMPLEMENT THIS FUNCTION */
void		process_instruction();

/* CUSTOMIZED FLAGS */
extern int nobp_set;
extern int noforward_set;
extern int num_inst;
extern int num_inst_set;
int run_i;
#endif
