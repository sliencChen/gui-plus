/*
 * private_heap.c
 *
 *  Created on: 2015-5-18
 *      Author: Brant
 */

#include <gdi/gui_interface.h>
#include <guicore/private_heap.h>


BOOL heap_create(PrivateHeap *heap, unsigned int block_number, unsigned int block_size)
{
	gui_mutex_init(&(heap->mutex), "");
	heap->block_number = block_number;
	heap->block_size = block_size + sizeof(BlockHeader);
	heap->first_free_block = 0;
	heap->data = gui_malloc(heap->block_number * heap->block_size);
	if (!heap->data)
	{
		return FALSE;
	}
	gui_memset(heap->data, 0, heap->block_number * heap->block_size);

	return TRUE;
}

void* heap_alloc(PrivateHeap *heap)
{
	unsigned char *data = NULL;
	unsigned char *ret = NULL;
	unsigned int i;

	gui_mutex_lock(&(heap->mutex), GUI_MUTEX_WAIT_FOREVER);
	data = heap->data + heap->first_free_block * heap->block_size;
	for (i = heap->first_free_block; i < heap->block_number; i++)
	{
		if (*((BlockHeader *)data) == HEAP_BLOCK_FREE)
		{
			*((BlockHeader *)data) = HEAP_BLOCK_USED;
			heap->first_free_block = i + 1;
			ret = data + sizeof(BlockHeader);
			break;
		}
		data += heap->block_size;
	}
	gui_mutex_unlock(&(heap->mutex));
	return (void *)ret;
}

void heap_free(PrivateHeap *heap, void* data)
{
	int i;
	char *block;

	gui_mutex_lock(&(heap->mutex), GUI_MUTEX_WAIT_FOREVER);
	block = (char *)data - sizeof(BlockHeader);
	if ((*(BlockHeader *)block) == HEAP_BLOCK_USED)
	{
		*((BlockHeader *)block) = HEAP_BLOCK_FREE;
		i = (block - (char *)heap->data) / heap->block_size;
		if (heap->first_free_block > i)
		{
			heap->first_free_block = i;
		}
	}
	gui_mutex_unlock(&(heap->mutex));
}

void heap_destroy(PrivateHeap *heap)
{
	gui_free(heap->data);
	gui_mutex_delete(&(heap->mutex));
}

U32 print_heap_total_used_block(PrivateHeap *heap)
{
	unsigned char *data = NULL;
	U32 i, count = 0;

	gui_mutex_lock(&(heap->mutex), GUI_MUTEX_WAIT_FOREVER);
	data = heap->data;
	for (i = 0; i < heap->block_number; i++)
	{
		if (*((BlockHeader *)data) == HEAP_BLOCK_USED)
		{
			count++;
		}
		data += heap->block_size;
	}
	gui_mutex_unlock(&(heap->mutex));
	return count;
}
