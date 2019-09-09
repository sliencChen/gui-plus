/*
 * clip_region.h
 *
 *  Created on: 2015-5-18
 *      Author: Brant
 */

#ifndef _GUI_CLIP_REGION_H_
#define _GUI_CLIP_REGION_H_

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _ui_clip_rect_
{
	UIRect rect;
	struct _ui_clip_rect_* next;
} UIClipRect;

typedef struct _ui_clip_region_
{
	UIRect		bound;	/* bound rect of clip region */
	UIClipRect*	head;	/* clip rect list head */
	UIClipRect*	tail;	/* clip rect list tail */
} UIClipRegion;


BOOL init_clip_region_heap(void);
void destroy_clip_region_heap(void);

BOOL init_clip_region(UIClipRegion *clip_region);
BOOL is_empty_clip_region(UIClipRegion *clip_region);
/*
 * calculate boundary of clip region
 */
BOOL get_bound_clip_region(UIClipRegion *clip_region);
/*
 * to make the clip region clean by delete the empty node
 */
BOOL cleanup_clip_region(UIClipRegion *clip_region);
/*
 * to make clip region empty
 */
BOOL empty_clip_region(UIClipRegion *clip_region);
/*
 * set initial rectangle to clip region
 */
BOOL set_init_rect_clip_region(UIClipRegion *clip_region, UIRect rect);
BOOL copy_clip_region(UIClipRegion *dst, UIClipRegion *src);
/*
 * add a rect to clip region
 * no recalculate boundary of clip region
 */
BOOL add_rect_clip_region(UIClipRegion *clip_region, UIRect rect);
/*
 * every rect of clip region will be clipped by the "rect"
 */
BOOL intersect_clip_region(UIClipRegion *clip_region, UIRect rect);	/* + */
BOOL subtract_clip_region(UIClipRegion *clip_region, UIRect rect);	/* - */
BOOL subtract_by_clip_region(UIClipRegion *clip_region, UIClipRegion *subtract);

/*
 * get intersect between a clip region and a invalidate region
 */
BOOL clip_region_intersect_inv_region(UIClipRegion *dst_region, const UIClipRegion *clip_region, UIInvalidRegion *inv_region);

/*
 * for debug
 */
#if ENABLE_DEBUG_CLIP_RGN
BOOL print_clip_region(UIClipRegion *clip_region);
void print_clip_region_use(void);
#endif


#ifdef __cplusplus
}
#endif

#endif /* _GUI_CLIP_REGION_H_ */
