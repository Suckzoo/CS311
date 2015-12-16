#include "cache.h"

cache_element** cache;
uint32_t cache_counter = 1;
int total_reads ;
int total_writes;
int write_backs ;
int reads_hits  ;
int write_hits  ;
int reads_misses;
int write_misses;
int set;
int words;
int way;
int blocksize;
void read(uint32_t addr)
{
	total_reads++;
	uint32_t tag = addr/set/blocksize;
	uint32_t index = ((addr/blocksize) & (set - 1));
	uint32_t offset = (addr & (blocksize - 1));
	uint32_t addr_align = (addr / blocksize) * blocksize;
	int found = 0;
	int i;
	for(i = 0; i < way; i++)
	{
		if(!cache[index][i].valid)
		{
			continue;
		}
		else
		{
			if(cache[index][i].tag == tag)
			{
				reads_hits++;
				found = 1;
				cache[index][i].usage = ++cache_counter;
				break;
			}
		}
	}
	if(!found)
	{
		reads_misses++;
		int evict_index = 0;
		for(i = 0; i < way; i++)
		{
			if(!cache[index][i].valid)
			{
				evict_index = i;
				break;
			}
			else if(cache[index][i].usage < cache[index][evict_index].usage)
			{
				evict_index = i;
			}
		}
		//evict evect_index block!
		if(cache[index][evict_index].dirty)
		{
			write_backs++;
		}
		cache[index][evict_index].dirty = 0;
		cache[index][evict_index].valid = 1;
		cache[index][evict_index].usage = ++cache_counter;
		cache[index][evict_index].tag = tag;
		cache[index][evict_index].addr = addr_align;
	}
}
void write(uint32_t addr)
{
	total_writes++;
	uint32_t tag = addr/set/blocksize;
	uint32_t index = ((addr/blocksize) & (set - 1));
	uint32_t offset = (addr & (blocksize - 1));
	uint32_t addr_align = (addr / blocksize) * blocksize;
	int found = 0;
	int i;
	for(i = 0; i < way; i++)
	{
		if(!cache[index][i].valid)
		{
			continue;
		}
		else
		{
			if(cache[index][i].tag == tag)
			{
				write_hits++;
				cache[index][i].dirty = 1;
				cache[index][i].usage = ++cache_counter;
				found = 1;
				break;
			}
		}
	}
	if(!found)
	{
		write_misses++;
		int evict_index = 0;
		for(i = 0; i < way; i++)
		{
			if(!cache[index][i].valid)
			{
				evict_index = i;
				break;
			}
			else if(cache[index][i].usage < cache[index][evict_index].usage)
			{
				evict_index = i;
			}
		}
		//evict evect_index block!
		if(cache[index][evict_index].dirty)
		{
			write_backs++;
		}
		cache[index][evict_index].dirty = 1;
		cache[index][evict_index].valid = 1;
		cache[index][evict_index].usage = ++cache_counter;
		cache[index][evict_index].tag = tag;
		cache[index][evict_index].addr = addr_align;
	}
}
void execute_command(char x, uint32_t addr)
{
	if(x == 'R')
	{
		read(addr);
	}
	else if(x == 'W')
	{
		write(addr);
	}
}
