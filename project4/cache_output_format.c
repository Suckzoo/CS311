#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "cache.h"

#define BYTES_PER_WORD 4

int capacity = 256;
int way = 4;
int blocksize = 8;
cache_element** cache;
uint32_t cache_counter;
int set;
int words;
int total_reads = 0;
int total_writes = 0;
int write_backs = 0;
int reads_hits = 0;
int write_hits = 0;
int reads_misses = 0;
int write_misses = 0;

/***************************************************************/
/*                                                             */
/* Procedure : cdump                                           */
/*                                                             */
/* Purpose   : Dump cache configuration                        */   
/*                                                             */
/***************************************************************/
void cdump(int capacity, int assoc, int blocksize){

	printf("Cache Configuration:\n");
	printf("-------------------------------------\n");
	printf("Capacity: %dB\n", capacity);
	printf("Associativity: %dway\n", assoc);
	printf("Block Size: %dB\n", blocksize);
	printf("\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : sdump                                           */
/*                                                             */
/* Purpose   : Dump cache stat		                       */   
/*                                                             */
/***************************************************************/
void sdump(int p1, int p2, int p3,
		int p4, int p5, int p6, int p7) {
	printf("Cache Stat:\n");
	printf("-------------------------------------\n");
	printf("Total reads: %d\n", p1);
	printf("Total writes: %d\n", p2);
	printf("Write-backs: %d\n", p3);
	printf("Read hits: %d\n", p4);
	printf("Write hits: %d\n", p5);
	printf("Read misses: %d\n", p6);
	printf("Write misses: %d\n", p7);
	printf("\n");
}


/***************************************************************/
/*                                                             */
/* Procedure : xdump                                           */
/*                                                             */
/* Purpose   : Dump current cache state                        */ 
/* 							       */
/* Cache Design						       */
/*  							       */
/* 	    cache[set][assoc][word per block]		       */
/*      						       */
/*      						       */
/*       ----------------------------------------	       */
/*       I        I  way0  I  way1  I  way2  I                 */
/*       ----------------------------------------              */
/*       I        I  word0 I  word0 I  word0 I                 */
/*       I  set0  I  word1 I  word1 I  work1 I                 */
/*       I        I  word2 I  word2 I  word2 I                 */
/*       I        I  word3 I  word3 I  word3 I                 */
/*       ----------------------------------------              */
/*       I        I  word0 I  word0 I  word0 I                 */
/*       I  set1  I  word1 I  word1 I  work1 I                 */
/*       I        I  word2 I  word2 I  word2 I                 */
/*       I        I  word3 I  word3 I  word3 I                 */
/*       ----------------------------------------              */
/*      						       */
/*                                                             */
/***************************************************************/
void xdump()
{
	int i,j,k = 0;

	printf("Cache Content:\n");
	printf("-------------------------------------\n");
	for(i = 0; i < way; i++)
	{
		if(i == 0)
		{
			printf("    ");
		}
		printf("      WAY[%d]",i);
	}
	printf("\n");

	for(i = 0 ; i < set;i++)
	{
		printf("SET[%d]:   ",i);
		for(j = 0; j < way;j++)
		{
			if(k != 0 && j == 0)
			{
				printf("          ");
			}
			printf("0x%08x  ", cache[i][j].addr);
		}
		printf("\n");
	}
	printf("\n");
}

void init_cache()
{
	int i, j;
	// allocate
	cache = (cache_element**) malloc (sizeof(cache_element*) * set);
	for(i = 0; i < set; i++) {
		cache[i] = (cache_element*) malloc(sizeof(cache_element) * way);
	}
	for(i = 0; i < set; i++) {
		for(j = 0; j < way; j ++) 
		{
			cache[i][j].dirty = 0;
			cache[i][j].valid = 0;
			cache[i][j].usage = 0;
			cache[i][j].tag = 0;
			cache[i][j].addr = 0;
		}
	}
}

int main(int argc, char *argv[]) {                              

	uint32_t** cache;
	int i, j, k;	
	int x_flag = 0;

	FILE *input;
	
	for(i = 1; i < argc; i++)
	{
		for(j = 0; j < strlen(argv[i]); j++)
		{
			if(!strcmp(argv[i], "-x"))
			{
				x_flag = 1;
			}
			else if(!strcmp(argv[i], "-c"))
			{
				i++;
				sscanf(argv[i], "%d:%d:%d", &capacity, &way, &blocksize);
			}
			else
			{
				input = fopen(argv[i], "r");
			}
		}
	}

	set = capacity/way/blocksize;
	words = blocksize / BYTES_PER_WORD;	

	init_cache(cache, set, way);

	char mode;
	uint32_t address;
	while(fscanf(input, "%c %x\n", &mode, &address) != EOF) {
		execute_command(mode, address);
	}

	// test example
	cdump(capacity, way, blocksize);
	sdump(total_reads, total_writes, write_backs, reads_hits,
			write_hits, reads_misses, write_misses); 
	if(x_flag) xdump();

	return 0;
}
