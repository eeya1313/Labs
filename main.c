#include <stdio.h>
#include "oslabs.h"

typedef struct MEMORY_BLOCK MEMORY_BLOCK;

void print_memory(const struct MEMORY_BLOCK mb[MAPMAX])
{
	printf("\nSTART\tEND\tSIZE\tPID\n");
	for (int i = 0; i < MAPMAX; i++)
	{
		printf("%d\t", mb[i].start_address);
		printf("%d\t", mb[i].end_address);
		printf("%d\t", mb[i].segment_size);
		printf("%d\n", mb[i].process_id);
	}
}

int main(int argc, char **argv)
{
	MEMORY_BLOCK memory_map[MAPMAX] = {
		{64, 64 + 8 - 1, 8, 0}, //
		{72, 72 + 16 - 1, 16, 1},
		{88, 88 + 12 - 1, 12, 2},
		{100, 100 + 24 - 1, 24, 0}, 
		{124, 124 + 8 - 1, 8, 3},
		{132, 132 + 4 - 1, 4, 4},
		{136, 136 + 30 - 1, 30, 0},
		{166, 166 + 20 - 1, 20, 5},
		{186, 186 + 70 - 1, 70, 0}, 
	};
	int map_cnt = 9;

	print_memory(memory_map);

	first_fit_allocate_end(28, memory_map, &map_cnt, 6);

	first_fit_allocate_end(16, memory_map, &map_cnt, 7);

	release_memory((MEMORY_BLOCK){88, 88 + 12 - 1, 12, 2}, memory_map, &map_cnt);

	first_fit_allocate_end(16, memory_map, &map_cnt, 8);

	release_memory((MEMORY_BLOCK){166, 166 + 20 - 1, 20, 5}, memory_map, &map_cnt);

	first_fit_allocate_end(12, memory_map, &map_cnt, 9);

	first_fit_allocate_end(4, memory_map, &map_cnt, 10);

	print_memory(memory_map);

	return 0;
}
