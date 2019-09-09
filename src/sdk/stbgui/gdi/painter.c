/*
 * painter.c
 *
 *  Created on: 2018-6-27
 *      Author: Brant
 */

#include <gui_common.h>


BOOL get_draw_rect(UIPainter *painter, UIRect rect, UIRect *draw_rect)
{
	UIRect res;

	if (painter == NULL || draw_rect == NULL)
	{
		return FALSE;
	}

	res = offset_rect(rect, painter->offset.x, painter->offset.y);
	if (!is_intersect(res, ui_rect(0, 0, painter->canvas->size.width, painter->canvas->size.height)))
	{
		return FALSE;
	}
	if (!is_intersect(res, painter->clip_region.bound))
	{
		return FALSE;
	}
//	printf("[(%d,%d) - (%d,%d)]\n", res.point.x, res.point.y, res.size.width, res.size.height);
	*draw_rect = res;

	return TRUE;
}


int fill_rect_with_brush(UIPainter *painter, UIRect rect, UIBrush *brush)
{
	UIRect draw_rect;
	UIClipRect *clip_rect;
	UIRect result;
	int x, y;
	UIPoint brush_offset;

	if (brush == NULL || brush->brush_data == NULL || brush->brush_func == NULL || brush->set_pixel_func == NULL)
	{
		return -1;
	}
	if (!get_draw_rect(painter, rect, &draw_rect))
	{
		return -1;
	}

	brush_offset = offset_point(rect.point, painter->offset.x, painter->offset.y);
	clip_rect = painter->clip_region.head;
	while (clip_rect)
	{
		if (intersect_rect(&result, draw_rect, clip_rect->rect))
		{
			for (y = result.point.y; y < result.point.y + result.size.height; y++)
			{
				for (x = result.point.x; x < result.point.x + result.size.width; x++)
				{
					brush->set_pixel_func(painter->canvas, x, y, brush->brush_func(brush->brush_data, x - brush_offset.x, y - brush_offset.y), brush->brush_data);
				}
			}

#if GSOSD_ENABLE_CANVAS_MASK
			SetCanvasMark(result.point.y, result.size.height);
#endif
		}
		clip_rect = clip_rect->next;
	}

	return 0;
}
