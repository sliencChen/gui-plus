/*
 * invalid_rect.h
 *
 *  Created on: 2015-6-21
 *      Author: Brant
 */

#ifndef _GUI_INVALID_REGION_H_
#define _GUI_INVALID_REGION_H_

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _ui_invalid_rect_
{
	UIRect rect;
	struct _ui_invalid_rect_ *next;
} UIInvalidRect;

typedef struct _invalid_region_
{
	UIRect bound;
	UIInvalidRect *head;
	UIInvalidRect *tail;
} UIInvalidRegion;


BOOL init_invalid_region_heap(void);
void destroy_invalid_region_heap(void);

BOOL init_invalid_region(UIInvalidRegion *inv_region);
BOOL is_empty_invalid_region(UIInvalidRegion *inv_region);
BOOL get_bound_invalid_region(UIInvalidRegion *inv_region);
BOOL empty_invalid_region(UIInvalidRegion *inv_region);
BOOL copy_invalid_region(UIInvalidRegion *dst, UIInvalidRegion *src);
BOOL subtract_invalid_region(UIInvalidRegion *inv_region, UIRect rect);
BOOL add_rect_invalid_region(UIInvalidRegion *inv_region, UIRect rect);
#if ENABLE_DEBUG_INV_RGN
void print_invalid_region_use(void);
#endif


#ifdef __cplusplus
}
#endif

#endif /* _GUI_INVALID_REGION_H_ */
