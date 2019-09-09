/*
 * clip_region.c
 *
 *  Created on: 2015-5-18
 *      Author: Brant
 */

#include <gdi/gui_interface.h>
#include <gdi/rect.h>
#include <guicore/private_heap.h>
#include <guicore/invalid_region.h>
#include <guicore/clip_region.h>



static PrivateHeap clip_region_heap;


BOOL init_clip_region_heap(void)
{
	return heap_create(&clip_region_heap, CLIP_RGN_HEAP_SIZE, sizeof(UIClipRect));
}

void destroy_clip_region_heap(void)
{
	heap_destroy(&clip_region_heap);
}


BOOL init_clip_region(UIClipRegion *clip_region)
{
	if (!clip_region)
	{
		return FALSE;
	}
	gui_memset(clip_region, 0, sizeof(UIClipRegion));
	return TRUE;
}

BOOL is_empty_clip_region(UIClipRegion *clip_region)
{
	if (!clip_region || !clip_region->head)
	{
		return TRUE;
	}
	return FALSE;
}

/*
 * calculate boundary of clip region
 */
BOOL get_bound_clip_region(UIClipRegion *clip_region)
{
	UIRect rect;
	UIClipRect *node, *temp;

	if (!clip_region)
	{
		return FALSE;
	}
	node = clip_region->head;
	if (!node)
	{
		return TRUE;
	}
	gui_memcpy(&rect, &(node->rect), sizeof(UIRect));
	node = node->next;
	while (node)
	{
		if (rect_left(node->rect) < rect_left(rect))
		{
			set_rect_left(&rect, rect_left(node->rect));
		}
		if (rect_top(node->rect) < rect_top(rect))
		{
			set_rect_top(&rect, rect_top(node->rect));
		}
		if (rect_right(node->rect) > rect_right(rect))
		{
			set_rect_right(&rect, rect_right(node->rect));
		}
		if (rect_bottom(node->rect) > rect_bottom(rect))
		{
			set_rect_bottom(&rect, rect_bottom(node->rect));
		}
		node = node->next;
	}
	gui_memcpy(&(clip_region->bound), &rect, sizeof(UIRect));

    /* cleanup null rect node in the clip region link table */
	node = clip_region->head;
	while (node->next)
	{
		if (is_empty_rect(node->next->rect))
		{
			temp = node->next;
			node->next = temp->next;
			heap_free(&clip_region_heap, (void *)temp);
		}
		else
		{
			node = node->next;
		}
	}
	/* remove head node if it is empty */
	clip_region->tail = node;
	if (is_empty_rect(clip_region->head->rect))
	{
		temp = clip_region->head;
		clip_region->head = temp->next;
		heap_free(&clip_region_heap, (void*)temp);
	}

	return TRUE;
}

/*
 * to make the clip region clean by delete the empty node
 */
BOOL cleanup_clip_region(UIClipRegion *clip_region)
{
//	PClipRect pListNode;
//	PClipRect pPrevNode;
//	pPrevNode = pListNode = pClipRegion->pHead;
//	if(!pClipRegion)
//		return false;
//	while(pListNode){
//		if(IsRectEmpty(&pListNode->rect)){
//			if(pListNode == pClipRegion->pHead){
//				if(pClipRegion->pHead == pClipRegion->pTail){//only one node
//					pClipRegion->pHead = pClipRegion->pTail = NULL;
//				}
//				else{//not only one node
//					pClipRegion->pHead = pListNode->pNext;
//				}
//			}
//			else if(pListNode == pClipRegion->pTail){
//				pClipRegion->pTail = pPrevNode;
//			}
//			else{
//				pPrevNode->pNext = pListNode->pNext;
//			}
//
//			HeapFree(pClipRegion->pHeap, (void*)pListNode);
//		}
//		pPrevNode = pListNode;
//		pListNode = pListNode->pNext;
//	}
	return TRUE;
}

/*
 * to make clip region empty
 */
BOOL empty_clip_region(UIClipRegion *clip_region)
{
	UIClipRect *node;
	UIClipRect *next_node;

	if (!clip_region)
	{
		return FALSE;
	}
	next_node = node = clip_region->head;
	while (node)
	{
		next_node = node->next;
		heap_free(&clip_region_heap, (void *)node);
		node = next_node;
	}
	clip_region->head = clip_region->tail = NULL;
	gui_memset(&(clip_region->bound), 0, sizeof(UIRect));

	return TRUE;
}

/*
 * set initial rectangle to clip region
 */
BOOL set_init_rect_clip_region(UIClipRegion *clip_region, UIRect rect)
{
	UIClipRect *node;

	if (clip_region->head)	/* if not empty clip region */
	{
		if (!empty_clip_region(clip_region))
		{
			return FALSE;
		}
	}
	node = (UIClipRect *)heap_alloc(&clip_region_heap);
	if (!node)
	{
		return FALSE;
	}
	gui_memcpy(&(node->rect), &rect, sizeof(UIRect));
	node->next = NULL;
	clip_region->head = clip_region->tail = node;
	gui_memcpy(&(clip_region->bound), &rect, sizeof(UIRect));

	return TRUE;
}

BOOL copy_clip_region(UIClipRegion *dst, UIClipRegion *src)
{
	UIClipRect *src_node, *dst_node;

	if (!dst || !src)
	{
		return FALSE;
	}
	if (dst->head)
	{
		empty_clip_region(dst);
	}
	if (!src->head)
	{
		return FALSE;
	}
	dst->head = (UIClipRect *)heap_alloc(&clip_region_heap);
	dst_node = dst->head;
	src_node = src->head;
	gui_memcpy(&dst->head->rect, &src->head->rect, sizeof(UIRect));
	src_node = src_node->next;
	while (src_node)
	{
		dst_node->next = (UIClipRect *)heap_alloc(&clip_region_heap);
		gui_memcpy(&dst_node->next->rect, &src_node->rect, sizeof(UIRect));
		dst_node = dst_node->next;
		src_node = src_node->next;
	}
	dst_node->next = NULL;
	dst->tail = dst_node;

	gui_memcpy(&dst->bound, &src->bound, sizeof(UIRect));

	return TRUE;
}

/*
 * add a rect to clip region
 * no recalculate boundary of clip region
 */
BOOL add_rect_clip_region(UIClipRegion *clip_region, UIRect rect)
{
	UIClipRect *node;

	if (!clip_region/* || !rect*/)
	{
		return FALSE;
	}
	node = (UIClipRect *)heap_alloc(&clip_region_heap);
	gui_memcpy(&node->rect, &rect, sizeof(UIRect));
	node->next = NULL;
	if (!clip_region->head)
	{
		clip_region->head = clip_region->tail = node;
		gui_memcpy(&clip_region->bound, &rect, sizeof(UIRect));
	}
	else
	{
		clip_region->tail->next = node;
		clip_region->tail = node;
	}

	return TRUE;
}

/*
 * every rect of clip region will be clipped by the "rect"
 */
BOOL intersect_clip_region(UIClipRegion *clip_region, UIRect rect)
{
	UIClipRect *node;
	UIRect clip_rect;

    gui_memcpy(&clip_rect, &rect, sizeof(UIRect));
//	normalize_rect(&clip_rect);
	node = clip_region->head;
	while (node)
	{
		if (!intersect_rect(&(node->rect), node->rect, clip_rect))
		{
			set_rect_empty(&(node->rect));
		}
		node = node->next;
	}
	get_bound_clip_region(clip_region);

	return TRUE;
}

BOOL subtract_clip_region(UIClipRegion *clip_region, UIRect rect)
{
	int i, count;
	UIRect new_rect[4];
	UIRect clip_rect;
	UIRect cur_rect;
	UIClipRect *node, *insert_pos, *new_node;

	if (!clip_region/* || !rect*/)
	{
		return FALSE;
	}
	gui_memcpy(&clip_rect, &rect, sizeof(UIRect));
//	normalize_rect(&clip_rect);
//	clip_rect = inflate_rect(rect, 1, 1);
	node = clip_region->head;
	if (!is_intersect(clip_rect, clip_region->bound))
	{
		return TRUE;
	}
	while (node)
	{
		if (!is_intersect(node->rect, clip_rect))
		{
			node = node->next;
			continue;
		}
		gui_memcpy(&cur_rect, &(node->rect), sizeof(UIRect));
		count = 0;
		if (rect_top(clip_rect) >= rect_top(cur_rect))
		{
			gui_memcpy(&new_rect[count], &cur_rect, sizeof(UIRect));
			set_rect_bottom(&(new_rect[count]), rect_top(clip_rect));
			set_rect_top(&cur_rect, rect_top(clip_rect));
			count++;
		}
		if (rect_bottom(clip_rect) <= rect_bottom(cur_rect))
		{
			gui_memcpy(&new_rect[count], &cur_rect, sizeof(UIRect));
			set_rect_top(&(new_rect[count]), rect_bottom(clip_rect));
			set_rect_bottom(&cur_rect, rect_bottom(clip_rect));
			count++;
		}
		if (rect_left(clip_rect) >= rect_left(cur_rect))
		{
			gui_memcpy(&new_rect[count], &cur_rect, sizeof(UIRect));
			set_rect_right(&(new_rect[count]), rect_left(clip_rect));
			count++;
		}
		if (rect_right(clip_rect) <= rect_right(cur_rect))
		{
			gui_memcpy(&new_rect[count], &cur_rect, sizeof(UIRect));
			set_rect_left(&(new_rect[count]), rect_right(clip_rect));
			count++;
		}
		if (count == 0)
		{
			set_rect_empty(&(node->rect));
		}
		else
		{
			insert_pos = node->next;
			gui_memcpy(&(node->rect), &new_rect[0], sizeof(UIRect));
			for (i = 1; i < count; i++)
			{
				new_node = (UIClipRect *)heap_alloc(&clip_region_heap);
				new_node->next = NULL;
				gui_memcpy(&(new_node->rect), &new_rect[i], sizeof(UIRect));
				node->next = new_node;
				node = node->next;
			}
			node->next = insert_pos;
		}
		node = node->next;
	}
	get_bound_clip_region(clip_region);

	return TRUE;
}

BOOL subtract_by_clip_region(UIClipRegion *clip_region, UIClipRegion *subtract)
{
	UIClipRect *node;

	if (!clip_region || !subtract)
	{
		return FALSE;
	}
	node = subtract->head;
	while (node)
	{
		subtract_clip_region(clip_region, node->rect);
		node = node->next;
	}
	return TRUE;
}

/*
 * get intersect between a clip region and a invalidate region
 */
BOOL clip_region_intersect_inv_region(UIClipRegion *dst_region, const UIClipRegion *clip_region, UIInvalidRegion *inv_region)
{
	UIRect rect;
	UIClipRect *clip_rect;
	UIClipRect *new_node;
	UIInvalidRect *invalid_rect;

	if (!dst_region || !clip_region || !inv_region)
	{
		return FALSE;
	}
	if (!is_intersect(clip_region->bound, inv_region->bound))
	{
		return TRUE;
	}
	clip_rect = clip_region->head;
	invalid_rect = inv_region->head;
	while (clip_rect)
	{
		while(invalid_rect)
		{
			if (intersect_rect(&rect, clip_rect->rect, invalid_rect->rect))
			{
				new_node = (UIClipRect *)heap_alloc(&clip_region_heap);
				new_node->next = NULL;
				gui_memcpy(&new_node->rect, &rect, sizeof(UIRect));
				if (!dst_region->head)
				{
					dst_region->head = dst_region->tail = new_node;
				}
				else
				{
					dst_region->tail->next = new_node;
					dst_region->tail = new_node;
				}
			}
			invalid_rect = invalid_rect->next;
		}
		invalid_rect = inv_region->head;
		clip_rect = clip_rect->next;
	}
	get_bound_clip_region(dst_region);

	return TRUE;
}


#if ENABLE_DEBUG_CLIP_RGN
BOOL print_clip_region(UIClipRegion *clip_region)
{
	UIClipRect *node;

	if (clip_region == NULL || clip_region->head == NULL)
	{
		CLIP_LOG("(empty)\n");
		return FALSE;
	}
	node = clip_region->head;
	while (node)
	{
		CLIP_LOG("%s\n", rect_to_string(node->rect));
		node = node->next;
	}
	return TRUE;
}

void print_clip_region_use(void)
{
	printf("used clip region block: %d\n", print_heap_total_used_block(&clip_region_heap));
}
#endif
