/*
 * invalid_region.c
 *
 *  Created on: 2015-6-21
 *      Author: Brant
 */

#include <gdi/gui_interface.h>
#include <gdi/rect.h>
#include <guicore/private_heap.h>
#include <guicore/invalid_region.h>


static PrivateHeap invalid_region_heap;


BOOL init_invalid_region_heap(void)
{
	return heap_create(&invalid_region_heap, INVALID_RGN_HEAP_SIZE, sizeof(UIInvalidRect));
}

void destroy_invalid_region_heap(void)
{
	return heap_destroy(&invalid_region_heap);
}


BOOL init_invalid_region(UIInvalidRegion *inv_region)
{
	if (!inv_region)
	{
		return FALSE;
	}
	gui_memset(inv_region, 0, sizeof(UIInvalidRegion));
	return TRUE;
}

BOOL is_empty_invalid_region(UIInvalidRegion *inv_region)
{
	if (!inv_region || !inv_region->head)
	{
		return TRUE;
	}
	return FALSE;
}

static BOOL cleanup_invalid_region(UIInvalidRegion *inv_region)
{
	UIInvalidRect *node, *temp;

	if (!inv_region || !inv_region->head)
	{
		return FALSE;
	}
	/* cleanup null rect node in the clip region link table */
	node = inv_region->head;
	while (node->next)
	{
		if (is_empty_rect(node->next->rect))
		{
			temp = node->next;
			node->next = temp->next;
			heap_free(&invalid_region_heap, (void *)temp);
		}
		else
		{
			node = node->next;
		}
	}
	/* remove head node if it is empty */
	inv_region->tail = node;
	if (is_empty_rect(inv_region->head->rect))
	{
		temp = inv_region->head;
		inv_region->head = temp->next;
		heap_free(&invalid_region_heap, (void*)temp);
	}
	return TRUE;
}

BOOL get_bound_invalid_region(UIInvalidRegion *inv_region)
{
	UIRect rect;
	UIInvalidRect *node;

	cleanup_invalid_region(inv_region);

	gui_memset(&inv_region->bound, 0, sizeof(UIRect));
	/* calculate bound */
	if (!inv_region->head)
	{
		return TRUE;
	}
	node = inv_region->head;
	gui_memcpy(&rect, &node->rect, sizeof(UIRect));
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
	gui_memcpy(&inv_region->bound, &rect, sizeof(UIRect));

	return TRUE;
}

BOOL empty_invalid_region(UIInvalidRegion *inv_region)
{
	UIInvalidRect *node;
	UIInvalidRect *next;

	if (!inv_region)
	{
		return FALSE;
	}
	next = node = inv_region->head;
	while (node)
	{
		next = node->next;
		heap_free(&invalid_region_heap, (void*)node);
		node = next;
	}
	inv_region->head = inv_region->tail = NULL;
	gui_memset(&inv_region->bound, 0, sizeof(UIRect));

	return TRUE;
}

BOOL copy_invalid_region(UIInvalidRegion *dst, UIInvalidRegion *src)
{
	UIInvalidRect *src_node, *dst_node;

	if (!dst || !src)
	{
		return FALSE;
	}
	if (dst->head)
	{
		empty_invalid_region(dst);
	}
	if (!src->head)
	{
		return FALSE;
	}
	dst->head = (UIInvalidRect *)heap_alloc(&invalid_region_heap);
	dst_node = dst->head;
	src_node = src->head;
	gui_memcpy(&dst->head->rect, &src->head->rect, sizeof(UIRect));
	src_node = src_node->next;
	while (src_node)
	{
		dst_node->next = (UIInvalidRect *)heap_alloc(&invalid_region_heap);
		gui_memcpy(&dst_node->next->rect, &src_node->rect, sizeof(UIRect));
		dst_node = dst_node->next;
		src_node = src_node->next;
	}
	dst_node->next = NULL;
	dst->tail = dst_node;

	gui_memcpy(&dst->bound, &src->bound, sizeof(UIRect));

	return TRUE;
}

BOOL subtract_invalid_region(UIInvalidRegion *inv_region, UIRect rect)
{
	int i, count;
	UIRect new_rect[4];
	UIRect inv_rect;
	UIRect cur_rect;
	UIInvalidRect *node, *insert_pos, *new_node;

	if (!inv_region)
	{
		return FALSE;
	}
	gui_memcpy(&inv_rect, &rect, sizeof(UIRect));
	if (!is_intersect(inv_rect, inv_region->bound))
	{
		return TRUE;
	}
	node = inv_region->head;
	while (node)
	{
		if (!is_intersect(node->rect, inv_rect))
		{
			node = node->next;
			continue;
		}
		gui_memcpy(&cur_rect, &(node->rect), sizeof(UIRect));
		count = 0;
		if (rect_top(inv_rect) >= rect_top(cur_rect))
		{
			gui_memcpy(&new_rect[count], &cur_rect, sizeof(UIRect));
			set_rect_bottom(&(new_rect[count]), rect_top(inv_rect));
			set_rect_top(&cur_rect, rect_top(inv_rect));
			count++;
		}
		if (rect_bottom(inv_rect) <= rect_bottom(cur_rect))
		{
			gui_memcpy(&new_rect[count], &cur_rect, sizeof(UIRect));
			set_rect_top(&(new_rect[count]), rect_bottom(inv_rect));
			set_rect_bottom(&cur_rect, rect_bottom(inv_rect));
			count++;
		}
		if (rect_left(inv_rect) >= rect_left(cur_rect))
		{
			gui_memcpy(&new_rect[count], &cur_rect, sizeof(UIRect));
			set_rect_right(&(new_rect[count]), rect_left(inv_rect));
			count++;
		}
		if (rect_right(inv_rect) <= rect_right(cur_rect))
		{
			gui_memcpy(&new_rect[count], &cur_rect, sizeof(UIRect));
			set_rect_left(&(new_rect[count]), rect_right(inv_rect));
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
				new_node = (UIInvalidRect *)heap_alloc(&invalid_region_heap);
				new_node->next = NULL;
				gui_memcpy(&(new_node->rect), &new_rect[i], sizeof(UIRect));
				node->next = new_node;
				node = node->next;
			}
			node->next = insert_pos;
		}
		node = node->next;
	}
	get_bound_invalid_region(inv_region);

	return TRUE;
}

BOOL add_rect_invalid_region(UIInvalidRegion *inv_region, UIRect rect)
{
	UIInvalidRect *node, *new_node;

	if (!inv_region)
	{
		return FALSE;
	}

	subtract_invalid_region(inv_region, rect);

	new_node = (UIInvalidRect *)heap_alloc(&invalid_region_heap);
	gui_memcpy(&new_node->rect, &rect, sizeof(UIRect));
	new_node->next = NULL;

	if (inv_region->head == NULL)
	{
		inv_region->head = inv_region->tail = new_node;
		gui_memcpy(&inv_region->bound, &rect, sizeof(UIRect));
	}
	else
	{
		node = inv_region->head;
		while (node->next) node = node->next;
		inv_region->tail = node;

		inv_region->tail->next = new_node;
		inv_region->tail = new_node;
		get_bound_invalid_region(inv_region);
	}

	return TRUE;
}

#if ENABLE_DEBUG_INV_RGN
void print_invalid_region_use(void)
{
	printf("used invalid region block: %d\n", print_heap_total_used_block(&invalid_region_heap));
}
#endif
