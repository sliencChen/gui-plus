/*
 * bitmap.c
 *
 *  Created on: 2018-6-26
 *      Author: Brant
 */

#include <gui_common.h>
#include <gdi/bitmap.h>


#define PI      3.1416


//-------------------------------------------------------------------------------------------------
// operate bitmap
//-------------------------------------------------------------------------------------------------
static BOOL init_bitmap(UIBmp *bmp, UISize size, int color_mode, int has_trans)
{
	BOOL result;

	result = create_pixel_buf(&(bmp->pixel_buf), NULL, size, color_mode);
	if (result)
	{
		bmp->has_trans = has_trans;
	}
	return result;
}

BOOL init_system_bitmap(UIBmp *bmp, UISize size)
{
	return init_bitmap(bmp, size, main_canvas_color_mode(), FALSE);
}

BOOL init_compatible_bitmap(UIBmp *bmp, UIBmp *src, UISize size)
{
	return init_bitmap(bmp, size, src->pixel_buf.color_mode, src->has_trans);
}

void destroy_bitmap(UIBmp *bmp)
{
	if (bmp)
	{
		destroy_pixel_buf(&(bmp->pixel_buf));
	}
}


//-------------------------------------------------------------------------------------------------
// draw bitmap internal
//-------------------------------------------------------------------------------------------------
static BOOL check_bitmap_data_addr(UIBmp *bmp_info)
{
	if ((U32)bmp_info->pixel_buf.addr % bmp_info->pixel_buf.pixel_size != 0)
	{
		return FALSE;
	}
	return TRUE;
}

static int draw_bitmap_internal_v2(UIPainter *painter, UIRect rect, UIBmp *bmp_info)
{
	UIRect draw_rect;
	UIRect bmp_rect;
	UIClipRect *clip_rect;
	UIRect result;
	int x, y;
	UIPoint bmp_offset;

	bmp_rect.point = rect.point;
	bmp_rect.size = bmp_info->pixel_buf.size;
	bmp_rect = offset_rect(bmp_rect, painter->offset.x, painter->offset.y);
	if (!get_draw_rect(painter, rect, &draw_rect))
	{
		return -1;
	}
	if (!intersect_rect(&draw_rect, draw_rect, bmp_rect))
	{
		return -1;
	}
	if (!check_bitmap_data_addr(bmp_info))
	{
		return -2;
	}

	bmp_offset = offset_point(rect.point, painter->offset.x, painter->offset.y);
	clip_rect = painter->clip_region.head;
	while (clip_rect)
	{
		if (intersect_rect(&result, draw_rect, clip_rect->rect))
		{
			GDI_LOG("BMP2: %s\n", rect_to_string(result));
			if (!bmp_info->has_trans && bmp_info->pixel_buf.color_mode == painter->canvas->color_mode)
			{
				for (y = result.point.y; y < result.point.y + result.size.height; y++)
				{
					set_canvas_buf(painter->canvas, result.point.x, y,
							get_canvas_buf(&(bmp_info->pixel_buf), result.point.x - bmp_offset.x, y - bmp_offset.y), result.size.width);
				}
			}
			else if (bmp_info->pixel_buf.color_mode == COLOR_MODE_ARGB8888)
			{
				for (y = result.point.y; y < result.point.y + result.size.height; y++)
				{
					for (x = result.point.x; x < result.point.x + result.size.width; x++)
					{
						blend_canvas_8888(painter->canvas, x, y, get_canvas_pixel(&(bmp_info->pixel_buf), x - bmp_offset.x, y - bmp_offset.y));
					}
				}
			}
			else if (bmp_info->pixel_buf.color_mode == COLOR_MODE_RGB565)
			{
				// no need implement yet
			}
			else if (bmp_info->pixel_buf.color_mode == COLOR_MODE_ARGB1555)
			{
				for (y = result.point.y; y < result.point.y + result.size.height; y++)
				{
					for (x = result.point.x; x < result.point.x + result.size.width; x++)
					{
						blend_canvas_1555(painter->canvas, x, y, get_canvas_pixel(&(bmp_info->pixel_buf), x - bmp_offset.x, y - bmp_offset.y));
					}
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


//-----------------------------------------------------------------------------
// draw normal bitmap
//-----------------------------------------------------------------------------
static int draw_bitmap_normal(UIPainter *painter, UIRect rect, DrawBmpParam *param)
{
	return draw_bitmap_internal_v2(painter, rect, &(param->bmp_info));
}


//-----------------------------------------------------------------------------
// draw variable size bitmap
//-----------------------------------------------------------------------------
typedef struct _var_size_bmp_brush_data_
{
	UIRect rect;
	UIBmp bmp_info;
	UIPoint copy_pt;
} VarSizeBmpBrushData;

static U32 var_size_bmp_brush_func(void *brush_data, int x, int y)
{
	VarSizeBmpBrushData *br = (VarSizeBmpBrushData *)brush_data;

	if (x >= br->copy_pt.x)
	{
		int extra_width = br->rect.size.width - br->bmp_info.pixel_buf.size.width;

		x = (x <= br->copy_pt.x + extra_width) ? (br->copy_pt.x) : (x - extra_width);
	}
	if (y >= br->copy_pt.y)
	{
		int extra_height = br->rect.size.height - br->bmp_info.pixel_buf.size.height;

		y = (y <= br->copy_pt.y + extra_height) ? (br->copy_pt.y) : (y - extra_height);
	}

	return get_canvas_pixel(&(br->bmp_info.pixel_buf), x, y);
}

static void var_size_bmp_set_pixel_1555(UICanvas *canvas, int x, int y, U32 color, void *brush)
{
	blend_canvas_1555(canvas, x, y, color);
}

static void var_size_bmp_set_pixel_8888(UICanvas *canvas, int x, int y, U32 color, void *brush)
{
	blend_canvas_8888(canvas, x, y, color);
}

static int draw_bitmap_var_size(UIPainter *painter, UIRect rect, DrawBmpParam *param)
{
	VarSizeBmpBrushData brush_data;
	UIBrush var_size_bmp_brush = {NULL, var_size_bmp_brush_func, NULL};

	if (param->mode_param.copy_pt.x == INVALID_COPY_POS)
	{
		rect.size.width = param->bmp_info.pixel_buf.size.width;
	}
	if (param->mode_param.copy_pt.y == INVALID_COPY_POS)
	{
		rect.size.height = param->bmp_info.pixel_buf.size.height;
	}
	if (param->mode_param.copy_pt.x != INVALID_COPY_POS && param->mode_param.copy_pt.x > param->bmp_info.pixel_buf.size.width)
	{
		param->mode_param.copy_pt.x = param->bmp_info.pixel_buf.size.width / 2;
	}
	if (param->mode_param.copy_pt.y != INVALID_COPY_POS && param->mode_param.copy_pt.y > param->bmp_info.pixel_buf.size.height)
	{
		param->mode_param.copy_pt.y = param->bmp_info.pixel_buf.size.height / 2;
	}

	brush_data.bmp_info = param->bmp_info;
	brush_data.copy_pt = param->mode_param.copy_pt;
	brush_data.rect = rect;
	var_size_bmp_brush.brush_data = &brush_data;
	if (param->bmp_info.pixel_buf.color_mode == COLOR_MODE_ARGB8888)
	{
		var_size_bmp_brush.set_pixel_func = var_size_bmp_set_pixel_8888;
	}
	else if (param->bmp_info.pixel_buf.color_mode == COLOR_MODE_ARGB1555)
	{
		var_size_bmp_brush.set_pixel_func = var_size_bmp_set_pixel_1555;
	}

	return fill_rect_with_brush(painter, rect, &var_size_bmp_brush);
}


//-----------------------------------------------------------------------------
// draw stretch bitmap
//-----------------------------------------------------------------------------
static void stretch_bitmap_fast(UIBmp *src, UIBmp *dst)
{
	U32 x, y, src_x, src_y = 0;
	U32 x_ratio, y_ratio;

	x_ratio = (src->pixel_buf.size.width << 16) / dst->pixel_buf.size.width + 1;
	y_ratio = (src->pixel_buf.size.height << 16) / dst->pixel_buf.size.height + 1;
	for (y = 0; y < dst->pixel_buf.size.height; y++)
	{
		src_x = 0;
		for (x = 0; x < dst->pixel_buf.size.width; x++)
		{
			set_canvas_pixel(&(dst->pixel_buf), x, y, get_canvas_pixel(&(src->pixel_buf), src_x >> 16, src_y >> 16));
			src_x += x_ratio;
		}
		src_y += y_ratio;
	}
}

/****************Bilinear Zoom Method Start****************/
typedef void (*BILINEAR_CALCULATE)(UIColor *, U8, U8, UIColor *);

//ARGB1555
static void bilinear_calculate_16bit(UIColor* pixels, U8 u, U8 v, UIColor* result)
{
	U8 pm3 = (u * v) >> 8;
	U8 pm2 = u - pm3;
	U8 pm1 = v - pm3;
	short pm0 = 256 - pm1 - pm2 - pm3;

	U32 r, g, b;

	r = (pm0 * (pixels[0] & 0x7C00) + pm2 * (pixels[1] & 0x7C00)
			+ pm1 * (pixels[2] & 0x7C00) + pm3 * (pixels[3] & 0x7C00)) >> 8;
	g = (pm0 * (pixels[0] & 0x3E0) + pm2 * (pixels[1] & 0x3E0)
			+ pm1 * (pixels[2] & 0x3E0) + pm3 * (pixels[3] & 0x3E0)) >> 8;
	b = (pm0 * (pixels[0] & 0x1F)  + pm2 * (pixels[1] & 0x1F)
			+ pm1 * (pixels[2] & 0x1F) + pm3 * (pixels[3] & 0x1F)) >> 8;

	*(U32*)(result) = (1 << 15) | (r & 0x7C00) | (g & 0x3E0) | (b & 0x1F); //ignore alpha and always set 1
}

//ARGB8888
static void bilinear_calculate_32bit(UIColor* pixels, U8 u, U8 v, UIColor* result)
{
	U32 color, a_g, r_b;
	U8 pm3 = (u * v) >> 8;
	U8 pm2 = u - pm3;
	U8 pm1 = v - pm3;
	short pm0 = 256 - pm1 - pm2 - pm3;

	color = (U32)pixels[0];
	a_g = ((color & 0xFF00FF00) >> 8) * pm0;
	r_b = (color & 0x00FF00FF) * pm0;
	color = (U32)pixels[1];
	a_g += ((color & 0xFF00FF00) >> 8) * pm2;
	r_b += (color & 0x00FF00FF) * pm2;
	color = (U32)pixels[2];
	a_g += ((color & 0xFF00FF00) >> 8) * pm1;
	r_b += (color & 0x00FF00FF) * pm1;
	color = (U32)pixels[3];
	a_g += ((color & 0xFF00FF00) >> 8) * pm3;
	r_b += (color & 0x00FF00FF) * pm3;

	*(U32*)(result) = (a_g & 0xFF00FF00) | ((r_b & 0xFF00FF00) >> 8);
}

static inline U32 get_bilinear_pixel_x(UIBmp *bmp, S16 x)
{
	return x < 0 ? 0 : (x >= bmp->pixel_buf.size.width ? (bmp->pixel_buf.size.width - 1) : x);
}

static inline U32 get_bilinear_pixel_y(UIBmp *bmp, S16 y)
{
	return y < 0 ? 0 : (y >= bmp->pixel_buf.size.height ? (bmp->pixel_buf.size.height - 1) : y);
}

static void bilinear_border_pixels(UIBmp *src, S16 x, S16 y, UIColor *pixels)
{
	pixels[0] = get_canvas_pixel(&(src->pixel_buf), get_bilinear_pixel_x(src, x), get_bilinear_pixel_y(src, y));
	pixels[1] = get_canvas_pixel(&(src->pixel_buf), get_bilinear_pixel_x(src, x + 1), get_bilinear_pixel_y(src, y));
	pixels[2] = get_canvas_pixel(&(src->pixel_buf), get_bilinear_pixel_x(src, x), get_bilinear_pixel_y(src, y + 1));
	pixels[3] = get_canvas_pixel(&(src->pixel_buf), get_bilinear_pixel_x(src, x + 1), get_bilinear_pixel_y(src, y + 1));
}

/*Formula:	Sx = (Dx + 0.5)*SW/DW - 0.5
 * 			Sy = (Dy + 0.5)*SH/DH - 0.5
 * 			u=Sx-floor(Sx); v=Sy-floor(Sy);
 * 			pm0=(1-u)*(1-v);
 * 			pm1=v*(1-u);
 * 			pm2=u*(1-v);
 * 			pm3=u*v;
 * 			DstColor = Color0*pm0 + Color1*pm1 + Color2*pm2 + Color3*pm3;
 */
void stretch_bitmap_bilinear(UIBmp* src, UIBmp* dst)
{
	long x_ratio = ((src->pixel_buf.size.width) << 16) / dst->pixel_buf.size.width + 1;
	long y_ratio = ((src->pixel_buf.size.height) << 16) / dst->pixel_buf.size.height + 1;
	const long x_start = (x_ratio >> 1) - (1 << 15);  //Sx0 = (Dx0 + 0.5)*SW/DW - 0.5, Dx0 = 0;
	const long y_start = (y_ratio >> 1) - (1 << 15);

	long border_left = -x_start/x_ratio + 1; //Sx0+Dx*x_ratio >= 0 --> Dx >= -Sx0/x_ratio
	long border_top = -y_start/y_ratio + 1;
	long border_right = (((src->pixel_buf.size.width - 1) << 16) - x_start) / x_ratio + 1; //Sx0+Dx*x_ratio<SW-1 --> Dx < (SW-1-Sx0)/x_ratio
	long border_bottom = (((src->pixel_buf.size.height - 1) << 16) - y_start) / y_ratio + 1;

	U8 v;
	U32 x, y;
	UIColor color;
	UIColor pixels[4];
	long src_x, real_src_x, real_src_y;
	long src_y = y_start;
	long dst_row_offset = 0;
	BILINEAR_CALCULATE bilinear_calculate = NULL;
	int color_mode;

	if (border_top >= dst->pixel_buf.size.height)
	{
		border_top = dst->pixel_buf.size.height;
	}
	if (border_left >= dst->pixel_buf.size.width)
	{
		border_left = dst->pixel_buf.size.width;
	}
	if (border_bottom < border_top)
	{
		border_bottom = border_top;
	}
	if (border_right < border_left)
	{
		border_right = border_left;
	}

	color_mode = src->pixel_buf.color_mode;
	if (color_mode == COLOR_MODE_ARGB8888)
	{
		bilinear_calculate = bilinear_calculate_32bit;
	}
	else
	{
		bilinear_calculate = bilinear_calculate_16bit;
	}

	//top border
	for (y = 0; y < border_top; ++y)
	{
		src_x = x_start;
		v = (src_y & 0xFFFF) >> 8;
		real_src_y = src_y >> 16;

		for (x = 0; x < dst->pixel_buf.size.width; ++x)
		{
			bilinear_border_pixels(src, src_x >> 16, real_src_y, pixels);;
			bilinear_calculate(pixels, (src_x & 0xFFFF) >> 8, v, &color);
			set_canvas_pixel(&(dst->pixel_buf), x, y, color);
			src_x += x_ratio;
		}

		src_y += y_ratio;
		dst_row_offset += dst->pixel_buf.size.width;
	}

	for (y = border_top; y < border_bottom; ++y)
	{
		src_x = x_start;
		v = (src_y & 0xFFFF) >> 8;
		real_src_y = src_y >> 16;

		//left border
		for (x = 0; x < border_left; ++x)
		{
			bilinear_border_pixels(src, src_x >> 16, real_src_y, pixels);
			bilinear_calculate(pixels, (src_x & 0xFFFF) >> 8, v, &color);
			set_canvas_pixel(&(dst->pixel_buf), x, y, color);
			src_x += x_ratio;
		}

		for (x = border_left; x < border_right; ++x)
		{
			real_src_x = src_x >> 16;
			pixels[0] = get_canvas_pixel(&(src->pixel_buf), real_src_x, real_src_y);
			pixels[1] = get_canvas_pixel(&(src->pixel_buf), real_src_x + 1, real_src_y);
			pixels[2] = get_canvas_pixel(&(src->pixel_buf), real_src_x, real_src_y + 1);
			pixels[3] = get_canvas_pixel(&(src->pixel_buf), real_src_x + 1, real_src_y + 1);
			bilinear_calculate(pixels, (src_x & 0xFFFF) >> 8, v, &color);
			set_canvas_pixel(&(dst->pixel_buf), x, y, color);
			src_x += x_ratio;
		}

		//right border
		for (x = border_right; x < dst->pixel_buf.size.width; ++x)
		{
			bilinear_border_pixels(src, src_x >> 16, real_src_y, pixels);
			bilinear_calculate(pixels, (src_x & 0xFFFF) >> 8, v, &color);
			set_canvas_pixel(&(dst->pixel_buf), x, y, color);
			src_x += x_ratio;
		}

		src_y += y_ratio;
		dst_row_offset += dst->pixel_buf.size.width;
	}

	//bottom border
	for (y = border_bottom; y < dst->pixel_buf.size.height; ++y)
	{
		src_x = x_start;
		v = (src_y & 0xFFFF) >> 8;
		real_src_y = src_y >> 16;

		for (x = 0; x < dst->pixel_buf.size.width; ++x)
		{
			bilinear_border_pixels(src, src_x >> 16, real_src_y, pixels);
			bilinear_calculate(pixels, (src_x & 0xFFFF) >> 8, v, &color);
			set_canvas_pixel(&(dst->pixel_buf), x, y, color);
			src_x += x_ratio;
		}

		src_y += y_ratio;
		dst_row_offset += dst->pixel_buf.size.width;
	}
}
/****************Bilinear Zoom Method End****************/

static int draw_bitmap_stretch(UIPainter *painter, UIRect rect, DrawBmpParam *param)
{
	UIBmp dst_bmp;
	int res = -1;

	if (!init_compatible_bitmap(&dst_bmp, &(param->bmp_info), rect.size))
	{
		return -1;
	}
	if (param->mode_param.fast)
	{
		stretch_bitmap_fast(&(param->bmp_info), &dst_bmp);
	}
	else
	{
		stretch_bitmap_bilinear(&(param->bmp_info), &dst_bmp);
	}
	res = draw_bitmap_internal_v2(painter, rect, &dst_bmp);
	destroy_bitmap(&dst_bmp);

	return res;
}


//-----------------------------------------------------------------------------
// draw repeat right bitmap
//-----------------------------------------------------------------------------
static void repeat_bmp_right_area(UIBmp *src, UIBmp *dst, int width, int start_x)
{
	int x, y;
	int repeat_width = 0;

	if (start_x < 0 || start_x >= src->pixel_buf.size.width)
	{
		start_x = src->pixel_buf.size.width - 2;
	}

	repeat_width = src->pixel_buf.size.width - start_x;
	for (y = 0; y < dst->pixel_buf.size.height; y++)
	{
		for (x = 0; x < dst->pixel_buf.size.width; x++)
		{
			if (x < src->pixel_buf.size.width)
			{
				set_canvas_pixel(&(dst->pixel_buf), x, y, get_canvas_pixel(&(src->pixel_buf), x, y));
			}
			else
			{
				set_canvas_pixel(&(dst->pixel_buf), x, y, get_canvas_pixel(&(src->pixel_buf), src->pixel_buf.size.width + start_x + (x - src->pixel_buf.size.width) % repeat_width, y));
			}
		}
	}
}

static int draw_bitmap_repeat_right(UIPainter *painter, UIRect rect, DrawBmpParam *param)
{
	UIBmp dst_bmp;
	int res = -1;

	if (!init_compatible_bitmap(&dst_bmp, &(param->bmp_info), rect.size))
	{
		return -1;
	}
	repeat_bmp_right_area(&(param->bmp_info), &dst_bmp, rect.size.width, param->mode_param.start_x);
	res = draw_bitmap_internal_v2(painter, rect, &dst_bmp);
	destroy_bitmap(&dst_bmp);

	return res;
}


//-----------------------------------------------------------------------------
// draw part bitmap
//-----------------------------------------------------------------------------
static void get_bitmap_part(UIBmp *src, UIBmp *dst, UIRect part_rect)
{
	int y = 0;

	/* make part bmp */
	for (y = 0; y < part_rect.size.height; y++)
	{
		set_canvas_buf(&(dst->pixel_buf), 0, y, get_canvas_buf(&(src->pixel_buf), part_rect.point.x, part_rect.point.y + y), dst->pixel_buf.size.width);
	}
}

static int draw_bitmap_part(UIPainter *painter, UIRect rect, DrawBmpParam *param)
{
	UIBmp dst_bmp;
	UISize bmp_size, part_size;
	int res = -1;

	bmp_size = param->bmp_info.pixel_buf.size;
	part_size = param->mode_param.part_rect.size;
	if (!init_compatible_bitmap(&dst_bmp, &(param->bmp_info), ui_size(MIN(part_size.width, bmp_size.width), MIN(part_size.height, bmp_size.height))))
	{
		return -1;
	}
	get_bitmap_part(&(param->bmp_info), &dst_bmp, param->mode_param.part_rect);
	res = draw_bitmap_internal_v2(painter, rect, &dst_bmp);
	destroy_bitmap(&dst_bmp);

	return res;
}


//-----------------------------------------------------------------------------
// draw rotate bitmap
//-----------------------------------------------------------------------------
static void rotate_bitmap(UIBmp *src_bmp, UIBmp *dst_bmp, int angle, U32 key_color)
{
	int x, y;
	int iIndexWidth, iIndexHeight;
	int iIndexWBeg, iIndexHBeg;
	int iIndexWEnd, iIndexHEnd;
	int iDesX, iDesY, iDexAdd;
	double dRealRota;
	double dSin, dCos;
	double dTemp;
	U32 uitemp;

	dRealRota = (double)angle * (double)PI / (double)180;
	dSin = sin(dRealRota);
	dCos = cos(dRealRota);
	iIndexWBeg = -src_bmp->pixel_buf.size.width / 2;
	iIndexHBeg = -src_bmp->pixel_buf.size.height / 2;
	iIndexWEnd = src_bmp->pixel_buf.size.width / 2;
	iIndexHEnd = src_bmp->pixel_buf.size.height / 2;
	iDexAdd = dst_bmp->pixel_buf.size.width / 2 - 1;

	dSin = (double)dSin * (double)0.9;
	dCos = (double)dCos * (double)0.9;

	for (y = 0; y < dst_bmp->pixel_buf.size.height; y++)
	{
		for (x = 0; x < dst_bmp->pixel_buf.size.width; x++)
		{
			set_canvas_pixel(&(dst_bmp->pixel_buf), x, y, key_color);
		}
	}
	for (iIndexHeight = iIndexHBeg; iIndexHeight < iIndexHEnd; iIndexHeight++)
	{
		for (iIndexWidth = iIndexWBeg; iIndexWidth < iIndexWEnd; iIndexWidth++)
		{
			uitemp = get_canvas_pixel(&(src_bmp->pixel_buf), iIndexWidth + iIndexWEnd, iIndexHeight + iIndexHEnd);
			if (uitemp == 0)
				continue;
			dTemp = (double)iIndexWidth * dCos + (double)iIndexHeight * dSin;
			iDesX = (int)dTemp;
			if (dTemp - (double)iDesX > 0.5)
				iDesX++;
			else if ((double)iDesX - dTemp > 0.5)
				iDesX--;
			dTemp = (double)iIndexHeight * dCos - (double)iIndexWidth * dSin;
			iDesY = (int)dTemp;
			if (dTemp - (double)iDesY > 0.5)
				iDesY++;
			else if ((double)iDesY - dTemp > 0.5)
				iDesY--;

			set_canvas_pixel(&(dst_bmp->pixel_buf), iDesX + iDexAdd, iDesY + iDexAdd, uitemp);
		}
	}
}

static int draw_bitmap_rotate(UIPainter *painter, UIRect rect, DrawBmpParam *param)
{
	int rotation_temp = 0;
	int width = 0;
	int height = 0;
	UIBmp dst_bmp, tmp_bmp;
	float width_offset, height_offset;
	int res = -1;

	rotation_temp = param->mode_param.rotate.angle + 180;
	width_offset = ((float)((1 - param->mode_param.rotate.w_ratio) * param->bmp_info.pixel_buf.size.width)) / 90.0;
	height_offset = ((float)((param->mode_param.rotate.h_ratio - 1) * param->bmp_info.pixel_buf.size.height)) / 90.0;
	if ((rotation_temp >= 0 && rotation_temp < 90) || (rotation_temp >= 180 && rotation_temp < 270))
	{
		width = param->bmp_info.pixel_buf.size.width - (float)(width_offset * (rotation_temp % 90));
		height = param->bmp_info.pixel_buf.size.height + (float)(height_offset * (rotation_temp % 90));
	}
	else if ((rotation_temp >= 90 && rotation_temp < 180) || (rotation_temp >= 270 && rotation_temp < 360))
	{
		width = param->bmp_info.pixel_buf.size.width - (float)(width_offset * (90 - (rotation_temp % 90)));
		height = param->bmp_info.pixel_buf.size.height + (float)(height_offset * (90 - (rotation_temp % 90)));
	}

	if (!init_compatible_bitmap(&tmp_bmp, &param->bmp_info, ui_size(width, height)))
	{
		return -1;
	}
	stretch_bitmap_bilinear(&param->bmp_info, &tmp_bmp);

	if (!init_compatible_bitmap(&dst_bmp, &tmp_bmp, ui_size(2 * tmp_bmp.pixel_buf.size.height, tmp_bmp.pixel_buf.size.height)))
	{
		destroy_bitmap(&tmp_bmp);
		return -1;
	}
	dst_bmp.pixel_buf.size.width = tmp_bmp.pixel_buf.size.height;
	dst_bmp.pixel_buf.size.height = tmp_bmp.pixel_buf.size.height;
	dst_bmp.has_trans = TRUE;

	rect.size.width = MAX(rect.size.width, dst_bmp.pixel_buf.size.width);
	rect.size.height = MAX(rect.size.height, dst_bmp.pixel_buf.size.height);
	rect.point.x = rect.point.x + (rect.size.width - dst_bmp.pixel_buf.size.width) / 2;
	rect.point.y = rect.point.y + (rect.size.height - dst_bmp.pixel_buf.size.height) / 2;

	rotate_bitmap(&tmp_bmp, &dst_bmp, param->mode_param.rotate.angle, 0);
	res = draw_bitmap_internal_v2(painter, rect, &dst_bmp);

	destroy_bitmap(&dst_bmp);
	destroy_bitmap(&tmp_bmp);

	return res;
}

//-----------------------------------------------------------------------------
// interface
//-----------------------------------------------------------------------------
int draw_bitmap(UIPainter *painter, UIRect rect, DrawBmpParam *param)
{
	switch (param->mode)
	{
		case DRAW_BMP_MODE_NORMAL:
			return draw_bitmap_normal(painter, rect, param);

		case DRAW_BMP_MODE_VAR_SIZE:
			return draw_bitmap_var_size(painter, rect, param);

		case DRAW_BMP_MODE_STRETCH:
			return draw_bitmap_stretch(painter, rect, param);

		case DRAW_BMP_MODE_REPEAT_RIGHT:
			return draw_bitmap_repeat_right(painter, rect, param);

		case DRAW_BMP_MODE_PART:
			return draw_bitmap_part(painter, rect, param);

		case DRAW_BMP_MODE_ROTATE:
			return draw_bitmap_rotate(painter, rect, param);

		default:
			return -1;
	}
}
