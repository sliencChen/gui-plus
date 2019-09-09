
/*
 * text_field.c
 *
 *  Created on: 2019-6-17
 *      Author: Brant
 */

#include <gui_window.h>
#include <control/text_field.h>


//-------------------------------------------------------------------------------------------------
// local variable
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
// internal use
//-------------------------------------------------------------------------------------------------
static int get_line_num_of_page(UITextField *textfield)
{
	return textfield->rect.size.height / get_text_field_line_height(textfield);
}

static int get_caret_line_index(UITextFieldData *textfield_data)
{
	int i;

	for (i = 0; i < textfield_data->line_num; i++)
	{
		if (textfield_data->caret_pos >= textfield_data->line_info[i].offset
			&& textfield_data->caret_pos < textfield_data->line_info[i].offset + textfield_data->line_info[i].length)
		{
			return i;
		}
	}

	return 0;
}

/* find char '\n' */
static inline int get_char_lf_index(U16 *text, int length)
{
	int i;

	for (i = 0; i < length; i++)
	{
		if (text[i] == '\n')
		{
			return i;
		}
	}
	return -1;
}

static inline BOOL is_cjk_character(U16 ucs2_char)
{
	if (ucs2_char >= 0x4E00 && ucs2_char <= 0x9FCC)
	{
		return TRUE;
	}
	return FALSE;
}

static inline BOOL is_word_splitter(U16 ucs2_char, U16 *word_splitter)
{
	int index = 0;

	if (word_splitter == NULL)
	{
		return FALSE;
	}
	while (word_splitter[index] != 0)
	{
		if (ucs2_char == word_splitter[index])
		{
			return TRUE;
		}
		index++;
	}

	return FALSE;
}

static inline int get_next_word_end_index(U16 *text, int start_index, int total, U16 *word_splitter, BOOL by_word)
{
	int i = 0;

	if (!by_word)
	{
		return start_index;
	}
	for (i = start_index; i < total; i++)
	{
		if (is_word_splitter(text[i], word_splitter))
		{
			return i;
		}
		if (is_cjk_character(text[i]))
		{
			if (i > start_index)
			{
				return i - 1;
			}
			return i;
		}
	}
	return total - 1;
}

static inline int get_prev_word_end_index(U16 *text, int end_index, U16 *word_splitter, BOOL by_word)
{
	int i = 0;

	if (!by_word)
	{
		return end_index;
	}
	for (i = end_index; i >= 0; i--)
	{
		if (is_word_splitter(text[i], word_splitter) || is_cjk_character(text[i]))
		{
			return i;
		}
	}
	return -1;
}

static int get_line_char_num(U16 *text, int length, int font_size, int scale, int line_width, U16 *word_splitter, BOOL by_word, int guess_num)
{
	int word_end_index;
	int last_valid_word_end_index = -1;
	int lf_index = -1;
	int check_length;

	guess_num = MIN(guess_num, length);

	/* search forward first */
	word_end_index = guess_num - 1;
	last_valid_word_end_index = -1;
	do
	{
		word_end_index = get_next_word_end_index(text, word_end_index + 1, length, word_splitter, by_word);
		lf_index = get_char_lf_index(text, word_end_index + 1);
		check_length = word_end_index + 1;
		if (lf_index >= 0)	/* find '\n' */
		{
			check_length = lf_index + 1;
		}
		if (get_ucs2_text_width(text, check_length, font_size, scale) <= line_width)
		{
			if (lf_index >= 0)
			{
				return lf_index + 1;
			}
			last_valid_word_end_index = word_end_index;
		}
		else
		{
			break;
		}
	} while (word_end_index < length - 1);
	if (last_valid_word_end_index >= 0)
	{
		return last_valid_word_end_index + 1;
	}

	/* search backward */
	word_end_index = check_length - 1;
	while (word_end_index >= 0)
	{
		word_end_index = get_prev_word_end_index(text, word_end_index - 1, word_splitter, by_word);
		if (word_end_index >= 0 && get_ucs2_text_width(text, word_end_index + 1, font_size, scale) <= line_width)
		{
			return word_end_index + 1;
		}
	}

	/* if length of first word exceed line width */
	word_end_index = check_length - 1;
	while (word_end_index >= 0)
	{
		word_end_index = get_prev_word_end_index(text, word_end_index - 1, NULL, FALSE);
		if (word_end_index >= 0 && get_ucs2_text_width(text, word_end_index + 1, font_size, scale) <= line_width)
		{
			return word_end_index + 1;
		}
	}

	return 0;
}


static BOOL single_line_text_layout(UITextFieldData *textfield_data, int windows_width)
{
	int offset = 0;
	int length = 0;
	int width;
	int i;

	if (textfield_data->editable)
	{
		/* get text width ahead caret */
		for (i = textfield_data->caret_pos - 1; i >= 0; i--)
		{
			width = get_ucs2_text_width(textfield_data->text + i, textfield_data->caret_pos - i, textfield_data->font_size, textfield_data->font_scale_x);
			if (width > windows_width - 6)
			{
				offset = i + 1;
				length = textfield_data->caret_pos - i - 1;
				goto END;
			}
		}
	}
	length = get_line_char_num(textfield_data->text, textfield_data->char_num, textfield_data->font_size,
			textfield_data->font_scale_x, windows_width - 6, NULL, FALSE, 0);

END:
	textfield_data->start_line = 0;
	textfield_data->line_info[0].offset = offset;
	textfield_data->line_info[0].length = length;
	textfield_data->line_num = 1;

	return TRUE;
}

static BOOL multi_line_text_layout(UITextFieldData *textfield_data, int windows_width)
{
	U16 splitter[] = {',', ' ', '.', '?', '!', '-', '\n', '\r', 0};
	int char_index = 0;
	int char_num = 0;
	int guess_number = 0;
	int char_num_remain;

	textfield_data->line_num = 0;
	char_index = 0;
	char_num_remain = textfield_data->char_num + 1;	/* include '\0' */
	while (char_num_remain > 0)
	{
		char_num = get_line_char_num(textfield_data->text + char_index, char_num_remain, textfield_data->font_size, textfield_data->font_scale_x,
				windows_width - 6, splitter, TRUE, guess_number);
        //printf("line[%d] char_num[%d]\n", textfield_data->line_num, char_num);
		if (textfield_data->line_num >= MAX_TEXT_LINE_NUM)
		{
			break;
		}
		textfield_data->line_info[textfield_data->line_num].offset = char_index;
		textfield_data->line_info[textfield_data->line_num].length = char_num;
		textfield_data->line_num++;

		char_index += char_num;
		char_num_remain -= char_num;
		guess_number = MAX(guess_number, char_num);
	}

	return TRUE;
}

static BOOL text_layout(UITextFieldData *textfield_data, int windows_width)
{
	if (textfield_data->multi_line)
	{
		multi_line_text_layout(textfield_data, windows_width);
	}
	else
	{
		single_line_text_layout(textfield_data, windows_width);
	}

	return TRUE;
}


static void init_text_field_param(UITextFieldData *textfield_data)
{
	memset(textfield_data, 0, sizeof(UITextFieldData));

	textfield_data->text_color = RED_COLOR;

	textfield_data->char_num_can_be_inserted = NULL;

	textfield_data->editable = FALSE;
	textfield_data->page_loop = TRUE;
	textfield_data->font_size = -1;

    textfield_data->font_scale_x = 100;
    textfield_data->font_scale_y = 100;

	textfield_data->line_spacing = 1.0;
	textfield_data->text_direction = WRITE_DIRECTION_AUTO;

	textfield_data->multi_line = TRUE;
	textfield_data->align_center = FALSE;

	textfield_data->caret_pos = 0;
	textfield_data->start_line = 0;
}

static BOOL set_text_field_text_internal(UITextFieldData *textfield_data, char *text, int encoding)
{
	if (encoding <= 0)
	{
		encoding = MULTI_LAN_ENCODING_UTF8;
	}
	convert_to_ucs2(text, -1, textfield_data->text, textfield_data->text_size, encoding);
	textfield_data->char_num = wcslen(textfield_data->text);

	return TRUE;
}

static void delete_text_field_private_data(UITextFieldData *textfield_data)
{
	if (textfield_data)
	{
		if (textfield_data->text)
		{
			gui_free(textfield_data->text);
		}
		gui_free(textfield_data);
	}
}


static BOOL move_caret_pos_to_line(UITextField *textfield, int line_index)
{
	LineRange line_range;
	int caret_offset;
	UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;

	textfield_data->prev_caret_line = get_caret_line_index(textfield_data);
	caret_offset = textfield_data->caret_pos - textfield_data->line_info[textfield_data->prev_caret_line].offset;

	line_range = textfield_data->line_info[line_index];
	textfield_data->caret_pos = line_range.offset + MIN(caret_offset, line_range.length - 1);

	return TRUE;
}

static BOOL text_field_insert_ucs2_string(UITextField *textfield, U16 *ucs2_string, int length)
{
	int ucs2_char_num;
	int free_space;
	int mem_move_size;
	int line_num_of_page;
	int i;
    UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;
	if (!textfield_data->editable)
	{
		return TRUE;
	}

	ucs2_char_num = length < 0 ? wcslen(ucs2_string) : length;
	//printf("ucs2_char_num = %d, text_size = %d caret_pos =%d\n", ucs2_char_num, textfield_data->text_size, textfield_data->caret_pos);
	free_space = (textfield_data->text_size - textfield_data->char_num - 1);
	ucs2_char_num = MIN(ucs2_char_num, free_space);
	//printf("ucs2_char_num = %d\n", ucs2_char_num);
	if (ucs2_char_num <= 0)
	{
		return FALSE;
	}

	if (textfield_data->caret_pos < 0 || textfield_data->caret_pos > textfield_data->char_num)
	{
		textfield_data->caret_pos = textfield_data->char_num;
	}
	mem_move_size = (textfield_data->char_num - textfield_data->caret_pos + 1) * sizeof(U16);
	if (mem_move_size > 0)
	{
		memmove(textfield_data->text + textfield_data->caret_pos + ucs2_char_num, textfield_data->text + textfield_data->caret_pos, mem_move_size);
	}
	for (i = 0; i < ucs2_char_num; i++)
	{
		textfield_data->text[textfield_data->caret_pos + i] = ucs2_string[i];
	}
	textfield_data->prev_caret_line = get_caret_line_index(textfield_data);
	textfield_data->char_num += ucs2_char_num;
	textfield_data->caret_pos += ucs2_char_num;
	text_layout(textfield_data, textfield->rect.size.width);
	line_num_of_page = get_line_num_of_page(textfield);
	if (textfield_data->start_line < get_caret_line_index(textfield_data) + 1 - line_num_of_page)
	{
		textfield_data->start_line = get_caret_line_index(textfield_data) + 1 - line_num_of_page;
	}

	return TRUE;
}

static int draw_text_field_line(UITextField *textfield, int line_index)
{
	int line_num_of_page;
	int caret_line_index;
	int offset, length;
	int line_left, line_top, line_height;//, line_width;
	//UIRect osd_rect;
	DrawTextParam draw_text_param;
	UIRect rect;
	int text_width;
	UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;

	line_height = get_text_field_line_height(textfield);
	line_num_of_page = get_line_num_of_page(textfield);
	caret_line_index = get_caret_line_index(textfield_data);

	if (line_index < textfield_data->start_line || line_index >= textfield_data->start_line + line_num_of_page)
	{
		return FALSE;
	}

	line_left = 0;//textfield->rect.point.x;
	line_top = (line_index - textfield_data->start_line) * line_height;//textfield->rect.point.y + (line_index - textfield_data->start_line) * line_height;
	//line_width = textfield_data->rect.size.width;

	offset = textfield_data->line_info[line_index].offset;
	length = textfield_data->line_info[line_index].length;

    //printf("line_left=%d line_top=%d size.width=%d size.height=%d \n",line_left,line_top);
	draw_text_param.font_size = textfield_data->font_size;
	draw_text_param.scale_x = textfield_data->font_scale_x;
	draw_text_param.scale_y = textfield_data->font_scale_y;
	draw_text_param.color = textfield_data->text_color;
	draw_text_param.direction = textfield_data->text_direction;
	draw_text_param.rtl_align_right = TRUE;
	draw_text_param.caret_pos = -1;

	//printf("need_draw_sidelong[%d]\n", textfield_data->need_draw_sidelong);
	rect.point.y = line_top + 2;
	rect.size.height = get_text_height(textfield_data->font_size, textfield_data->font_scale_y);
	if (textfield_data->align_center)
	{
		text_width = get_ucs2_text_width(textfield_data->text + offset, length, textfield_data->font_size, textfield_data->font_scale_x);
		rect.size.width = text_width;
		if (textfield_data->need_draw_sidelong)
		{
			rect.point.x = line_left + 2 + (textfield->rect.size.width - text_width) / 2 + textfield_data->sidelong_x_offset * line_index;
		}
		else
		{
			rect.point.x = line_left + 2 + (textfield->rect.size.width - text_width) / 2;
		}
	}
	else
	{
		if (textfield_data->need_draw_sidelong)
		{
			rect.size.width = textfield->rect.size.width - 4;
			rect.point.x = line_left + 2 + textfield_data->sidelong_x_offset * line_index;
		}
		else
		{
			rect.size.width = textfield->rect.size.width - 4;
			rect.point.x = line_left + 2;
		}
	}

	//printf("rect.point.x=%d rect.point.y=%d\n",rect.point.x, rect.point.y);
	if (textfield_data->editable && caret_line_index == line_index)
	{
		draw_text_param.caret_pos = textfield_data->caret_pos - offset;
	}
	draw_ucs2_text(&(textfield->painter), rect, textfield_data->text + offset, length, &draw_text_param);

	return TRUE;
}

static int draw_text_field_page(UITextField *textfield)
{
	int line_num_of_page;
	int i;
	UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;
	line_num_of_page = get_line_num_of_page(textfield);
    //printf("num_of_page[%d], start line[%d] line_num=%d\n", line_num_of_page, textfield_data->start_line, textfield_data->line_num);
	for (i = textfield_data->start_line; i < textfield_data->start_line + line_num_of_page && i < textfield_data->line_num; i++)
	{
		draw_text_field_line(textfield, i);
	}

	return TRUE;
}

#if 0
static int draw_text_field_caret(UITextField *textfield)
{
    UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;
	return draw_text_field_line(textfield, get_caret_line_index(textfield_data));
}
#endif

static int text_field_refresh(UITextField *textfield)
{
    UIRect inv_rect;
    int current_line ;
    int line_height = get_text_field_line_height(textfield);
    int refresh_line_num = 0;
    UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return -1;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;
    current_line = get_caret_line_index(textfield_data);

	if((current_line > textfield_data->line_num) || (textfield_data->prev_caret_line > textfield_data->line_num))
    {
        return -1;
    }
    if(current_line > textfield_data->prev_caret_line)//caret down
    {
        refresh_line_num = current_line - textfield_data->prev_caret_line + 1;
        inv_rect.point.y = textfield->rect.point.y + textfield_data->prev_caret_line*line_height;
    }
    else//caret up or left or right
    {
        refresh_line_num = textfield_data->prev_caret_line - current_line + 1;
        inv_rect.point.y = textfield->rect.point.y + current_line*line_height;
    }

    inv_rect.point.x = textfield->rect.point.x;
    inv_rect.size.width = textfield->rect.size.width;
    inv_rect.size.height = refresh_line_num*line_height;

    invalidate_window_rect(textfield, inv_rect);

    return 0;
}

static BOOL init_common_text_field(UITextField *textfield, int text_size, U32 text_color)
{
	UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;
    init_text_field_param(textfield_data);
    textfield_data->text_size = text_size + 1;
    textfield_data->text = (U16 *)gui_malloc(textfield_data->text_size * sizeof(U16));
	if (textfield_data->text == NULL)
	{
		return FALSE;
	}
	textfield_data->editable = TRUE;
	textfield_data->page_loop = FALSE;
	memset(textfield_data->text, 0, textfield_data->text_size * sizeof(U16));

    textfield_data->text_color = text_color;

	return TRUE;
}


UITextFieldData* new_text_field_private_data(void)
{
	UITextFieldData *textfield_data;

	textfield_data = (UITextFieldData *)gui_malloc(sizeof(UITextFieldData));
	if (textfield_data == NULL)
	{
		return NULL;
	}

	/* default parameter */
	gui_memset(textfield_data, 0, sizeof(UITextFieldData));

	return textfield_data;
}


//-------------------------------------------------------------------------------------------------
// window manager callback
//-------------------------------------------------------------------------------------------------
static int text_field_on_draw(UITextField *textfield)
{
    draw_text_field_page(textfield);
	return TRUE;
}

static BOOL text_field_on_destroy(UITextField *textfield)
{

	delete_text_field_private_data(textfield->private_data);

	return TRUE;
}


//-------------------------------------------------------------------------------------------------
// interface
//-------------------------------------------------------------------------------------------------
UITextField* create_text_field(const char *id)
{
	UIWindow *textfield;

	textfield = create_control(id, 0, 0, TEXTFIELD_DEFAULT_WIDTH, TEXTFIELD_DEFAULT_HEIGHT);
	if (textfield != NULL)
	{
		UITextFieldData *textfield_data = new_text_field_private_data();

		if (textfield_data == NULL)
		{
			gui_free(textfield);
			return NULL;
		}
		textfield->private_data = textfield_data;
		textfield->focus_stop = FALSE;
		textfield->class_name = TEXTFIELD_CLASS_NAME;
		textfield->on_draw = text_field_on_draw;
		textfield->on_destroy = text_field_on_destroy;

		init_common_text_field(textfield, TEXT_FIELD_TEXT_SIZE_DEFAULT, WHITE_COLOR);
	}
	return textfield;
}


BOOL set_text_field_text(UITextField *textfield, const char *text, int encoding)
{
    UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data || !text)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;

	set_text_field_text_internal(textfield_data, text, encoding);
    text_layout(textfield_data, textfield->rect.size.width);

	invalidate_window(textfield);

	return TRUE;
}

BOOL set_text_field_with_size(UITextField *textfield, int size)
{
	UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;

	if (textfield_data->text)
	{
		gui_free(textfield_data->text);
		textfield_data->text = NULL;
	}

	textfield_data->text_size = size + 1;
    textfield_data->text = (U16 *)gui_malloc(textfield_data->text_size * sizeof(U16));
	if (textfield_data->text == NULL)
	{
		return FALSE;
	}
	textfield_data->editable = TRUE;
	textfield_data->page_loop = FALSE;
	memset(textfield_data->text, 0, textfield_data->text_size * sizeof(U16));

	invalidate_window(textfield);
	return TRUE;
}


BOOL set_text_field_text_color(UITextField *textfield, U32 text_color)
{
    UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;

	textfield_data->text_color = text_color;

    invalidate_window(textfield);
	return TRUE;
}


BOOL set_text_field_editable(UITextField *textfield, BOOL editable)
{
    UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;

	if (textfield_data->editable == editable)
	{
		return TRUE;
	}
	textfield_data->editable = editable;
	if (!textfield_data->multi_line)
	{
		text_layout(textfield_data, textfield->rect.size.width);
	}
	if (editable)
	{
		textfield_data->page_loop = FALSE;
	}
	text_field_refresh(textfield);

	return TRUE;
}


BOOL set_text_field_multi_line(UITextField *textfield, BOOL multi_line)
{
    UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;

	if (textfield_data->multi_line == multi_line)
	{
		return TRUE;
	}
	textfield_data->multi_line = multi_line;
	text_layout(textfield_data, textfield->rect.size.width);
	invalidate_window(textfield);

	return TRUE;
}

BOOL set_text_field_font_size(UITextField *textfield, int font_size)
{
    UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;

	if (textfield_data->font_size == font_size)
	{
		return TRUE;
	}
	textfield_data->font_size = font_size;
	text_layout(textfield_data, textfield->rect.size.width);
	invalidate_window(textfield);

	return TRUE;
}

BOOL set_text_field_font_scale(UITextField *textfield, int scale_x, int scale_y)
{
	int save_font_scale_x;
	UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;

	if (textfield_data->font_scale_x == scale_x && textfield_data->font_scale_y == scale_y)
	{
		return TRUE;
	}
	save_font_scale_x = textfield_data->font_scale_x;
	textfield_data->font_scale_x = scale_x;
	textfield_data->font_scale_y = scale_y;
	if (save_font_scale_x != scale_x)
	{
		text_layout(textfield_data, textfield->rect.size.width);
	}
	invalidate_window(textfield);

	return TRUE;

}


BOOL set_text_field_line_spacing(UITextField *textfield, float line_spacing)
{
    UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;

	if (textfield_data->line_spacing == line_spacing)
	{
		return TRUE;
	}
	textfield_data->line_spacing = line_spacing;
	invalidate_window(textfield);

	return TRUE;
}


BOOL set_text_field_text_direction(UITextField *textfield, int text_direction)
{
    UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;

	if (textfield_data->text_direction == text_direction)
	{
		return TRUE;
	}
	if (text_direction != WRITE_DIRECTION_LTR && text_direction != WRITE_DIRECTION_RTL && text_direction != WRITE_DIRECTION_AUTO)
	{
		return FALSE;
	}
	textfield_data->text_direction = text_direction;
	invalidate_window(textfield);

	return TRUE;
}


BOOL set_text_field_align_center(UITextField *textfield, BOOL align_center)
{
    UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;

	if (textfield_data->align_center == align_center)
	{
		return TRUE;
	}
	textfield_data->align_center = align_center;
	invalidate_window(textfield);

	return TRUE;
}

BOOL set_text_field_page_loop(UITextField *textfield, BOOL loop)
{
	UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;

	if (textfield_data == NULL)
	{
		return FALSE;
	}
	if (!textfield_data->editable)
	{
		textfield_data->page_loop = loop;
	}
	return TRUE;
}

BOOL set_text_field_check_limit_callback(UITextField *textfield, CheckLimitFunc check_limit_cb)
{
	UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;

	textfield_data->char_num_can_be_inserted = check_limit_cb;

	return TRUE;
}

BOOL set_text_region_size(UITextField *textfield, U32 width, U32 height)
{

	if (!textfield || width > 1280 || height > 720)
	{
		return FALSE;
	}

	textfield->rect.size.height = height;
	textfield->rect.size.width = width;

	return TRUE;
}

int get_text_field_line_height(UITextField *textfield)
{
	int line_height = 0;
	int space = 4;
	UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return -1;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;

	if (textfield_data != NULL)
	{
		line_height = (int)(get_text_height(textfield_data->font_size, textfield_data->font_scale_y) * textfield_data->line_spacing);
	}

	return line_height + space;
}

int get_text_field_cur_page(UITextField *textfield)
{
	int line_num_of_page;
	UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;

	line_num_of_page = get_line_num_of_page(textfield);

	if (textfield_data == NULL || line_num_of_page == 0)
	{
		return 0;
	}
	return textfield_data->start_line / line_num_of_page;
}

int get_text_field_page_num(UITextField *textfield)
{
	int line_num_of_page;
	UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;
	line_num_of_page = get_line_num_of_page(textfield);

	if (textfield_data == NULL || line_num_of_page == 0)
	{
		return 0;
	}
	return (textfield_data->line_num + line_num_of_page - 1) / line_num_of_page;
}

int get_text_field_line_num(UITextField *textfield)
{
	UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;

	if (textfield_data == NULL)
	{
		return 0;
	}
	return textfield_data->line_num;
}

BOOL get_text_field_string(UITextField *textfield, int encoding, char *str_buf, int buf_size)
{
    UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;
	if (encoding <= 0 || encoding == MULTI_LAN_ENCODING_UTF8)
	{
		ucs2_to_utf8(textfield_data->text, textfield_data->char_num, str_buf, buf_size);
	}

	return TRUE;
}

int get_text_field_string_length(UITextField *textfield)
{
	UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;

	if (textfield_data == NULL)
	{
		return 0;
	}
	return ucs2_to_utf8_bytes_num(textfield_data->text, textfield_data->char_num);
}

int get_text_field_text_size(UITextField *textfield)
{
	UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;

	if (textfield_data == NULL)
	{
		return 0;
	}
	return textfield_data->text_size;
}

int get_text_field_slider_percent(UITextField *textfield)
{
	int line_num_of_page;
	int percent;
	UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;

	if (textfield_data == NULL || textfield_data->line_num <= 0)
	{
		return 0;
	}
	line_num_of_page = get_line_num_of_page(textfield);
	percent = line_num_of_page * 100 / textfield_data->line_num;
	percent = MAX(percent, 1);
	percent = MIN(percent, 100);

	return percent;
}

int get_text_field_bar_value(UITextField *textfield)
{
	int line_num_of_page;
	int max_start_line_num;
	UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;


	if (textfield_data == NULL || textfield_data->line_num <= 0)
	{
		return 0;
	}
	line_num_of_page = get_line_num_of_page(textfield);
	max_start_line_num = textfield_data->line_num - line_num_of_page;
	if (max_start_line_num <= 0)
	{
		return 0;
	}
	return textfield_data->start_line * 100 / max_start_line_num;
}

BOOL text_field_page_up(UITextField *textfield)
{
	int line_num_of_page = get_line_num_of_page(textfield);
	int caret_line_index_in_page;
	UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;

	if (!textfield_data->multi_line)
	{
		return FALSE;
	}
	if (textfield_data->start_line <= 0 && !textfield_data->page_loop)
	{
		return FALSE;
	}
	caret_line_index_in_page = (get_caret_line_index(textfield_data) - textfield_data->start_line) % line_num_of_page;
	if (!textfield_data->editable)
	{
		int cur_page_num, total_page_num;

		cur_page_num = get_text_field_cur_page(textfield);
		total_page_num = get_text_field_page_num(textfield);
		cur_page_num = (cur_page_num + total_page_num - 1) % total_page_num;
		textfield_data->start_line = cur_page_num * line_num_of_page;
	}
	else
	{
		if (textfield_data->start_line > line_num_of_page)
		{
			textfield_data->start_line -= line_num_of_page;
		}
		else
		{
			textfield_data->start_line = 0;
		}
	}
	if (textfield_data->editable)
	{
		move_caret_pos_to_line(textfield, textfield_data->start_line + caret_line_index_in_page);
	}
	invalidate_window(textfield);

	return TRUE;
}


BOOL text_field_page_down(UITextField *textfield)
{
	int line_num_of_page = get_line_num_of_page(textfield);
	int caret_line_index_in_page;
	UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;

	if (!textfield_data->multi_line)
	{
		return FALSE;
	}
	if (textfield_data->start_line >= textfield_data->line_num - line_num_of_page && !textfield_data->page_loop)
	{
		return FALSE;
	}
	caret_line_index_in_page = (get_caret_line_index(textfield_data) - textfield_data->start_line) % line_num_of_page;
	if (!textfield_data->editable)
	{
		int cur_page_num, total_page_num;

		cur_page_num = get_text_field_cur_page(textfield);
		total_page_num = get_text_field_page_num(textfield);
		cur_page_num = (cur_page_num + 1) % total_page_num;
		textfield_data->start_line = cur_page_num * line_num_of_page;
	}
	else
	{
		if (textfield_data->start_line < textfield_data->line_num - 2 * line_num_of_page)
		{
			textfield_data->start_line += line_num_of_page;
		}
		else
		{
			textfield_data->start_line = textfield_data->line_num - line_num_of_page;
		}
	}
	if (textfield_data->editable)
	{
		move_caret_pos_to_line(textfield, textfield_data->start_line + caret_line_index_in_page);
	}
	invalidate_window(textfield);

	return TRUE;
}

BOOL text_field_page_end(UITextField *textfield)
{
	int line_num_of_page = get_line_num_of_page(textfield);
	int caret_line_index_in_page;
	UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;

	if (!textfield_data->multi_line)
	{
		return FALSE;
	}
	if (textfield_data->start_line >= textfield_data->line_num - line_num_of_page && !textfield_data->page_loop)
	{
		return FALSE;
	}
	caret_line_index_in_page = (get_caret_line_index(textfield_data) - textfield_data->start_line) % line_num_of_page;
	if (!textfield_data->editable)
	{
		textfield_data->start_line = (get_text_field_page_num(textfield) - 1) * line_num_of_page;
	}
	else
	{
		textfield_data->start_line = textfield_data->line_num - line_num_of_page;
	}
	if (textfield_data->editable)
	{
		move_caret_pos_to_line(textfield, textfield_data->start_line + caret_line_index_in_page);
	}
	invalidate_window(textfield);

	return TRUE;
}


BOOL text_field_minimize(UITextField *textfield)
{
	int new_width, new_height;
	int line_num;
	int line_index;
	int offset, length;
	int text_width;
	UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;
	if (textfield_data == NULL)
	{
		return FALSE;
	}

	line_num = get_line_num_of_page(textfield);
	line_num = MIN(line_num, textfield_data->line_num);
	new_height = get_text_field_line_height(textfield) * line_num;
	new_width = 0;
	for (line_index = 0; line_index < textfield_data->line_num; line_index++)
	{
		offset = textfield_data->line_info[line_index].offset;
		length = textfield_data->line_info[line_index].length;
		text_width = get_ucs2_text_width(textfield_data->text + offset, length, textfield_data->font_size, textfield_data->font_scale_x);
		new_width = MAX(new_width, text_width);
	}
	new_width += 6;
	if (new_width < textfield->rect.size.width || new_height < textfield->rect.size.height)
	{
		set_window_size(textfield, ui_size(new_width, new_height));
	}

	return TRUE;
}

BOOL move_text_field_caret_forward(UITextField *textfield)
{
	int line_num_of_page;

	UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;
	if (!textfield_data->editable)
	{
		return FALSE;
	}
	if (textfield_data->caret_pos >= textfield_data->char_num)
	{
		return TRUE;
	}
	textfield_data->prev_caret_line = get_caret_line_index(textfield_data);
	textfield_data->caret_pos++;
	if (!textfield_data->multi_line)
	{
		text_layout(textfield_data, textfield->rect.size.width);
	}
	if (textfield_data->prev_caret_line != get_caret_line_index(textfield_data))
	{
		line_num_of_page = get_line_num_of_page(textfield);
		if (textfield_data->start_line < get_caret_line_index(textfield_data) + 1 - line_num_of_page)
		{
			textfield_data->start_line = get_caret_line_index(textfield_data) + 1 - line_num_of_page;
			invalidate_window(textfield);
		}
	}
	else
    {
        text_field_refresh(textfield);
    }

	return TRUE;
}


BOOL move_text_field_caret_backward(UITextField *textfield)
{
	UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;
	if (!textfield_data->editable)
	{
		return FALSE;
	}
	if (textfield_data->caret_pos <= 0)
	{
		return TRUE;
	}
	textfield_data->prev_caret_line = get_caret_line_index(textfield_data);
	textfield_data->caret_pos--;
	if (!textfield_data->multi_line)
	{
		text_layout(textfield_data, textfield->rect.size.width);
	}
	if (textfield_data->start_line > get_caret_line_index(textfield_data))
	{
		textfield_data->start_line = get_caret_line_index(textfield_data);
		invalidate_window(textfield);
	}
	else
    {
        text_field_refresh(textfield);
    }

	return TRUE;
}


BOOL move_text_field_caret_up(UITextField *textfield)
{
	UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;
	if (!textfield_data->editable || !textfield_data->multi_line)
	{
		return FALSE;
	}
	textfield_data->prev_caret_line = get_caret_line_index(textfield_data);
	if (textfield_data->prev_caret_line <= 0)
	{
		return TRUE;
	}
	move_caret_pos_to_line(textfield, textfield_data->prev_caret_line - 1);

	if (textfield_data->start_line > get_caret_line_index(textfield_data))
	{
		textfield_data->start_line = get_caret_line_index(textfield_data);
		invalidate_window(textfield);
	}
	else
    {
        text_field_refresh(textfield);
    }

	return TRUE;
}


BOOL move_text_field_caret_down(UITextField *textfield)
{
	UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;
	if (!textfield_data->editable || !textfield_data->multi_line)
	{
		return FALSE;
	}
	textfield_data->prev_caret_line = get_caret_line_index(textfield_data);
	if (textfield_data->prev_caret_line >= textfield_data->line_num - 1)
	{
		return TRUE;
	}
	move_caret_pos_to_line(textfield, textfield_data->prev_caret_line + 1);

	if (textfield_data->start_line < get_caret_line_index(textfield_data) - get_line_num_of_page(textfield) + 1)
	{
		textfield_data->start_line = get_caret_line_index(textfield_data) - get_line_num_of_page(textfield) + 1;
		invalidate_window(textfield);
	}
	else
    {
        text_field_refresh(textfield);
    }

	return TRUE;
}


BOOL move_text_field_caret_to_home(UITextField *textfield)
{
	return TRUE;
}


BOOL move_text_field_caret_to_end(UITextField *textfield)
{
	int line_num_of_page;
	UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;

	if (!textfield_data->editable)
	{
		return FALSE;
	}
	textfield_data->prev_caret_line = get_caret_line_index(textfield_data);
	textfield_data->caret_pos = textfield_data->char_num;
	if (!textfield_data->multi_line)
	{
		text_layout(textfield_data, textfield->rect.size.width);
	}
	if (textfield_data->prev_caret_line != get_caret_line_index(textfield_data))
	{
		line_num_of_page = get_line_num_of_page(textfield);
		if (textfield_data->start_line < get_caret_line_index(textfield_data) + 1 - line_num_of_page)
		{
			textfield_data->start_line = get_caret_line_index(textfield_data) + 1 - line_num_of_page;
			invalidate_window(textfield);
		}
		else
        {
            text_field_refresh(textfield);
        }
	}
	else
    {
        text_field_refresh(textfield);
    }


	return TRUE;
}


BOOL text_field_remove_character(UITextField *textfield)
{
	int delete_pos;
	int mem_move_size;
	int line_num_of_page;
	UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;

	if (!textfield_data->editable)
	{
		return TRUE;
	}
	/* delete character before caret */
	delete_pos = textfield_data->caret_pos - 1;
	if (delete_pos < 0 || delete_pos >= textfield_data->char_num)
	{
		return FALSE;
	}
	mem_move_size = (textfield_data->char_num - textfield_data->caret_pos + 1) * sizeof(U16);
	if (mem_move_size > 0)
	{
		memmove(textfield_data->text + delete_pos, textfield_data->text + delete_pos + 1, mem_move_size);
	}
	textfield_data->char_num--;
	textfield_data->caret_pos--;
	text_layout(textfield_data, textfield->rect.size.width);
	line_num_of_page = get_line_num_of_page(textfield);
	if (textfield_data->start_line > textfield_data->line_num - line_num_of_page)
	{
		if (textfield_data->line_num - line_num_of_page >= 0)
		{
			textfield_data->start_line = textfield_data->line_num - line_num_of_page;
		}
		else
		{
			textfield_data->start_line = 0;
		}
	}
    invalidate_window(textfield);
	return TRUE;
}


BOOL text_field_remove_all_character(UITextField *textfield)
{
	UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;

	if (!textfield_data->editable)
	{
		return TRUE;
	}
	memset(textfield_data->text, 0, textfield_data->text_size * sizeof(U16));
	textfield_data->caret_pos = textfield_data->char_num = 0;
	text_layout(textfield_data, textfield->rect.size.width);
	invalidate_window(textfield);

	return TRUE;
}

BOOL text_field_insert_string(UITextField *textfield, const char *utf8_string)
{
	int utf8_len;
	int can_inserted_char_num;
	U16 *ucs2_string;
	BOOL result;
	UITextFieldData *textfield_data;

	if (!textfield || !textfield->private_data)
	{
		return FALSE;
	}
	textfield_data = (UITextFieldData *)textfield->private_data;

	can_inserted_char_num = utf8_len = strlen(utf8_string);
	//printf("insert_string=%s char_num=%d\n", utf8_string, can_inserted_char_num);
	if (textfield_data->char_num_can_be_inserted != NULL)
	{
		can_inserted_char_num = textfield_data->char_num_can_be_inserted(textfield_data, utf8_string);
	}
	can_inserted_char_num = MIN(can_inserted_char_num, utf8_len);
	if (can_inserted_char_num <= 0)
	{
		return FALSE;
	}
	ucs2_string = gui_malloc((can_inserted_char_num + 1) * sizeof(U16));
	if (ucs2_string == NULL)
	{
		return FALSE;
	}
    //printf("before=%d caret_pos = %d\n", textfield_data->char_num, textfield_data->caret_pos);
	utf8_to_ucs2(utf8_string, can_inserted_char_num, ucs2_string, can_inserted_char_num + 1);
	result = text_field_insert_ucs2_string(textfield, ucs2_string, -1);

	//printf("after=%d caret_pos = %d\n", textfield_data->char_num, textfield_data->caret_pos);
	gui_free(ucs2_string);

	return result;
}


BOOL text_field_insert_character(UITextField *textfield, U16 ucs2_char)
{
	char utf8_string[4] = "";

	ucs2_to_utf8(&ucs2_char, 1, utf8_string, sizeof(utf8_string));
	return text_field_insert_string(textfield, (char *)utf8_string);
}

