/*
 * private_heap.h
 *
 *  Created on: 2015-5-18
 *      Author: Brant
 */

#ifndef _GUI_PRIVATE_HEAP_H_
#define _GUI_PRIVATE_HEAP_H_


#ifdef __cplusplus
extern "C" {
#endif


typedef unsigned int		BlockHeader;

#define HEAP_BLOCK_FREE		0x00000000
#define HEAP_BLOCK_USED		0x11111111


typedef struct _private_heap_
{
	int				mutex;
	unsigned int	block_number;
	unsigned int	block_size;
	unsigned int	first_free_block;
	unsigned char*	data;
} PrivateHeap;


BOOL heap_create(PrivateHeap *heap, unsigned int block_number, unsigned int block_size);
void* heap_alloc(PrivateHeap *heap);
void heap_free(PrivateHeap *heap, void* data);
void heap_destroy(PrivateHeap *heap);
U32 print_heap_total_used_block(PrivateHeap *heap);


#ifdef __cplusplus
}
#endif


#endif /* _GUI_PRIVATE_HEAP_H_ */
