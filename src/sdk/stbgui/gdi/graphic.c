/*
 * graphic.c
 *
 *  Created on: 2017-12-29
 *      Author: Brant
 */

#include <gdi/gui_interface.h>
#include <gdi/rect.h>
#include <gdi/color.h>
#include <gdi/canvas.h>
#include <guicore/invalid_region.h>
#include <guicore/clip_region.h>
#include <gdi/painter.h>
#include <gdi/graphic.h>
#include <gdi/text.h>


//static inline UIColor get_pixel(UIPainter *painter, UIPoint point)
//{
//	U32 bkg_color;
//	UIColor color;
//
//	bkg_color = get_canvas_pixel(painter->canvas, point.x, point.y);
//	if (get_main_canvas_color_mode() == COLOR_MODE_ARGB8888)
//	{
//		return bkg_color;
//	}
//	/* ARGB1555 to ARGB8888 */
//	color = (bkg_color & 0x8000) ? 0xff000000 : 0x00000000;
//	color |= ((bkg_color & 0x7c00) << 9) | ((bkg_color & 0x03e0) << 6) | ((bkg_color & 0x001f) << 3);
//
//	return color;
//}
//
//static inline UIColor get_pixel_ex(UIPainter *painter, UIPoint point, UIColor color)
//{
//	if (painter->mode == UI_DRAW_MODE_COVER) return color;
//	if (painter->mode == UI_DRAW_MODE_BLEND) return BlendBackColor32(get_pixel(painter, point), color);
//
//	return color;
//}

int set_pixel(UIPainter *painter, UIPoint point, UIColor color)
{
	UIClipRect *clip_rect;

	if (painter == NULL)
	{
		return -1;
	}
	point = offset_point(point, painter->offset.x, painter->offset.y);
	if (!point_in_rect(point, ui_rect(0, 0, painter->canvas->size.width, painter->canvas->size.height)))
	{
		return -1;
	}
	if (!point_in_rect(point, painter->clip_region.bound))
	{
		return -1;
	}

	clip_rect = painter->clip_region.head;
	while (clip_rect)
	{
		if (point_in_rect(point, clip_rect->rect))
		{
			if (painter->mode == UI_DRAW_MODE_BLEND)
			{
				blend_canvas_8888(painter->canvas, point.x, point.y, color);
			}
			else
			{
				set_canvas_pixel(painter->canvas, point.x, point.y, get_canvas_color(painter->canvas, color));
			}
#if GSOSD_ENABLE_CANVAS_MASK
			if (painter->canvas.addr == GetOSDCanvasAddr())
			{
				SetCanvasMark(point.y, 1);
			}
#endif
			break;
		}
		clip_rect = clip_rect->next;
	}

	return 0;
}


int draw_line(UIPainter *painter, UIPoint start_pt, UIPoint end_pt, int width, UIColor color, BOOL use_aa)
{
	int x_scope, y_scope;
	int x_dir = 1, y_dir = 1;
	int linear_deviance;
	int counter;
	int anti_alias_strength;
	U8 temp_anti_alias_strength;
	int x1, y1, x2, y2;
	int canvas_width, canvas_height;
	UIColor draw_color;

	UIPoint save_painter_offset;
	UIDrawMode save_painter_mode;


	if (painter == NULL)
	{
		return -1;
	}
	canvas_width = painter->canvas->size.width;
	canvas_height = painter->canvas->size.height;
	start_pt = offset_point(start_pt, painter->offset.x, painter->offset.y);
	end_pt = offset_point(end_pt, painter->offset.x, painter->offset.y);
	x1 = start_pt.x;
	y1 = start_pt.y;
	x2 = end_pt.x;
	y2 = end_pt.y;
	x_scope = x2 - x1;
	y_scope = y2 - y1;
	if (x_scope < 0)
	{
		x_scope = abs(x_scope);
		x_dir = -1;
	}
	if (y_scope < 0)
	{
		y_scope = abs(y_scope);
		y_dir = -1;
	}
	if (x_scope + y_scope == 0)
	{
		/* exit when length is 0 */
		return -1;
	}

	save_painter_offset = painter->offset;
	save_painter_mode = painter->mode;
	painter->offset = ui_point(0, 0);
	if (x_scope > y_scope)
	{
		linear_deviance = x_scope / 2;
		for (counter = 0; counter < x_scope; counter++)
		{
			/* main line, output full strength direct to osd */
			painter->mode = save_painter_mode;
			set_pixel(painter, ui_point(x1, y1), color);

			/* output the blended lines for anti-alias effect. */
			anti_alias_strength = (linear_deviance * 255) / x_scope;	//255 is max alpha
			temp_anti_alias_strength = 255 - anti_alias_strength;
			if (use_aa)
			{
				painter->mode = save_painter_mode;
				draw_color = color;//get_pixel_ex(painter, ui_point(x1, y1 - y_dir), color);
				painter->mode = UI_DRAW_MODE_BLEND;
				set_pixel(painter, ui_point(x1, y1 - y_dir), ((temp_anti_alias_strength << 24) | (draw_color & 0x00ffffff)));

				painter->mode = save_painter_mode;
				draw_color = color;//get_pixel_ex(painter, ui_point(x1, y1 + y_dir), color);
				painter->mode = UI_DRAW_MODE_BLEND;
				set_pixel(painter, ui_point(x1, y1 + y_dir), ((anti_alias_strength << 24) | (draw_color & 0x00ffffff)));
			}
			linear_deviance = (linear_deviance + y_scope);
			if (linear_deviance > x_scope)
			{
				linear_deviance = linear_deviance - x_scope;
				y1 = y1 + y_dir;
				if (y1 < 0 || y1 >= canvas_height)
				{
					break;
				}
			}
			x1 = x1 + x_dir;
			if (x1 < 0 || x1 >= canvas_width)
			{
				break;
			}
		}
	}
	else
	{
		linear_deviance = y_scope / 2;
		for (counter = 0; counter < y_scope; counter++)
		{
			/* main line, output full strength direct to osd */
			painter->mode = save_painter_mode;
			set_pixel(painter, ui_point(x1, y1), color);

			/* output the blended lines for anti-alias effect. */
			anti_alias_strength = (linear_deviance * 255) / y_scope;	//255 is max alpha
			temp_anti_alias_strength = 255 - anti_alias_strength;
			if (use_aa)
			{
				painter->mode = save_painter_mode;
				draw_color = color;//get_pixel_ex(painter, ui_point(x1 - x_dir, y1), color);
				painter->mode = UI_DRAW_MODE_BLEND;
				set_pixel(painter, ui_point(x1 - x_dir, y1), ((temp_anti_alias_strength << 24) | (draw_color & 0x00ffffff)));

				painter->mode = save_painter_mode;
				draw_color = color;//get_pixel_ex(painter, ui_point(x1 + x_dir, y1), color);
				painter->mode = UI_DRAW_MODE_BLEND;
				set_pixel(painter, ui_point(x1 + x_dir, y1), ((anti_alias_strength << 24) | (draw_color & 0x00ffffff)));
			}
			linear_deviance = linear_deviance + x_scope;
			if (linear_deviance > y_scope)
			{
				linear_deviance = linear_deviance - y_scope;
				x1 = x1 + x_dir;
				if (x1 < 0 || x1 >= canvas_width)
				{
					break;
				}
			}
			y1 = y1 + y_dir;
			if (y1 < 0 || y1 >= canvas_height)
			{
				break;
			}
		}
	}
	painter->offset = save_painter_offset;
	painter->mode = save_painter_mode;

	return 0;
}

int draw_rect(UIPainter *painter, UIRect rect, UIColor color)
{
	U32 draw_color = get_canvas_color(painter->canvas, color);
	UIClipRect *clip_rect;
	UIRect result;
	int x, y;

	if (!get_draw_rect(painter, rect, &rect))
	{
		return -1;
	}

	clip_rect = painter->clip_region.head;
	while (clip_rect)
	{
		if (intersect_rect(&result, rect, clip_rect->rect))
		{
			GDI_LOG("GRAH: %s\n", rect_to_string(result));

			switch (painter->mode)
			{
				case UI_DRAW_MODE_MIX:
					for (y = result.point.y; y < result.point.y + result.size.height; y++)
					{
						for (x = result.point.x; x < result.point.x + result.size.width; x++)
						{
							mix_canvas_8888(painter->canvas, x, y, color);
						}
					}
					break;
				case UI_DRAW_MODE_BLEND:
					for (y = result.point.y; y < result.point.y + result.size.height; y++)
					{
						for (x = result.point.x; x < result.point.x + result.size.width; x++)
						{
							blend_canvas_8888(painter->canvas, x, y, color);
						}
					}
					break;
				case UI_DRAW_MODE_COVER:
				default:
					for (x = result.point.x; x < result.point.x + result.size.width; x++)
					{
						set_canvas_pixel(painter->canvas, x, result.point.y, draw_color);
					}
					for (y = result.point.y + 1; y < result.point.y + result.size.height; y++)
					{
						set_canvas_buf(painter->canvas, result.point.x, y, get_canvas_buf(painter->canvas, result.point.x, result.point.y), result.size.width);
					}
					break;
			}
#if GSOSD_ENABLE_CANVAS_MASK
			if (painter->canvas.addr == GetOSDCanvasAddr())
			{
				SetCanvasMark(result.point.y, result.size.height);
			}
#endif
		}
		clip_rect = clip_rect->next;
	}

	return 0;
}

int draw_rect_frame_gui(UIPainter *painter, UIRect rect, int border_width, UIColor color_outside, UIColor color_inside)
{
	int loop = 0;
    int temp = 0;

	if (painter == NULL)
	{
		return -1;
	}

    /* Check border width */
	if (border_width <= 0)
	{
		border_width = 1;
	}
	temp = rect.size.width > rect.size.height ? rect.size.height : rect.size.width;
	if (border_width > temp / 2)
	{
		border_width = temp / 2;
	}

    for (loop = 0; loop < border_width; loop++)
    {
        /* top */
        draw_line(painter, ui_point(rect.point.x, rect.point.y + loop), ui_point(rect.point.x + rect.size.width, rect.point.y + loop),
                1, color_outside, TRUE);
        /* bottom */
        draw_line(painter, ui_point(rect.point.x, rect.point.y + rect.size.height - loop - 1), ui_point(rect.point.x + rect.size.width, rect.point.y + rect.size.height - loop - 1),
                1, color_outside, TRUE);
        /* left */
        draw_line(painter, ui_point(rect.point.x + loop, rect.point.y + border_width), ui_point(rect.point.x + loop, rect.point.y + rect.size.height - border_width),
                1, color_outside, TRUE);
        /* right */
        draw_line(painter, ui_point(rect.point.x + rect.size.width - loop - 1, rect.point.y + border_width), ui_point(rect.point.x + rect.size.width - loop - 1, rect.point.y + rect.size.height - border_width),
                1, color_outside, TRUE);
    }
    if (color_inside != 0)
    {
        draw_rect(painter, ui_rect(rect.point.x + border_width, rect.point.y + border_width, rect.size.width - 2 * border_width, rect.size.height - 2 * border_width),
                color_inside);
    }

	return 0;
}

int draw_box_frame_gui(UIPainter *painter, UIRect rect, int alpha, int border_width, UIColor border_color, UIColor panel_color)
{
	int loop = 0;
    int temp = 0;
    int delta;

	if (painter == NULL)
	{
		return -1;
	}

    /* Check border width */
	if (border_width <= 0)
	{
		border_width = 1;
	}
	temp = rect.size.width > rect.size.height ? rect.size.height : rect.size.width;
	if (border_width > temp / 2)
	{
		border_width = temp / 2;
	}
	delta = alpha / 2;

    draw_rect(painter, ui_rect(rect.point.x + border_width, rect.point.y + border_width + delta, rect.size.width - 2 * border_width,
            rect.size.height - 2 * border_width - alpha), panel_color);
    for (loop = 0; loop < border_width; loop++)
    {
        /* left */
        draw_line(painter, ui_point(rect.point.x + loop, rect.point.y + border_width + delta),
                ui_point(rect.point.x + loop, rect.point.y + rect.size.height - border_width - delta), 1, border_color, TRUE);
        /* right */
        draw_line(painter, ui_point(rect.point.x + rect.size.width - loop - 1, rect.point.y + border_width + delta),
                ui_point(rect.point.x + rect.size.width - loop - 1, rect.point.y + rect.size.height - border_width - delta), 1, border_color, TRUE);
    }
    for (loop = 1; loop <= border_width + delta; loop++)
    {
        if (loop <= border_width)
        {
            /* top */
            draw_line(painter, ui_point(rect.point.x, rect.point.y + border_width + delta - loop),
                    ui_point(rect.point.x + rect.size.width, rect.point.y + border_width + delta - loop), 1, border_color, TRUE);
            /* bottom */
            draw_line(painter, ui_point(rect.point.x, rect.point.y + rect.size.height - border_width - delta + loop - 1),
                    ui_point(rect.point.x + rect.size.width, rect.point.y + rect.size.height - border_width - delta + loop - 1), 1, border_color, TRUE);
        }
        else
        {
            /* top */
            draw_line(painter, ui_point(rect.point.x + loop - border_width, rect.point.y + border_width + delta - loop),
                    ui_point(rect.point.x + rect.size.width - loop + border_width, rect.point.y + border_width + delta - loop), 1, border_color, TRUE);
            /* bottom */
            draw_line(painter, ui_point(rect.point.x + loop - border_width, rect.point.y + rect.size.height - border_width - delta + loop - 1),
                    ui_point(rect.point.x + rect.size.width - loop + border_width, rect.point.y + rect.size.height - border_width - delta + loop - 1), 1, border_color, TRUE);
        }
        if (loop <= delta)
        {
            /* top */
            draw_line(painter, ui_point(rect.point.x + border_width + loop, rect.point.y + border_width + delta - loop),
                    ui_point(rect.point.x + rect.size.width - border_width - loop, rect.point.y + border_width + delta - loop), 1, panel_color, TRUE);
            /* bottom */
            draw_line(painter, ui_point(rect.point.x + border_width + loop, rect.point.y + rect.size.height - border_width - delta + loop - 1),
                    ui_point(rect.point.x + rect.size.width - border_width - loop, rect.point.y + rect.size.height - border_width - delta + loop - 1), 1, panel_color, TRUE);
        }
    }

	return 0;
}

static inline void set_symmetrical_pixel_circle(UIPainter *painter, int cx, int cy, int x, int y, U32 color, double alpha, int extra_w, int extra_h)
{
	U32 draw_color;
	U8 real_alpha;

	if (alpha < 0)
	{
		alpha = 0;
	}
	if (alpha > 1)
	{
		alpha = 1;
	}

	real_alpha = (U8)(alpha * 255);
	draw_color = (real_alpha << 24) | (color & 0x00ffffff);

	blend_canvas_8888(painter->canvas, cx + x + extra_w, cy + y + extra_h, draw_color);
	blend_canvas_8888(painter->canvas, cx + y + extra_w, cy + x + extra_h, draw_color);
	blend_canvas_8888(painter->canvas, cx + y + extra_w, cy - x - extra_h, draw_color);
	blend_canvas_8888(painter->canvas, cx + x + extra_w, cy - y - extra_h, draw_color);
	blend_canvas_8888(painter->canvas, cx - x - extra_w, cy - y - extra_h, draw_color);
	blend_canvas_8888(painter->canvas, cx - y - extra_w, cy - x - extra_h, draw_color);
	blend_canvas_8888(painter->canvas, cx - y - extra_w, cy + x + extra_h, draw_color);
	blend_canvas_8888(painter->canvas, cx - x - extra_w, cy + y + extra_h, draw_color);
}

int draw_round_rect(UIPainter *painter, UIRect rect, int radius, UIColor color, BOOL use_aa, BOOL type)
{
	UIRect draw_rt;
	int draw_radius = radius;
	int min_size;
	int cx, cy;
	int extra_w, extra_h;
	int x, y, d;
	int d_e, d_se, d_s;
	double alpha;

	UIPoint save_painter_offset;
	UIDrawMode save_painter_mode;


	if (painter == NULL)
	{
		return -1;
	}
	draw_rt = rect = offset_rect(rect, painter->offset.x, painter->offset.y);
	if (use_aa)
	{
		draw_rt = inflate_rect(draw_rt, -1, -1);
		draw_radius = radius - 1;
	}
	if (!is_intersect(draw_rt, ui_rect(0, 0, painter->canvas->size.width, painter->canvas->size.height)))
	{
		return -1;
	}
	if (!is_intersect(draw_rt, painter->clip_region.bound))
	{
		return -1;
	}
	min_size = MIN(draw_rt.size.width, draw_rt.size.height);
	if (draw_radius > min_size / 2)
	{
		draw_radius = min_size / 2;
	}
	alpha = 1.0 / ((draw_radius << 1) * 1.4142);

	cx = draw_rt.point.x + draw_rt.size.width / 2;
	cy = draw_rt.point.y + draw_rt.size.height / 2;
	extra_w = draw_rt.size.width / 2 - draw_radius;
	extra_h = draw_rt.size.height / 2 - draw_radius;
	x = 0;
	y = draw_radius;
	d = 1 - draw_radius;
	d_e = 3;
	d_se = -(draw_radius << 1) + 5;
	d_s = 0;

	save_painter_offset = painter->offset;
	save_painter_mode = painter->mode;
	painter->offset = ui_point(0, 0);
	draw_rect(painter, ui_rect(draw_rt.point.x, rect.point.y + radius, draw_rt.size.width + 1, rect.size.height - 2 * radius + 1), color);
	if (use_aa)
	{
		U8 real_alpha = 0x33;//(U8)((double)(1-1.2/1.5) * 255);
		U32 draw_color;

		draw_color = (color & 0x00ffffff) | (real_alpha << 24);
		painter->mode = UI_DRAW_MODE_BLEND;
		draw_rect(painter, ui_rect(cx - extra_w, rect.point.y, extra_w + extra_w, 1), draw_color);
		draw_rect(painter, ui_rect(cx - extra_w, rect.point.y + rect.size.height, extra_w + extra_w, 1), draw_color);

		draw_rect(painter, ui_rect(rect.point.x, rect.point.y + radius, 1, rect.size.height - 2 * radius + 1), draw_color);
		draw_rect(painter, ui_rect(rect.point.x + rect.size.width, rect.point.y + radius, 1, rect.size.height - 2 * radius + 1), draw_color);
	}

	while (x < y)
	{
		if (d < 0)
		{
			d += d_e;
			d_e += 2;
			d_se += 2;
		}
		else
		{
			d_s += 1 - (y << 1);
			d += d_se;
			d_e += 2;
			d_se += 4;
			y--;
		}
		d_s += 1 + (x << 1);
		x++;

		painter->mode = save_painter_mode;
		draw_rect(painter, ui_rect(cx - x - extra_w, cy + y + extra_h, (x + extra_w) * 2 + 1, 1), color);
		draw_rect(painter, ui_rect(cx - x - extra_w, cy - y - extra_h, (x + extra_w) * 2 + 1, 1), color);


		draw_rect(painter, ui_rect(cx - y - extra_w, cy + x + extra_h, (y + extra_w) * 2 + 1, 1), color);
		draw_rect(painter, ui_rect(cx - y - extra_w, cy - x - extra_h, (y + extra_w) * 2 + 1, 1), color);

		if (use_aa)
		{
			set_symmetrical_pixel_circle(painter, cx, cy, x, y + 1, color, 1 - alpha * (d_s + (y << 1) + 1), extra_w, extra_h);
		}
	}
	painter->offset = save_painter_offset;
	painter->mode = save_painter_mode;

	return 0;
}

int draw_triangle(UIPainter *painter, int cx, int cy, int direction, int length, UIColor color)
{
    UIColor draw_color = get_canvas_color(painter->canvas,color);
	UIPoint start_pt, end_pt;
    int counter;
	BOOL err = -1;

    if (painter == NULL)
	{
		return err;
	}

	switch (direction)
	{
		case 0:
			for (counter = 0; counter < length; counter++)
			{
				start_pt = ui_point(cx + counter, cy - (length - 1 - counter));
				end_pt = ui_point(cx + counter, cy + (length - 1 - counter));
				err &= draw_line(painter, start_pt, end_pt, 1, draw_color, TRUE);
			}
			break;
		case 1: /*left*/
			for (counter = 0; counter < length; counter++)
			{
				start_pt = ui_point(cx - counter, cy - (length - 1 - counter));
				end_pt = ui_point(cx - counter, cy + (length - 1 - counter));
				err &= draw_line(painter, start_pt, end_pt, 1, draw_color, TRUE);
			}
			break;
		case 2: /*up*/
			for (counter = 0; counter < length; counter++)
			{
				start_pt = ui_point(cx - (length - 1 - counter), cy - counter);
				end_pt = ui_point(cx + (length - 1 - counter), cy - counter);
				err &= draw_line(painter, start_pt, end_pt, 1, draw_color, TRUE);
			}
			break;
		case 3: /*down*/
			for (counter = 0; counter < length; counter++)
			{
				start_pt = ui_point(cx - (length - 1 - counter), cy + counter);
				end_pt = ui_point(cx + (length - 1 - counter), cy + counter);
				err &= draw_line(painter, start_pt, end_pt, 1, draw_color, TRUE);
			}
			break;
		case 4: //bias
			for(counter = 0; counter < length; counter++)
			{
				start_pt = ui_point(cx - (length - 1 - counter), cy + counter);
				end_pt = ui_point(cx, cy + counter);
				err &= draw_line(painter, start_pt, end_pt, 1, draw_color, TRUE);
			}
			break;
	}
    return err;
}

int draw_circle(UIPainter *painter, int cx, int cy, U32 radius, UIColor color, BOOL use_aa)
{
    if (painter == NULL)
	{
		return -1;
	}
	return draw_round_rect(painter, ui_rect(cx - radius, cy - radius, 2 * radius, 2 * radius),
			radius, color, use_aa, 0);
}

