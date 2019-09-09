/*
 * painter.h
 *
 *  Created on: 2017-12-27
 *      Author: Brant
 */

#ifndef _GUI_PAINTER_H_
#define _GUI_PAINTER_H_


typedef enum
{
	UI_DRAW_MODE_COVER,
	UI_DRAW_MODE_BLEND,
	UI_DRAW_MODE_MIX,
	UI_DRAW_MODE_NUM
} UIDrawMode;

typedef struct _ui_painter_
{
	UICanvas *canvas;
	UIPoint offset;
	UIDrawMode mode;
	UIClipRegion clip_region;
} UIPainter;


typedef struct _ui_brush_
{
	void *brush_data;
	U32 (*brush_func)(void *brush_data, int x, int y);
	void (*set_pixel_func)(UICanvas *canvas, int x, int y, U32 color, void *brush_data);
} UIBrush;


BOOL get_draw_rect(UIPainter *painter, UIRect rect, UIRect *draw_rect);

/*
 * draw rect with brush, brush can be color, bmp or anything defined by caller.
 */
int fill_rect_with_brush(UIPainter *painter, UIRect rect, UIBrush *brush);


#endif /* _GUI_PAINTER_H_ */

