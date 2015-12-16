#ifndef __CACHE__
#define __CACHE__
#include <stdint.h>
extern int capacity;
extern int way;
extern int blocksize;
extern int set;
extern int words;
typedef struct {
	uint8_t dirty;
	uint8_t valid;
	uint32_t usage;
	uint32_t tag;
	uint32_t addr;
} cache_element;
extern cache_element** cache;
extern uint32_t cache_counter;
extern int total_reads ;
extern int total_writes;
extern int write_backs ;
extern int reads_hits  ;
extern int write_hits  ;
extern int reads_misses;
extern int write_misses;
void execute_command(char x, uint32_t addr);
#endif
