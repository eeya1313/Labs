#include <limits.h>
#include "oslabs.h"

typedef struct MEMORY_BLOCK MEMORY_BLOCK;

void srink(struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, int index)
{
	for (int i = index; i < *map_cnt; i++)
	{
		memory_map[i - 1] = memory_map[i];
	}
	memory_map[--(*map_cnt)] = (MEMORY_BLOCK){0};
}

void expand(struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, int index)
{
	for (int i = *map_cnt; i >= index; i--)
	{
		memory_map[i + 1] = memory_map[i];
	}
	++(*map_cnt);
}

struct MEMORY_BLOCK best_fit_allocate(const int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, const int process_id)
{

	int value = INT_MAX;
	int index = -1;
	for (int i = 0; i < *map_cnt; i++)
	{
		int segment_size = memory_map[i].segment_size;
		if (memory_map[i].process_id == 0 &&
			segment_size >= request_size &&
			segment_size < value)
		{
			value = segment_size;
			index = i;
		}
	}
	if (index == -1)
		return (MEMORY_BLOCK){0};

	if (memory_map[index].segment_size > request_size)
	{
		MEMORY_BLOCK split = memory_map[index];
		split.segment_size -= request_size;
		split.start_address += request_size;
		expand(memory_map, map_cnt, index);
		memory_map[index + 1] = split;
	}
	memory_map[index].process_id = process_id;
	memory_map[index].segment_size = request_size;
	memory_map[index].end_address = memory_map[index].start_address + request_size - 1;

	return memory_map[index];
}

struct MEMORY_BLOCK first_fit_allocate(const int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, const int process_id)
{
	int index = -1;
	for (int i = 0; i < *map_cnt; i++)
	{
		if (memory_map[i].process_id == 0 &&
			memory_map[i].segment_size >= request_size)
		{
			index = i;
			break;
		}
	}
	if (index == -1)
		return (MEMORY_BLOCK){0};

	if (memory_map[index].segment_size > request_size)
	{

		MEMORY_BLOCK split = memory_map[index];
		split.segment_size -= request_size;
		split.start_address += request_size;
		expand(memory_map, map_cnt, index);
		memory_map[index + 1] = split;
	}

	memory_map[index].process_id = process_id;
	memory_map[index].segment_size = request_size;
	memory_map[index].end_address = memory_map[index].start_address + request_size - 1;

	return memory_map[index];
}

struct MEMORY_BLOCK first_fit_allocate_end(const int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, const int process_id)
{
	int index = -1;
	for (int i = 0; i < *map_cnt; i++)
	{
		if (memory_map[i].process_id == 0 &&
			memory_map[i].segment_size >= request_size)
		{
			index = i;
			break;
		}
	}
	if (index == -1)
		return (MEMORY_BLOCK){0};

	if (memory_map[index].segment_size > request_size)
	{

		MEMORY_BLOCK split = memory_map[index];
		split.segment_size -= request_size;
		split.end_address -= request_size;
		expand(memory_map, map_cnt, index);
		memory_map[index++] = split;
	}

	memory_map[index].process_id = process_id;
	memory_map[index].segment_size = request_size;
	memory_map[index].start_address = memory_map[index].end_address - request_size + 1;

	return memory_map[index];
}

struct MEMORY_BLOCK worst_fit_allocate(const int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, const int process_id)
{

	int value = 0;
	int index = -1;
	for (int i = 0; i < *map_cnt; i++)
	{
		int segment_size = memory_map[i].segment_size;
		if (memory_map[i].process_id == 0 &&
			segment_size >= request_size &&
			segment_size > value)
		{
			value = segment_size;
			index = i;
		}
	}
	if (index == -1)
		return (MEMORY_BLOCK){0};

	if (memory_map[index].segment_size > request_size)
	{
		MEMORY_BLOCK split = memory_map[index];
		split.segment_size -= request_size;
		split.start_address += request_size;
		memory_map[(*map_cnt)++] = split;
	}
	memory_map[index].process_id = process_id;
	memory_map[index].segment_size = request_size;
	memory_map[index].end_address = memory_map[index].start_address + request_size - 1;

	return memory_map[index];
}

struct MEMORY_BLOCK next_fit_allocate(const int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, const int process_id, const int last_address)
{

	int index = -1;
	int i = last_address;
	int cnt = *map_cnt;
	while (cnt-- > 0)
	{
		if (memory_map[i].process_id == 0 &&
			memory_map[i].segment_size >= request_size)
		{
			index = i;
			break;
		}
		if (++i >= *map_cnt)
			i = 0;
	}
	if (index == -1)
		return (MEMORY_BLOCK){0};

	if (memory_map[index].segment_size > request_size)
	{
		MEMORY_BLOCK split = memory_map[index];
		split.segment_size -= request_size;
		split.start_address += request_size;
		memory_map[(*map_cnt)++] = split;
	}
	memory_map[index].process_id = process_id;
	memory_map[index].segment_size = request_size;
	memory_map[index].end_address = memory_map[index].start_address + request_size - 1;

	return memory_map[index];
}

void release_memory(const struct MEMORY_BLOCK freed_block, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt)
{
	int index = -1;
	for (int i = 0; i < *map_cnt; i++)
	{
		if (memory_map[i].process_id == freed_block.process_id &&
			memory_map[i].start_address == freed_block.start_address)
		{
			index = i;
			break;
		}
	}
	if (index == -1)
		return;
	memory_map[index].process_id = 0;
	if (index + 1 < *map_cnt && memory_map[index + 1].process_id == 0)
	{
		memory_map[index].segment_size += memory_map[index + 1].segment_size;
		memory_map[index].end_address = memory_map[index + 1].end_address;
		for (int i = index + 2; i < *map_cnt; i++)
		{
			memory_map[i - 1] = memory_map[i];
		}
		memory_map[--(*map_cnt)] = (MEMORY_BLOCK){0};
	}
	if (index > 0 && memory_map[index - 1].process_id == 0)
	{
		memory_map[index - 1].segment_size += memory_map[index].segment_size;
		memory_map[index - 1].end_address = memory_map[index].end_address;
		for (int i = index + 1; i < *map_cnt; i++)
		{
			memory_map[i - 1] = memory_map[i];
		}
		memory_map[--(*map_cnt)] = (MEMORY_BLOCK){0};
	}
}
