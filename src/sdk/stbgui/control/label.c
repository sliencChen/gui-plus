/*
 * label.c
 *
 *  Created on: 2018-6-25
 *      Author: Brant
 */

#include <gui_window.h>
#include <control/label.h>


//-------------------------------------------------------------------------------------------------
// local variable
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
// internal use
//-------------------------------------------------------------------------------------------------
static int set_label_text_internal(UILabelData *lbl_data, const char *text)
{
	int text_len = strlen(text);
	BOOL need_new_space = FALSE;

	if (lbl_data->text == NULL || lbl_data->text_size <= text_len)
	{
		need_new_space = TRUE;
	}
	if (need_new_space)
	{
		if (lbl_data->text != NULL)
		{
			gui_free(lbl_data->text);
			lbl_data->text_size = 0;
		}
		lbl_data->text = (U16 *)gui_malloc((text_len + 1)* sizeof(U16));
		if (lbl_data->text == NULL)
		{
			return -1;
		}
		lbl_data->text_size = text_len + 1;
	}
	utf8_to_ucs2(text, text_len, lbl_data->text, text_len + 1);

	return 0;
}


static UISize calculate_text_size(UILabelData *lbl_data)
{
	UISize size;
	int stop = wcslen(lbl_data->text);
	size.width = get_ucs2_text_width(lbl_data->text, stop, lbl_data->font_size, 100);
	size.height = get_text_height(lbl_data->font_size, 100);
	return size;
}

static BOOL adjust_text_show_rect(UILabelData *lbl_data, UIRect *src_rect)
{
	int text_length;
	UISize text_size = {0, 0};
	U8 ver_direction = 0;
	U8 hor_direction = 0;
	if (NULL == lbl_data)
	{
		return FALSE;
	}

	text_size = calculate_text_size(lbl_data);
	text_length = wcslen(lbl_data->text);
	if (text_length <= 0)
	{
		gui_free(lbl_data->text);
		lbl_data->text_size = 0;
		return FALSE;
	}

	if (lbl_data->auto_size)
	{
		return TRUE;
	}
	else
	{
		ver_direction = lbl_data->align_style & 0xf0;
		hor_direction = lbl_data->align_style & 0x0f;
		switch (ver_direction)
		{
			case LABEL_ALIGN_TOP:
				break;
			case LABEL_ALIGN_BOTTOM:
				src_rect->point.y = src_rect->size.height <= text_size.height ? 0 : src_rect->size.height - text_size.height;
				break;
			case LABEL_ALIGN_CENTER_VER:
			default:
				src_rect->point.y = src_rect->size.height <= text_size.height ? 0 : (src_rect->size.height - text_size.height) / 2;
				break;
		}

		switch (hor_direction)
		{
			case LABEL_ALIGN_LEFT:
				break;
			case LABEL_ALIGN_CENTER_HOR:
				src_rect->point.x = src_rect->size.width <= text_size.width ? 0 : (src_rect->size.width - text_size.width) / 2;
				break;
			case LABEL_ALIGN_RIGHT:
			src_rect->point.x = src_rect->size.width <= text_size.width ? 0 : src_rect->size.width - text_size.width;
				break;
			case LABEL_ALIGN_AUTO:
			default:
				if(lbl_data->text_direction == WRITE_DIRECTION_RTL)
				{
					src_rect->point.x = src_rect->size.width <= text_size.width ? 0 : src_rect->size.width - text_size.width;
				}
				else if(lbl_data->text_direction == WRITE_DIRECTION_AUTO)
				{
					if(have_rtl_character(lbl_data->text,text_length))
					{
						src_rect->point.x = src_rect->size.width <= text_size.width ? 0 : src_rect->size.width - text_size.width;
					}
				}
				break;
		}
	}
	return TRUE;
}

static UILabelData* new_label_private_data(void)
{
	UILabelData *lbl_data;

	lbl_data = (UILabelData *)gui_malloc(sizeof(UILabelData));
	if (lbl_data == NULL)
	{
		return NULL;
	}

	/* default parameter */
	gui_memset(lbl_data, 0, sizeof(UILabelData));
	set_label_text_internal(lbl_data, "Label Text");
	lbl_data->color = BLACK_COLOR;
	lbl_data->font_size = 18;
	lbl_data->auto_size = TRUE;
	lbl_data->align_style = LABEL_ALIGN_CENTER_VER | LABEL_ALIGN_AUTO;

	return lbl_data;
}

static void delete_label_private_data(UILabelData *lbl_data)
{
	if (lbl_data)
	{
		if (lbl_data->text)
		{
			gui_free(lbl_data->text);
		}
		gui_free(lbl_data);
	}
}


//-------------------------------------------------------------------------------------------------
// class event
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
// window manager callback
//-------------------------------------------------------------------------------------------------
static BOOL label_on_draw(UILabel *self)
{
	UIPainter *painter = &(self->painter);
	UIRect rect = ui_rect(0, 0, self->rect.size.width, self->rect.size.height);
	UILabelData *lbl_data = self->private_data;
	DrawTextParam param;

	default_draw_text_param(&param);
	param.color = lbl_data->color;
	param.font_size = lbl_data->font_size;
	param.direction = lbl_data->text_direction;

	adjust_text_show_rect(lbl_data, &rect);
	draw_ucs2_text(painter, rect, lbl_data->text, -1, &param);
	return TRUE;
}


static BOOL label_on_destroy(UILabel *self)
{
	delete_label_private_data(self->private_data);

	return TRUE;
}


//-------------------------------------------------------------------------------------------------
// interface
//-------------------------------------------------------------------------------------------------
UILabel* create_label(const char *id)
{
	UIWindow *label;

	label = create_control(id, 0, 0, 160, 50);
	if (label != NULL)
	{
		UILabelData *lbl_data = new_label_private_data();

		if (lbl_data == NULL)
		{
			gui_free(label);
			return NULL;
		}

		label->private_data = lbl_data;
		label->focus_stop = FALSE;
		label->class_name = LABEL_CLASS_NAME;
		label->on_draw = label_on_draw;
		label->on_destroy = label_on_destroy;
	}
	return label;
}


int set_label_text(UILabel *label, const char *text)
{
	UILabelData *lbl_data;

	if (!label || !label->private_data || !text)
	{
		return -1;
	}
	lbl_data = (UILabelData *)label->private_data;
	set_label_text_internal(lbl_data, text);
	if (lbl_data->auto_size)
	{
		set_window_size(label, calculate_text_size(lbl_data));
	}
	invalidate_window(label);

	return 0;
}


int set_label_auto_size(UILabel *label, BOOL auto_size)
{
	UILabelData *lbl_data;

	if (!label || !label->private_data)
	{
		return -1;
	}
	lbl_data = (UILabelData *)label->private_data;
	lbl_data->auto_size = auto_size;
	if (auto_size)
    {
        set_window_size(label, calculate_text_size(lbl_data));
    }
    invalidate_window(label);

	return 0;
}


int set_label_text_color(UILabel *label, U32 color)
{
	UILabelData *lbl_data;

	if (!label || !label->private_data)
	{
		return -1;
	}
	lbl_data = (UILabelData *)label->private_data;
	if (lbl_data->color == color)
	{
		return 0;
	}
	lbl_data->color = color;
	invalidate_window(label);

	return 0;
}


int set_label_font_size(UILabel *label, int font_size)
{
	UILabelData *lbl_data;

	if (!label || !label->private_data)
	{
		return -1;
	}
	lbl_data = (UILabelData *)label->private_data;
	lbl_data->font_size = font_size;
	if (lbl_data->auto_size)
	{
		set_window_size(label, calculate_text_size(lbl_data));
	}
	invalidate_window(label);

	return 0;
}


int set_label_align_style(UILabel *label, LABEL_HOR_ALIGN_STYLE hor_style, LABEL_VER_ALIGN_STYLE ver_style)
{
	UILabelData *lbl_data = NULL;
	if (!label || !label->private_data)
	{
		return -1;
	}

	if ((LABEL_ALIGN_AUTO > hor_style ) || (LABEL_ALIGN_NUM_HOR <= hor_style) || (LABEL_ALIGN_CENTER_VER > ver_style ) || (LABEL_ALIGN_NUM_VER <= ver_style))
	{
		return -1;
	}
	lbl_data = (UILabelData *)label->private_data;
	lbl_data->align_style = hor_style | ver_style;
	invalidate_window(label);
	return 0;
}


int set_label_text_dir(UILabel *label, LABEL_TEXT_DIRECTION dir)
{
	UILabelData *lbl_data = NULL;

	if (!label || !label->private_data)
	{
		return -1;
	}
	lbl_data = (UILabelData *)label->private_data;
	switch(dir)
	{
		case LABEL_WRITE_LTR:
			lbl_data->text_direction = WRITE_DIRECTION_LTR;
			break;
		case LABEL_WRITE_RTL:
			lbl_data->text_direction = WRITE_DIRECTION_RTL;
			break;
		case LABEL_WRITE_AUTO:
		default:
			lbl_data->text_direction = WRITE_DIRECTION_AUTO;
			break;
	}
	invalidate_window(label);
}
