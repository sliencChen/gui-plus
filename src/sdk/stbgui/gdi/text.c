/*
 * text.c
 *
 *  Created on: 2018-6-29
 *      Author: Brant
 */

#include <gui_common.h>
#include <gdi/graphic.h>
#include <gdi/font/arabic_transform.h>
#include <gdi/font/font_metrics.h>
#include <gdi/text.h>


static BOOL is_control_char(U16 char_code)
{
	if (char_code >= 0x01 && char_code <= 0x1f)
	{
		return TRUE;
	}
	if (char_code == 0x200c || char_code == 0x200d)
	{
		return TRUE;
	}
	return FALSE;
}

static BOOL is_transparent_char(U16 char_code)
{
	if (is_arabic_transparent_char(char_code))
	{
		return TRUE;
	}
	return FALSE;
}

static BOOL is_rtl_char(U16 char_code)
{
	/* arabic digit, need use left to right direction */
	if (char_code >= 0x660 && char_code <= 0x669)
	{
		return FALSE;
	}
	if ((char_code >= 0x0600 && char_code <= 0x077F) || (char_code >= 0xFB50 && char_code <= 0xFEFF))
	{
		return TRUE;
	}
	return FALSE;
}

static BOOL is_neutral_char(U16 char_code)
{
	if ((char_code >= 0x20 && char_code <= 0x2C)
		|| (char_code >= 0x2E && char_code <= 0x2F)
		|| (char_code >= 0x3A && char_code <= 0x40)
		|| (char_code >= 0x5B && char_code <= 0x5E)
		|| (char_code == 0x60)
		|| (char_code >= 0x7B && char_code <= 0x7E))
	{
		return TRUE;
	}
	return FALSE;
}


static int get_transform_text_max_len(const U16 *ucs2_text)
{
	int delta_len = 0;
	int count = 0;
	int text_length = wcslen(ucs2_text);

	for (count = 0; count < text_length; count++)
	{
		//khmer unicode char
		if ((ucs2_text[count] >= 0x17be && ucs2_text[count] <= 0x17c0) || ucs2_text[count] == 0x17c4 || ucs2_text[count] == 0x17c5 || ucs2_text[count] == 0x1799)
		{
			delta_len++;
		}
	}

	text_length += delta_len;

	return text_length;
}

static void get_transformed_ucs2_text_font_glyph(U16 *ucs2_text, FONT_GLYPH_MAP *font_glyph_map)
{
	int count = 0;
	int text_length = wcslen(ucs2_text);

	for (count = 0; count < text_length; count++)
	{
		get_unicode_font_glyph(ucs2_text[count], &font_glyph_map->font_fun_glyph[count]);
#if ENABLE_KHMER_LANGUAGE
		set_khmer_unicode_cat_pos(ucs2_text[count], &font_glyph_map->font_fun_glyph[count]);
#endif
		font_glyph_map->font_fun_glyph[count].unicode_props = get_char_unicode_props(ucs2_text[count]);
		font_glyph_map->font_fun_glyph[count].syllable = 0;
		font_glyph_map->font_glyph_num++;
	}
#if ENABLE_KHMER_LANGUAGE
	khmer_transform_text(font_glyph_map);
#endif
}


static inline int get_scale_percent(int font_size, int scale)
{
	double font_height_by_size;

	if (font_size < 0)
	{
		font_size = 18;//system_default_font_size();
	}
	if (scale <= 1)
	{
		scale = 100;
	}
	if (font_size == get_real_font_size())
	{
		return scale;
	}
	font_height_by_size = ((font_size * 96) / (double)72) + 4;
	return (int)((font_height_by_size * scale) / get_real_font_height());
}

static inline int get_scaled_size(int origin, int font_size, int scale)
{
	return (get_scale_percent(font_size, scale) * origin + 99) / 100;
}


static int get_transformed_ucs2_text_width(U16 *ucs2_text, int stop, int font_size, int scale)
{
	int width = 0;
	int index;
	int char_advance;
	int char_num, string_len;
	GS_TT_OUTLINE *outline = NULL;
	GS_TT_BITMAP font_bitmap;
	FONT_GLYPH_MAP font_glyph_map;
	int transform_text_max_len = 0;

	if (ucs2_text == NULL)
	{
		return 0;
	}
	memset(&font_glyph_map, 0, sizeof(FONT_GLYPH_MAP));
	transform_text_max_len = get_transform_text_max_len(ucs2_text);
	font_glyph_map.font_fun_glyph = (FONT_GLYPH *)gui_malloc((transform_text_max_len + 1) * sizeof(FONT_GLYPH));
	if (font_glyph_map.font_fun_glyph == NULL)
	{
		GS_ERROR("no enough memory\n");
		return 0;
	}

	outline = (GS_TT_OUTLINE *)gui_malloc(sizeof(GS_TT_OUTLINE));
	if (outline == NULL)
	{
		gui_free(font_glyph_map.font_fun_glyph);
		GS_ERROR("no enough memory\n");
		return 0;
	}

	get_transformed_ucs2_text_font_glyph(ucs2_text, &font_glyph_map);
	/* stop maybe great than string_len */
	char_num = string_len = font_glyph_map.font_glyph_num;
	if (stop >= 0)
	{
		char_num = MIN(stop, string_len);
	}

	width = 0;
	for (index = 0; index < char_num; index++)
	{
		if (font_glyph_map.font_fun_glyph[index].font_glyph_get_metrics_func || (!font_glyph_map.font_fun_glyph[index].font_glyph_get_metrics_func && !is_transparent_char(ucs2_text[index]) && !is_control_char(ucs2_text[index])))
		{
			get_char_metrics(font_glyph_map.font_fun_glyph[index], outline, NULL, &char_advance);

			memset(&font_bitmap, 0, sizeof(GS_TT_BITMAP));
			/* first & last character maybe exceed the range of "font advance" */
			if (index == 0)
			{
				get_glyph_cbox(outline, &font_bitmap, get_scale_percent(font_size, scale), get_scale_percent(font_size, 100));
				if (font_bitmap.iLeft < 0)
				{
					width -= font_bitmap.iLeft;
				}
			}
			if (index == char_num - 1)
			{
				int right_side;

				get_glyph_cbox(outline, &font_bitmap, get_scale_percent(font_size, scale), get_scale_percent(font_size, 100));
				right_side = font_bitmap.iLeft + font_bitmap.iWidth;

				char_advance = get_scaled_size(char_advance, font_size, scale);
				width += MAX(right_side, char_advance);
			}
			else
			{
				width += get_scaled_size(char_advance, font_size, scale);
			}
		}
	}
	if (font_glyph_map.font_fun_glyph)
	{
		gui_free(font_glyph_map.font_fun_glyph);
		font_glyph_map.font_fun_glyph = NULL;
	}

	if (outline)
	{
		gui_free(outline);
		outline = NULL;
	}
	return width;
}


static int get_windows_text_width_internal(const char *windows_text, int stop, int font_size, int scale)
{
	U16 *ucs2_text;
	int result;
	int encoding = windows_text[0];

	if (stop < 0)
	{
		stop = strlen(windows_text);
	}
	ucs2_text = (U16 *)gui_malloc((stop + 1) * sizeof(U16));
	if (!ucs2_text)
	{
		return 0;
	}
	windows_to_ucs2(windows_text, stop, ucs2_text, stop + 1, encoding);
	result = get_ucs2_text_width(ucs2_text, stop, font_size, scale);
	gui_free(ucs2_text);

	return result;
}

static int get_utf8_text_width_internal(const char *utf8_text, int stop, int font_size, int scale)
{
	U16 *ucs2_text;
	int result;

	if (stop < 0)
	{
		stop = strlen(utf8_text);
	}
	ucs2_text = (U16 *)gui_malloc((stop + 1) * sizeof(U16));
	if (!ucs2_text)
	{
		return 0;
	}
	utf8_to_ucs2(utf8_text, stop, ucs2_text, stop + 1);
	result = get_ucs2_text_width(ucs2_text, stop, font_size, scale);
	gui_free(ucs2_text);

	return result;
}


int get_text_width(const char *text, int stop, int font_size, int scale)
{
	if (text == NULL || stop == 0)
	{
		return 0;
	}
	if (is_windows_string(text))
	{
		return get_windows_text_width_internal(text, stop, font_size, scale);
	}
	return get_utf8_text_width_internal(text, stop, font_size, scale);
}

int get_ucs2_text_width(const U16 *text, int stop, int font_size, int scale)
{
	int text_length;
	U16 *transformed_text;
	int width = 0;
	int transform_text_max_len = 0;

	if (text == NULL || stop == 0)
	{
		return 0;
	}
	text_length = wcslen(text);
	if (text_length <= 0)
	{
		return 0;
	}
	transform_text_max_len = get_transform_text_max_len(text);
	transformed_text = (U16 *)gui_malloc((transform_text_max_len + 1) * sizeof(U16));
	if (transformed_text == NULL)
	{
		return 0;
	}
	arabic_text_transform(transformed_text, text);
#if ENABLE_KHMER_LANGUAGE
	khmer_text_decompos(transformed_text);
#endif
	width = get_transformed_ucs2_text_width(transformed_text, stop, font_size, scale);
	gui_free(transformed_text);

	return width;
}


int get_text_height(int font_size, int scale)
{
	return get_scaled_size(get_real_font_height(), font_size, scale);
}


static WriteDirection get_char_direction(U16 char_code)
{
	if (is_neutral_char(char_code))
	{
		return WRITE_DIRECTION_NEUTRAL;
	}
	else if (is_rtl_char(char_code))
	{
		return WRITE_DIRECTION_RTL;
	}
	return WRITE_DIRECTION_LTR;
}

static WriteDirection get_char_direction_in_text(U16 *ucs2_text, int char_index, int total, WriteDirection using_dir, WriteDirection sys_dir)
{
	int direction, check_direction;
	int check_index;

	direction = get_char_direction(ucs2_text[char_index]);
	if (direction != WRITE_DIRECTION_NEUTRAL)
	{
		return direction;
	}
	if (using_dir == sys_dir)
	{
		return using_dir;
	}
	direction = sys_dir;
	for (check_index = char_index + 1; check_index < total; check_index++)
	{
		check_direction = get_char_direction(ucs2_text[check_index]);
		if (check_direction == WRITE_DIRECTION_NEUTRAL)
		{
			continue;
		}
		if (check_direction != sys_dir)
		{
			direction = !sys_dir;
		}
		break;
	}
	return direction;
}

BOOL have_rtl_character(U16 *ucs2_text, int total)
{
	int i;

	for (i = 0; i < total; i++)
	{
		if (is_rtl_char(ucs2_text[i]))
		{
			return TRUE;
		}
	}
	return FALSE;
}


static U16 swap_bracket_pair(U16 char_code)
{
	U16 left_bracket[] = {'(', '{', '[', '<'};
	U16 right_bracket[] = {')', '}', ']', '>'};
	int i;

	for (i = 0; i < sizeof(left_bracket) / sizeof(left_bracket[0]); i++)
	{
		if (char_code == left_bracket[i])
		{
			return right_bracket[i];
		}
		else if (char_code == right_bracket[i])
		{
			return left_bracket[i];
		}
	}
	return char_code;
}

static void swap_ucs2_text_bracket_pair(U16 *ucs2_text, int count)
{
	int index;

	for (index = 0; index < count; ++index)
	{
		ucs2_text[index] = swap_bracket_pair(ucs2_text[index]);
	}
}


static void draw_font_bitmap(UIPainter *painter, UIRect rect, int x, int y, GS_TT_BITMAP *font_bitmap, U32 color)
{
	UIRect font_rect, draw_rect;
	UIClipRect *clip_rect;
	UIRect result;
	UIPoint bmp_offset;
	U32 draw_color;
	int i, j;

	if (!get_draw_rect(painter, rect, &draw_rect))
	{
		return;
	}
	font_rect = offset_rect(ui_rect(x, y, font_bitmap->iWidth, font_bitmap->iHeight), painter->offset.x, painter->offset.y);
	if (!intersect_rect(&draw_rect, font_rect, draw_rect))
	{
		return;
	}

	bmp_offset = offset_point(ui_point(x, y), painter->offset.x, painter->offset.y);
	clip_rect = painter->clip_region.head;
	while (clip_rect)
	{
		if (intersect_rect(&result, draw_rect, clip_rect->rect))
		{
			GDI_LOG("TEXT: %s\n", rect_to_string(result));
#if PLATFORM_SUPPORT == ALI_PLATFORM
			if (color <= 0xff)
			{
				for (j = result.point.y; j < result.point.y + result.size.height; j++)
				{
					for (i = result.point.x; i < result.point.x + result.size.width; i++)
					{
						if (font_bitmap->pucBuffer[(j - bmp_offset.y) * font_bitmap->iWidth + i - bmp_offset.x] & 0x80)
						{
//							painter->canvas->addr[j * painter->canvas->size.width + i] = (color & 0xff);
						}
					}
				}
			}
			else
#endif
#if 0
			if (color == CLEAR_COLOR)
			{
				for (j = result.point.y; j < result.point.y + result.size.height; j++)
				{
					for (i = result.point.x; i < result.point.x + result.size.width; i++)
					{
						set_canvas_pixel_alpha(painter->canvas, i, j, 255 - font_bitmap->pucBuffer[(j - bmp_offset.y) * font_bitmap->iWidth + i - bmp_offset.x]);
					}
				}
			}
			else
#endif
			{
				for (j = result.point.y; j < result.point.y + result.size.height; j++)
				{
					for (i = result.point.x; i < result.point.x + result.size.width; i++)
					{
							draw_color = (color & 0x00ffffff)
									| (font_bitmap->pucBuffer[(j - bmp_offset.y) * font_bitmap->iWidth + i - bmp_offset.x] << 24);
							blend_canvas_8888(painter->canvas, i, j, draw_color);
					}
				}
			}

#if GSOSD_ENABLE_CANVAS_MASK
			SetCanvasMark(result.point.y, result.size.height);
#endif
		}
		clip_rect = clip_rect->next;
	}
}


static void draw_text_caret(UIPainter *painter, UIRect rect, int x, int y, int font_size, int scale_y, U32 color)
{
	int text_height;

	text_height = get_text_height(font_size, scale_y);
	text_height = MIN(text_height, rect.size.height);
	rect.point = ui_point(x, y);
	rect.size = ui_size(2, text_height);
	draw_rect(painter, rect, color);
}

static BOOL construct_font_bitmap(GS_TT_OUTLINE *outline, GS_TT_BITMAP *font_bitmap, int font_size, int scale_x, int scale_y)
{
	int font_bmp_buf_size;

	memset(font_bitmap, 0, sizeof(GS_TT_BITMAP));
	get_glyph_cbox(outline, font_bitmap, get_scale_percent(font_size, scale_x), get_scale_percent(font_size, scale_y));
	font_bmp_buf_size = font_bitmap->iWidth * font_bitmap->iHeight;
	if (font_bmp_buf_size <= 0)
	{
		return TRUE;
	}
	font_bitmap->pucBuffer = gui_malloc(font_bmp_buf_size);
	if (NULL == font_bitmap->pucBuffer)
	{
		return FALSE;
	}
	memset(font_bitmap->pucBuffer, 0, font_bmp_buf_size);
	RenderGlyph(outline, font_bitmap, get_scale_percent(font_size, scale_x), get_scale_percent(font_size, scale_y));

	return TRUE;
}

static BOOL is_valid_font_bitmap(GS_TT_BITMAP *font_bitmap)
{
	return ((font_bitmap != NULL) && (font_bitmap->pucBuffer != NULL));
}

static void destroy_font_bitmap(GS_TT_BITMAP *font_bitmap)
{
	if (is_valid_font_bitmap(font_bitmap))
	{
		gui_free(font_bitmap->pucBuffer);
		font_bitmap->pucBuffer = NULL;
	}
}

#if ENABLE_KHMER_LANGUAGE
static int get_x_pos_delta_for_khmer(FONT_GLYPH_MAP *font_glyph_map, GS_TT_BITMAP *font_bitmap, GS_TT_OUTLINE *outline, int cur_index, int font_size, int scale_x, int scale_y)
{
	int x_delta = 0;
	GS_TT_BITMAP font_bitmap_temp;
	int ascender, advance;
	int space = 2;
	U8 next_char_pos = 0;

	if (font_bitmap->iLeft >= 0)
	{
		return x_delta;
	}

	if ((cur_index + 1) < font_glyph_map->font_glyph_num)
	{
		get_char_metrics(font_glyph_map->font_fun_glyph[cur_index + 1], outline, &ascender, &advance);

		if (!construct_font_bitmap(outline, &font_bitmap_temp, font_size, scale_x, scale_y))
		{
			return x_delta;
		}

		if ((font_bitmap_temp.iLeft) < 0 && ((font_bitmap->iLeft + font_bitmap->iWidth) > font_bitmap_temp.iLeft) && (abs(font_bitmap->iTop - font_bitmap_temp.iTop) <= space * 2))
		{
			x_delta = font_bitmap_temp.iLeft - (font_bitmap->iLeft + font_bitmap->iWidth) - space;
		}
	}

	if (((cur_index - 1) >= 0))
	{
		get_char_metrics(font_glyph_map->font_fun_glyph[cur_index - 1], outline, &ascender, &advance);
		if (!construct_font_bitmap(outline, &font_bitmap_temp, font_size, scale_x, scale_y))
		{
			return x_delta;
		}

		if (font_bitmap_temp.iLeft >= 0 && is_special_base_khmer_char(font_glyph_map->font_fun_glyph[cur_index - 1]))
		{
			x_delta = -(font_bitmap_temp.iLeft + font_bitmap_temp.iWidth + space) - font_bitmap->iLeft;
		}
	}

	return x_delta;
}
#endif


static int draw_ucs2_sequence(UIPainter *painter, UIRect rect, U16 *ucs2_text, int count, U16 *caret_ptr,
		BOOL *showed_caret, int font_size, int scale_x, int scale_y, U32 color, int dir)
{
	int index;
	U16 char_code;
	GS_TT_BITMAP font_bitmap;
	GS_TT_OUTLINE *outline = NULL;
	int ascender, advance;
	int pen_pos;
	int text_width = 0;
	int transform_text_max_len = 0;
	FONT_GLYPH_MAP font_glyph_map;
	int x_pos_delta = 0;

	transform_text_max_len = get_transform_text_max_len(ucs2_text);
	memset(&font_glyph_map, 0, sizeof(FONT_GLYPH_MAP));
	font_glyph_map.font_fun_glyph = (FONT_GLYPH *)gui_malloc((transform_text_max_len + 1) * sizeof(FONT_GLYPH));
	if (font_glyph_map.font_fun_glyph == NULL)
	{
		GS_ERROR("no enough memory\n");
		return 0;
	}

	outline = (GS_TT_OUTLINE *)gui_malloc(sizeof(GS_TT_OUTLINE));
	if (outline == NULL)
	{
		gui_free(font_glyph_map.font_fun_glyph);
		GS_ERROR("no enough memory\n");
		return 0;
	}

	get_transformed_ucs2_text_font_glyph(ucs2_text, &font_glyph_map);
	count = MIN(count, font_glyph_map.font_glyph_num);
	pen_pos = dir == WRITE_DIRECTION_LTR ? rect_left(rect) : rect_right(rect);
	for (index = 0; index <= count; ++index)	/* index <= count: caret_ptr may be equal to  */
	{
		/* show caret */
		if (caret_ptr != NULL && !(*showed_caret) && ucs2_text + index == caret_ptr)
		{
			*showed_caret = TRUE;
			draw_text_caret(painter, rect, pen_pos - 1, rect.point.y, font_size, scale_y, color);
		}
		if (index >= count)
		{
			break;
		}
		/* show character */
		char_code = font_glyph_map.font_fun_glyph[index].glyph;//ucs2_text[index];
		if (!font_glyph_map.font_fun_glyph[index].font_glyph_get_metrics_func && is_control_char(char_code))
//		if (is_control_char(char_code))
		{
			continue;
		}
		get_char_metrics(font_glyph_map.font_fun_glyph[index], outline, &ascender, &advance);
		ascender = get_scaled_size(ascender, font_size, scale_y);
		if (!font_glyph_map.font_fun_glyph[index].font_glyph_get_metrics_func && is_transparent_char(char_code))
//		if (is_transparent_char(char_code))
		{
			advance = 0;
		}
		else
		{
			advance = get_scaled_size(advance, font_size, scale_x);
		}

		if (!construct_font_bitmap(outline, &font_bitmap, font_size, scale_x, scale_y))
		{
			continue;
		}
#if ENABLE_KHMER_LANGUAGE
		if (dir == WRITE_DIRECTION_LTR)
		{
			x_pos_delta = get_x_pos_delta_for_khmer(&font_glyph_map, &font_bitmap, outline, index, font_size, scale_x, scale_y);
		}
#endif

		if (dir == WRITE_DIRECTION_RTL)
		{
			pen_pos -= advance;
			text_width += advance;
		}
		/* draw a char */
		if (is_valid_font_bitmap(&font_bitmap))
		{
			draw_font_bitmap(painter, rect, pen_pos + font_bitmap.iLeft + x_pos_delta, rect.point.y + ascender - font_bitmap.iTop, &font_bitmap, color);
			x_pos_delta = 0;
		}
		if (dir == WRITE_DIRECTION_LTR)
		{
			text_width += advance;
			pen_pos += advance;
		}

		destroy_font_bitmap(&font_bitmap);
	}

	for (index = 0; index < font_glyph_map.font_glyph_num; ++index)
	{
		ucs2_text[index] = font_glyph_map.font_fun_glyph[index].glyph;
	}
	ucs2_text[index] = 0;
	if (font_glyph_map.font_fun_glyph)
	{
		gui_free(font_glyph_map.font_fun_glyph);
		font_glyph_map.font_fun_glyph = NULL;
	}

	if (outline)
	{
		gui_free(outline);
		outline = NULL;
	}
	return text_width;
}

static int draw_transformed_ucs2_text_internal(UIPainter *painter, UIRect rect, U16 *ucs2_text, DrawTextParam *param)
{
	int text_length = 0;
	int using_direction, write_direction;
	int dir_changed_index, index;
	int draw_width = 0;

	BOOL draw_text_seq;
	U16 *text_seq = NULL;
	int text_seq_length = 0;
	int pen_pos, width_left;
	int text_seq_width;
	BOOL exceed_max_width = FALSE;
	U16 *caret_ptr = NULL;
	BOOL showed_caret = FALSE;

	text_length = wcslen(ucs2_text);
	if (text_length <= 0)
	{
		return 0;
	}

	/* init */
	dir_changed_index = 0;
	using_direction = param->direction;
	width_left = rect.size.width;
	if (param->caret_pos >= 0)
	{
		caret_ptr = ucs2_text + param->caret_pos;
	}

	for (index = 0; index <= text_length && !exceed_max_width; index++)
	{
		draw_text_seq = FALSE;
		write_direction = get_char_direction_in_text(ucs2_text, index, text_length, using_direction, param->direction);
		/* write direction change or reach string end */
		if (write_direction != using_direction)
		{
			if (index == 0)		/* use different write direction at the beginning */
			{
				dir_changed_index = index;
				using_direction = write_direction;
				continue;
			}
			draw_text_seq = TRUE;
		}
		else if (index == text_length)	/* reach end */
		{
			draw_text_seq = TRUE;
		}
		if (!draw_text_seq)
		{
			continue;
		}

		text_seq = ucs2_text + dir_changed_index;
		text_seq_length = index - dir_changed_index;

		if (using_direction == WRITE_DIRECTION_RTL)
		{
			swap_ucs2_text_bracket_pair(text_seq, text_seq_length);
		}

		text_seq_width = get_transformed_ucs2_text_width(text_seq, text_seq_length, param->font_size, param->scale_x);
		if (/*width_left > 0 && */text_seq_width > width_left)
		{
			exceed_max_width = TRUE;
			do
			{
				text_seq_length--;
				text_seq_width = get_transformed_ucs2_text_width(text_seq, text_seq_length, param->font_size, param->scale_x);
			} while (text_seq_width > width_left);
		}

		if (param->direction == WRITE_DIRECTION_LTR)
		{
			pen_pos = rect_left(rect) + draw_width;
		}
		else
		{
			pen_pos = rect_right(rect) - draw_width - text_seq_width;
		}
		draw_width += draw_ucs2_sequence(painter, ui_rect(pen_pos, rect_top(rect), text_seq_width, rect.size.height),
				text_seq, text_seq_length, caret_ptr, &showed_caret, param->font_size, param->scale_x, param->scale_y,
				param->color, using_direction);
		text_seq_length = wcslen(ucs2_text);

		dir_changed_index = index;
		using_direction = write_direction;
	}

	return draw_width;
}

static int draw_ucs2_text_internal(UIPainter *painter, UIRect rect, U16 *ucs2_text, DrawTextParam *param)
{
	int text_length;
	U16 *transformed_text;
	int transform_text_max_len = 0;
	int draw_width = 0;

	if (ucs2_text == NULL)
	{
		return 0;
	}
	text_length = wcslen(ucs2_text);
	if (text_length <= 0)
	{
		return 0;
	}

	transform_text_max_len = get_transform_text_max_len(ucs2_text);
	transformed_text = (U16 *)gui_malloc((transform_text_max_len + 1) * sizeof(U16));
	if (transformed_text == NULL)
	{
		return 0;
	}
	arabic_text_transform(transformed_text, ucs2_text);
#if ENABLE_KHMER_LANGUAGE
	khmer_text_decompos(transformed_text);
#endif
	if (param->direction == WRITE_DIRECTION_AUTO)
	{
		param->direction = have_rtl_character(transformed_text, text_length) ? WRITE_DIRECTION_RTL : WRITE_DIRECTION_LTR;
	}
	if (param->direction == WRITE_DIRECTION_RTL && !param->rtl_align_right)
	{
		int text_width;

		text_width = get_transformed_ucs2_text_width(transformed_text, -1, param->font_size, param->scale_x);
		rect.size.width = MIN(text_width, rect.size.width);
	}
	draw_width = draw_transformed_ucs2_text_internal(painter, rect, transformed_text, param);
	gui_free(transformed_text);

	return draw_width;
}

static int draw_windows_text_internal(UIPainter *painter, UIRect rect, const char *windows_text, int stop, DrawTextParam *param)
{
	U16 *ucs2_text;
	int length;
	int result;
	int encoding = windows_text[0];

	length = strlen(windows_text) + 1;
	ucs2_text = (U16 *)gui_malloc(length * sizeof(U16));
	if (!ucs2_text)
	{
		return 0;
	}
	windows_to_ucs2(windows_text, stop, ucs2_text, length, encoding);
	result = draw_ucs2_text_internal(painter, rect, ucs2_text, param);
	gui_free(ucs2_text);

	return result;
}

static int draw_utf8_text_internal(UIPainter *painter, UIRect rect, const char *utf8_text, int stop, DrawTextParam *param)
{
	U16 *ucs2_text;
	int length;
	int result;

	length = strlen(utf8_text) + 1;
	ucs2_text = (U16 *)gui_malloc(length * sizeof(U16));
	if (!ucs2_text)
	{
		return 0;
	}
	utf8_to_ucs2(utf8_text, stop, ucs2_text, length);
	result = draw_ucs2_text_internal(painter, rect, ucs2_text, param);
	gui_free(ucs2_text);

	return result;
}


void default_draw_text_param(DrawTextParam *param)
{
	param->font_size = 18;
	param->scale_x = 100;
	param->scale_y = 100;
	param->color = BLACK_COLOR;
	param->direction = WRITE_DIRECTION_AUTO;
	param->rtl_align_right = FALSE;
	param->caret_pos = -1;
}


int draw_text(UIPainter *painter, UIRect rect, const char *text, int stop, DrawTextParam *param)
{
	if (painter == NULL || text == NULL || stop == 0 || param == NULL)
	{
		return 0;
	}
	if (is_windows_string(text))
	{
		return draw_windows_text_internal(painter, rect, text, stop, param);
	}
	return draw_utf8_text_internal(painter, rect, text, stop, param);
}

int draw_ucs2_text(UIPainter *painter, UIRect rect, U16 *text, int stop, DrawTextParam *param)
{
	U16 *text_part;
	int result;

	if (painter == NULL || text == NULL || stop == 0 || param == NULL)
	{
		return 0;
	}
	if (stop < 0)	/* draw whole text */
	{
		return draw_ucs2_text_internal(painter, rect, text, param);
	}

	text_part = (U16 *)gui_malloc((stop + 1) * sizeof(U16));
	if (text_part == NULL)
	{
		return 0;
	}
	wcsncpy(text_part, text, stop);
	text_part[stop] = '\0';
	result = draw_ucs2_text_internal(painter, rect, text_part, param);
	gui_free(text_part);

	return result;
}


//-------------------------------------------------------------------------------------------------
// gui interface
//-------------------------------------------------------------------------------------------------
int gui_draw_text(UIPainter *painter, UIRect rect, const char *text, UIColor color)
{
	DrawTextParam param;

	default_draw_text_param(&param);
	param.color = color;

	return draw_text(painter, rect, text, -1, &param);
}
