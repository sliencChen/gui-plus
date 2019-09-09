/*
 * list.c
 *
 *  Created on: 2018-8-6
 *      Author: Brant
 */

#include <gui_window.h>
#include <control/list.h>
#include <control/help.h>


//-------------------------------------------------------------------------------------------------
// local variable
//-------------------------------------------------------------------------------------------------
char *item[MAX_COLUMN_NUM];

//-------------------------------------------------------------------------------------------------
// local functiop about bar
//-------------------------------------------------------------------------------------------------
static void init_default_list_bar(UIListData *list_data)
{
	if (list_data->direction == VER_DIRECTION)
		list_data->bar_data.enable_bar = TRUE;

	list_data->bar_data.bar_offset = 0;
	list_data->bar_data.bar_width = 14;
	list_data->bar_data.scroll_bar_color = WHITE_COLOR;
	list_data->bar_data.bk_color = RED_COLOR;
	list_data->bar_data.triangle_color = WHITE_COLOR;
	list_data->bar_data.scroll_bar_color = WHITE_COLOR;
}

static int init_bar_width(UIListData *list_data)
{
	DrawBmpParam bmp_param;
	int max_bar_width = list_data->bar_data.bar_width;

	if (gui_get_bmp(list_data->bar_data.up_bmp, &(bmp_param.bmp_info)))
	{
		max_bar_width = MAX(get_bmp_icon_width(list_data->bar_data.up_bmp), max_bar_width);
	}
	if (gui_get_bmp(list_data->bar_data.down_bmp, &(bmp_param.bmp_info)))
	{
		max_bar_width = MAX(get_bmp_icon_width(list_data->bar_data.down_bmp), max_bar_width);
	}
	if (gui_get_bmp(list_data->bar_data.scroll_bar_bmp, &(bmp_param.bmp_info)))
	{
		max_bar_width = MAX(get_bmp_icon_width(list_data->bar_data.scroll_bar_bmp), max_bar_width);
	}

	return max_bar_width;
}

static void set_scroll_bar_rect(UIRect *rect, int x, int y, int width, int height)
{
	rect->point.x = x;
	rect->point.y = y;
	rect->size.height = height;
	rect->size.width = width;
}


//-------------------------------------------------------------------------------------------------
// local function about column
//-------------------------------------------------------------------------------------------------
BOOL read_tv_favorites_item(struct _ui_list_data_ *list_data, int col_tag, int row_index, char *buffer, BOOL focus)
{
	DrawBmpParam bmp_param;
	const char *focus_bmp[] = {"LIGHTFAV1", "LIGHTFAV2", "LIGHTFAV3", "LIGHTFAV4"};
	const char *bmp_name[] = {"FAV1", "FAV2", "FAV3", "FAV4"};

	list_data->column_data[col_tag].tag = col_tag;
	list_data->column_data[col_tag].visiable = TRUE;
	switch (col_tag)
	{
		case 0:
			sprintf(buffer, "%04d", row_index);
			break;
		case 1:
			list_data->column_data[col_tag].proportion = 2;
			sprintf(buffer, "Channel Number %d", row_index);
			break;
		default:
			if (col_tag < 6)
				strcpy(buffer, focus ? focus_bmp[col_tag - 2] : bmp_name[col_tag - 2]);
			break;
	}

	if (gui_get_bmp(buffer, &(bmp_param.bmp_info)))
	{
		list_data->column_data[col_tag].min_width = get_bmp_icon_width(buffer);
	}
	else
	{
		list_data->column_data[col_tag].min_width = get_text_width(buffer, -1, 18, 100);
	}

	if (col_tag >= 6)
		list_data->column_data[col_tag].visiable = FALSE;

	return TRUE;
}

BOOL read_item_example(struct _ui_list_data_ *list_data, int col_tag, int row_index, char *buffer, BOOL focus)
{
	DrawBmpParam bmp_param;
	const char *focus_bmp[] = {"EPG", "NOFTA", "HD"};
	const char *bmp_name[] = {"NOEPG", "FTA", "NOHD"};

	if (col_tag < 5 )
	{
		list_data->column_data[col_tag].tag = col_tag;
		list_data->column_data[col_tag].visiable = TRUE;
		switch (col_tag)
		{
			case 0:
				sprintf(buffer, "%04d", row_index);
				break;
			case 1:
				list_data->column_data[col_tag].proportion = 2;
				sprintf(buffer, "Channel Number %d", row_index);
				break;
			default:
				strcpy(buffer, focus ? focus_bmp[col_tag - 2] : bmp_name[col_tag - 2]);
				break;
		}

		if (gui_get_bmp(buffer, &(bmp_param.bmp_info)))
		{
			list_data->column_data[col_tag].min_width = get_bmp_icon_width(buffer);
		}
		else
		{
			list_data->column_data[col_tag].min_width = get_text_width(buffer, -1, 18, 100);
		}

		return TRUE;
	}

	if (!list_data->column_data[col_tag].visiable)
		return FALSE;

	return TRUE;
}

static int get_visiable_column(UIListData *list_data)
{
	int index;
	int visiable_num = 0;

	for (index = 0; index < MAX_COLUMN_NUM; index++)
	{
		if (list_data->column_data[index].visiable)
		{
			visiable_num++;
		}
	}

	return visiable_num;
}

static int get_proportion_count(UIListData *list_data)
{
	int index;
	int total_num = 0;

	if (!list_data)
		return FALSE;

	for (index = 0; index < MAX_COLUMN_NUM; index++)
	{
		if (list_data->column_data[index].visiable)
		{
		    total_num += list_data->column_data[index].proportion;
		}
	}

	return total_num;
}

static int calculate_proportion_width(UIList *list, int font_size)
{
	UIListData *list_data;

	int index;
	int space_width, proportion_width;
	int zero_proportion_num = 0;
	int visiable_column_num = 0;
	int zero_proportion_width = 0;

	if (!list || !list->private_data)
		return FALSE;

	list_data = list->private_data;
	visiable_column_num = get_visiable_column(list_data);
	space_width = list_data->item_space * (visiable_column_num - 1);

	for (index = 0; index < MAX_COLUMN_NUM; index++)
	{
		if (!list_data->column_data[index].proportion && list_data->column_data[index].visiable)
		{
			zero_proportion_num++;
			/*if (width > list_data->column_data[index].min_width)
				zero_proportion_width += width;
			else*/
				zero_proportion_width += list_data->column_data[index].min_width;
		}
	}
	proportion_width = (list->min_size.width - space_width - list_data->offset * 2 - zero_proportion_width);
	if (visiable_column_num == zero_proportion_num)
		proportion_width = (list->min_size.width - space_width - list_data->offset * 2);

	if (proportion_width < 0)
	{
		gui_printf("the list width is too small \n");
		return FALSE;
	}

	return proportion_width;
}

static int calculate_column_x_pos(UIListData *list_data, int col_tag, int proportion_width, int zero_proportion_width)
{
	int index;
	int x_pos = 0;
	int proportion_total = get_proportion_count(list_data);
	int visiable_column_num = get_visiable_column(list_data);

	if (col_tag != -1)
		x_pos += list_data->offset;
	for (index = 0; index < col_tag; index++)
	{
		if (list_data->column_data[index].visiable)
		{
			if (list_data->column_data[index].proportion)
			{
				x_pos += proportion_width * list_data->column_data[index].proportion / proportion_total;
			}
			else
			{
				if (proportion_total)
				{
					/*if (zero_proportion_width > list_data->column_data[index].min_width)
					{
						x_pos += zero_proportion_width;
					}
					else*/
					{
						x_pos += list_data->column_data[index].min_width;
					}
				}
				else
				{
					x_pos += proportion_width / visiable_column_num;
				}
			}
			x_pos += list_data->item_space;
		}
	}

	return x_pos;
}

static int get_cur_tag_width(UIListData *list_data, int col_tag, int proportion_width, int zero_proportion_width)
{
	int proportion_total = get_proportion_count(list_data);
	int visiable_column_num = get_visiable_column(list_data);

	if (!list_data->column_data[col_tag].proportion && proportion_total != 0)
	{
		if (zero_proportion_width > list_data->column_data[col_tag].min_width)
			return zero_proportion_width;
		else
			return list_data->column_data[col_tag].min_width;
	}
	else if (proportion_total == 0)
		return proportion_width / visiable_column_num;
	else
		return proportion_width * list_data->column_data[col_tag].proportion / proportion_total;

}

static int get_first_visiable_column_pos(UIListData *list_data)
{
	int index;
	for (index = 0; index < MAX_COLUMN_NUM; index++)
	{
		if (list_data->column_data[index].visiable)
		{
			return ++index;
		}
	}
	return -1;
}

static void free_column_data(UIListData *list_data)
{
	int index;
	int max_data = get_visiable_column(list_data);

	for (index = 0; index < max_data; index++)
	{
		gui_free(item[index]);
	}

}

static void read_column_data(UIListData *list_data, int row_index, BOOL focus)
{
	int index;
	int item_num = 0;
	char *tag_buf[MAX_COLUMN_NUM];

	for (index = 0; index < MAX_COLUMN_NUM; index++)
	{
		tag_buf[index] = gui_malloc(sizeof(char) * MAX_BYTES_PER_COLUMN);
		if (list_data->read_column_data_function(list_data, index, row_index, tag_buf[index], focus) > 0)
		{
			item[item_num] = tag_buf[index];
			item_num++;
		}
	}

}


//-------------------------------------------------------------------------------------------------
// local function
//-------------------------------------------------------------------------------------------------
static UIRect get_item_rect(UIList *list, int index, BOOL direction)
{
	UIListData *list_data = list->private_data;
	int offset = index % list_data->page_max_item;

	if (direction)
		return ui_rect(list_data->iColStart, list_data->iRowStart + offset * (list_data->item_height + list_data->item_space), list->min_size.width, list_data->item_height);
	else
		return ui_rect(list_data->iColStart + offset * (list->min_size.width + list_data->item_space), list_data->iRowStart, list->min_size.width, list_data->item_height);

}

static BOOL is_in_same_page(UIListData *list_data, int index1, int index2)
{
	int page_max_item = list_data->page_max_item;

	if (page_max_item <= 0)
		return FALSE;

return (index1 / page_max_item) == (index2 / page_max_item);

}

static BOOL list_select_up_down(UIList *list, BOOL up)
{
	UIListData *list_data;

	if (!list || !list->private_data)
	{
		return FALSE;
	}

	list_data = list->private_data;
	list_data->prev_select = list_data->select;
	if (up)
	{
		list_data->select = (list_data->select + list_data->total - 1) % list_data->total;
	}
	else
	{
		list_data->select = (list_data->select + 1) % list_data->total;
	}
	list_data->cur_page = list_data->select / list_data->page_max_item;

	if (is_in_same_page(list_data, list_data->prev_select, list_data->select))
	{
		if (list_data->select != list_data->prev_select)
		{
			invalidate_window_rect(list, client_rect_to_win_rect(list, get_item_rect(list, list_data->prev_select, 1)));
			invalidate_window_rect(list, client_rect_to_win_rect(list, get_item_rect(list, list_data->select, 1)));
		}
	}
	else
	{
		invalidate_window(list);
	}

	if (list_data->bar_data.enable_bar)
	{
		UIRect scroll_bar_rect;
		int bar_col = list_data->iColStart + list->min_size.width + list_data->bar_data.bar_offset;
		int list_height = (list_data->item_height + list_data->item_space) * list_data->page_max_item - list_data->item_space;
		set_scroll_bar_rect(&scroll_bar_rect, bar_col, list_data->iRowStart, list_data->bar_data.bar_width, list_height);
		invalidate_window_rect(list, client_rect_to_win_rect(list, scroll_bar_rect));
	}

	return TRUE;
}

static BOOL list_select_page_up_down(UIList *list, BOOL up)
{
	UIListData *list_data;

	if (!list || !list->private_data)
	{
		return FALSE;
	}

	list_data = list->private_data;
	list_data->prev_select = list_data->select;
	if (up)
	{
		list_data->select = list_data->select - list_data->page_max_item;
		if (list_data->select < 0)
		{
			if ((list_data->total - 1) % list_data->page_max_item - list_data->prev_select >= 0)
				list_data->select = list_data->total - (list_data->total - 1) % list_data->page_max_item + list_data->prev_select - 1;
			else
				list_data->select = list_data->total - 1;
		}


	}
	else
	{
		list_data->select = list_data->select + list_data->page_max_item;
		if (list_data->select > list_data->total - 1 && list_data->cur_page != (list_data->total - 1) / list_data->page_max_item)
		{
			list_data->select = list_data->total - 1;
		}
		else if (list_data->cur_page == (list_data->total - 1) / list_data->page_max_item)
		{
			list_data->select = list_data->select % list_data->page_max_item;
		}
	}
	list_data->cur_page = list_data->select / list_data->page_max_item;

	invalidate_window(list);

	if (list_data->bar_data.enable_bar)
	{
		UIRect scroll_bar_rect;
		int bar_col = list_data->iColStart + list->min_size.width + list_data->bar_data.bar_offset;
		int list_height = (list_data->item_height + list_data->item_space) * list_data->page_max_item - list_data->item_space;
		set_scroll_bar_rect(&scroll_bar_rect, bar_col, list_data->iRowStart, list_data->bar_data.bar_width, list_height);
		invalidate_window_rect(list, client_rect_to_win_rect(list, scroll_bar_rect));
	}

	return TRUE;
}

static UIListData* new_list_private_data(void)
{
	UIListData *list_data;

	list_data = (UIListData *)gui_malloc(sizeof(UIListData));
	if (list_data == NULL)
		return NULL;

	/* default parameter */
	gui_memset(list_data, 0, sizeof(UIListData));

	list_data->total = 20;
	list_data->page_max_item = 5;
	list_data->cur_page = 0;
	list_data->total_page = (list_data->total + list_data->page_max_item - 1) / list_data->page_max_item;
	list_data->item_height = 36;
	list_data->item_space = 2;

	/* column and text data*/
	list_data->offset = 0;
	list_data->direction = VER_DIRECTION;
	list_data->read_column_data_function = read_item_example;

	/* bar data */
	init_default_list_bar(list_data);

	return list_data;
}

static void delete_list_private_data(UIListData *list_data)
{
	if (list_data)
	{
		gui_free(list_data);
	}
}


//-------------------------------------------------------------------------------------------------
// class event
//-------------------------------------------------------------------------------------------------
static BOOL list_select_up(UIList *list)
{
	return list_select_up_down(list, TRUE);
}

static BOOL list_select_down(UIList *list)
{
	return list_select_up_down(list, FALSE);
}

static BOOL list_select_page_up(UIList *list)
{
	return list_select_page_up_down(list, TRUE);
}

static BOOL list_select_page_down(UIList *list)
{
	return list_select_page_up_down(list, FALSE);
}

static int list_on_up_key(UIWindow *self, Event event, EventData data)
{
	list_select_up(self);

	return 0;
}

static int list_on_down_key(UIWindow *self, Event event, EventData data)
{
	list_select_down(self);

	return 0;
}

static int list_on_page_up_key(UIWindow *self, Event event, EventData data)
{
	list_select_page_up(self);

	return 0;
}

static int list_on_page_down_key(UIWindow *self, Event event, EventData data)
{
	list_select_page_down(self);

	return 0;
}

BOOL register_list_event(void)
{
	BOOL result = TRUE;

	result &= register_class_event_handler(LIST_CLASS_NAME, GUI_EVT_VK_UP, list_on_up_key);
	result &= register_class_event_handler(LIST_CLASS_NAME, GUI_EVT_VK_DOWN, list_on_down_key);
	result &= register_class_event_handler(LIST_CLASS_NAME, GUI_EVT_VK_LEFT, list_on_page_up_key);
	result &= register_class_event_handler(LIST_CLASS_NAME, GUI_EVT_VK_RIGHT, list_on_page_down_key);

	return result;
}

static BOOL register_list_class(void)
{
	static BOOL registered = FALSE;
	BOOL res;

	if (registered) return FALSE;

	res = register_list_event();
	registered = TRUE;

	return res;
}


//-------------------------------------------------------------------------------------------------
// window manager callback
//-------------------------------------------------------------------------------------------------
BOOL draw_list_bar(UIList *list)
{
	if (!list || !list->private_data)
	    return FALSE;

	DrawBmpParam bmp_param;
	UIPainter *painter = &(list->painter);
	UIListData *list_data = list->private_data;
	UIColor bk_color = list_data->bar_data.bk_color;
	UIRect scroll_bar_rect, panel_rect, triangle_rect;
	UIColor triangle_color = list_data->bar_data.triangle_color;
	UIColor scroll_bar_color = list_data->bar_data.scroll_bar_color;

	int iMarkLength;
	int iMinBarHeight = 10;
	int iHeight = 0, iRow = 0;
	int total = list_data->total;
	int select = list_data->select;
	int bar_col = list_data->iColStart + list->min_size.width + list_data->bar_data.bar_offset;
	int iTriangleMiddle = list_data->bar_data.bar_width <= 10 ? 5 : list_data->bar_data.bar_width / 2;
	int list_height = (list_data->item_height + list_data->item_space) * list_data->page_max_item - list_data->item_space;

	if (gui_get_bmp(list_data->bar_data.up_bmp, &(bmp_param.bmp_info)))
		iMarkLength = get_bmp_icon_height(list_data->bar_data.up_bmp);
	else
		iMarkLength = 2 + iTriangleMiddle + 3;

	set_scroll_bar_rect(&scroll_bar_rect, bar_col, list_data->iRowStart, list_data->bar_data.bar_width, list_height);
	panel_rect = triangle_rect = scroll_bar_rect;
	panel_rect.point.y = scroll_bar_rect.point.y + iMarkLength;
	panel_rect.size.height = (scroll_bar_rect.size.height - iMarkLength * 2);

	if (total > 1)
	{
		if (iMinBarHeight >= (panel_rect.size.height / total))
			iHeight = iMinBarHeight;
		else
			iHeight = panel_rect.size.height / total;

		iRow = panel_rect.point.y + (double)(select) / (double)(total - 1) * (panel_rect.size.height - iHeight);
	}
	else
	{
		iHeight = panel_rect.size.height;
		iRow = panel_rect.point.y;
	}

	/* draw up_bmp */
	if (gui_get_bmp(list_data->bar_data.up_bmp, &(bmp_param.bmp_info)))
	{
		triangle_rect.size.height = get_bmp_icon_height(list_data->bar_data.up_bmp);
		bmp_param.mode = DRAW_BMP_MODE_NORMAL;
		draw_bitmap(painter, triangle_rect, &bmp_param);
	}
	else
	{
		triangle_rect.size.height = 2;
		draw_rect(painter, triangle_rect, triangle_color);

		triangle_rect.point.x = scroll_bar_rect.point.x + iTriangleMiddle;
		triangle_rect.point.y = scroll_bar_rect.point.y + iTriangleMiddle + 2;
		draw_triangle(painter, triangle_rect.point.x, triangle_rect.point.y, 2, iTriangleMiddle, triangle_color);
	}

	/* draw down_bmp */
	if (gui_get_bmp(list_data->bar_data.down_bmp, &(bmp_param.bmp_info)))
	{
		triangle_rect.point.y = scroll_bar_rect.point.y + scroll_bar_rect.size.height - get_bmp_icon_height(list_data->bar_data.down_bmp);
		triangle_rect.size.height = get_bmp_icon_height(list_data->bar_data.up_bmp);
		bmp_param.mode = DRAW_BMP_MODE_NORMAL;
		draw_bitmap(painter, triangle_rect, &bmp_param);
	}
	else
	{
		triangle_rect.size.height = 2;
		triangle_rect.point.x = scroll_bar_rect.point.x;
		triangle_rect.point.y = scroll_bar_rect.point.y + scroll_bar_rect.size.height - 2;
		draw_rect(painter, triangle_rect, triangle_color);

		triangle_rect.point.x += iTriangleMiddle;
		triangle_rect.point.y = scroll_bar_rect.point.y + scroll_bar_rect.size.height - iTriangleMiddle - 2;
		draw_triangle(painter, triangle_rect.point.x, triangle_rect.point.y, 3, iTriangleMiddle, triangle_color);
	}

	draw_rect(painter, panel_rect, bk_color); // draw background
	/* draw scroll bar */
	if (gui_get_bmp(list_data->bar_data.scroll_bar_bmp, &(bmp_param.bmp_info)))
	{
		panel_rect.size.height = iHeight;
		panel_rect.point.y = iRow;
		bmp_param.mode = DRAW_BMP_MODE_VAR_SIZE;
		draw_bitmap(painter, panel_rect, &bmp_param);
	}
	else
	{
		panel_rect.size.height = iHeight;
		panel_rect.point.y = iRow;
		draw_rect(painter, panel_rect, scroll_bar_color);
	}

	return TRUE;
}

BOOL draw_list_item_detail(UIList *list, void *theme_param, int index, BOOL focus)
{
    DrawTextParam param;
	DrawBmpParam bmp_param;
	UIRect rect, text_rect, bmp_rect;
	UIPainter *painter = &(list->painter);
	UIListData *list_data = list->private_data;
	UIListThemeCommonData *theme_data = (UIListThemeCommonData *)theme_param;

	int x_pos, i;
	BOOL ret = TRUE;
	int proportion_width = calculate_proportion_width(list, theme_data->font_size);
	int zero_proportion_width = MAX(get_text_width("0000", -1, theme_data->font_size, 100), get_bmp_icon_width("NOHD"));

	if (list_data == NULL)
	{
		ret = FALSE;
        goto END;
	}

	default_draw_text_param(&param);
	param.font_size = theme_data->font_size;
	param.color = focus ? theme_data->light_font_color : theme_data->font_color;
	rect = get_item_rect(list, index, list_data->direction);
	bmp_rect = rect;

	for (i = 0; i < MAX_COLUMN_NUM; i++)
	{
		if (list_data->column_data[i].visiable)
		{
			x_pos = rect.point.x + calculate_column_x_pos(list_data, i, proportion_width, zero_proportion_width);
			if (gui_get_bmp(item[i], &(bmp_param.bmp_info)))
			{
				bmp_rect.point.x = x_pos;
				bmp_rect.point.y = rect.point.y + (rect.size.height - get_bmp_icon_height(item[i])) / 2;
				bmp_param.mode = DRAW_BMP_MODE_NORMAL;
				draw_bitmap(painter, bmp_rect, &bmp_param);
			}
			else
			{
				text_rect.point.x = x_pos;
				text_rect.point.y = rect.point.y + (rect.size.height - get_text_height(theme_data->font_size, 100)) / 2;
				text_rect.size.width = get_cur_tag_width(list_data, i, proportion_width, zero_proportion_width);
				text_rect.size.height = get_text_height(theme_data->font_size, 100);
				draw_text(painter, text_rect, item[i], -1, &param);
			}
		}
	}

END:
	free_column_data(list_data);
	return ret;

}

BOOL draw_list_theme_bmp_with_rect(UIList *list, UIListThemeBmpWithRect *theme_param, int index, BOOL focus)
{
	BOOL is_have_bmp = FALSE;
	int first_visiable_pos;
	int zero_proportion_width, proportion_width, field_width;
	const char *now_bmp = focus ? theme_param->focus_bmp : theme_param->unfocus_bmp;

	DrawTextParam param;
	DrawBmpParam bmp_param;
	UIRect rect, save_rect;
	UIPainter *painter = &(list->painter);
	UIListData *list_data = list->private_data;
	UIColor color = focus ? theme_param->light_bk_color : theme_param->bk_color;
	UIColor color1 = focus ? theme_param->light_bk_color1 : theme_param->bk_color1;

	if (list_data == NULL)
		return FALSE;

	read_column_data(list_data, index, focus);
	default_draw_text_param(&param);
	param.font_size = theme_param->common_data.font_size;
	param.color = focus ? theme_param->common_data.light_font_color : theme_param->common_data.font_color;

	if (gui_get_bmp(theme_param->focus_bmp, &(bmp_param.bmp_info)) || gui_get_bmp(theme_param->unfocus_bmp, &(bmp_param.bmp_info)))
		is_have_bmp = TRUE;

	list_data->bar_data.bar_width = init_bar_width(list_data);
	rect = get_item_rect(list, index, list_data->direction);
	zero_proportion_width = MAX(get_text_width("0000", -1, theme_param->common_data.font_size, 100), get_bmp_icon_width("NOHD"));
	proportion_width = calculate_proportion_width(list, theme_param->common_data.font_size);
	first_visiable_pos = get_first_visiable_column_pos(list_data);
	field_width = calculate_column_x_pos(list_data, first_visiable_pos, proportion_width, zero_proportion_width);

	/* list height redefine */
	if (is_have_bmp)
	{
		rect.size.height = MAX(get_bmp_icon_height(theme_param->focus_bmp), get_bmp_icon_height(theme_param->unfocus_bmp));
		list_data->item_height = rect.size.height;
		if (list_data->item_height <= get_bmp_icon_height("NOHD"))
		{
			list_data->item_height = get_bmp_icon_height("NOHD");
			rect.size.height = get_bmp_icon_height("NOHD");
		}
	}

	/* draw diffrent are rect by tag*/
	save_rect = rect;
	save_rect.point.x = rect.point.x;
	save_rect.size.width = field_width;
	draw_rect(painter, save_rect, color);
	save_rect.point.x = rect.point.x + field_width;
	save_rect.size.width = rect.size.width - field_width;
	draw_rect(painter, save_rect, color1);

	/* draw bmp */
	if(gui_get_bmp(now_bmp, &(bmp_param.bmp_info)))
	{
		bmp_param.mode = DRAW_BMP_MODE_VAR_SIZE;
		bmp_param.mode_param.copy_pt = ui_point(get_bmp_icon_width(now_bmp) / 2, get_bmp_icon_height(now_bmp) / 2);
		draw_bitmap(painter, rect, &bmp_param);
	}

	if (index < list_data->total)
		draw_list_item_detail(list, theme_param, index, focus);

	if (list_data->bar_data.enable_bar)
	{
		draw_list_bar(list);
	}

	return TRUE;
}

BOOL draw_list_theme_bmp(UIList *list, UIListThemeBmp *theme_param, int index, BOOL focus)
{
	UIRect bmp_rect;
	DrawTextParam param;
	DrawBmpParam bmp_param;
	UIPainter *painter = &(list->painter);
	UIListData *list_data = list->private_data;

	const char *now_bmp = focus ? theme_param->focus_bmp : theme_param->unfocus_bmp;

	if (list_data == NULL)
		return FALSE;

	read_column_data(list_data, index, focus);
	default_draw_text_param(&param);
	param.font_size = theme_param->common_data.font_size;
	param.color = focus ? theme_param->common_data.light_font_color : theme_param->common_data.font_color;

	/* draw bmp */
	bmp_rect = get_item_rect(list, index, list_data->direction);
	list_data->bar_data.bar_width = init_bar_width(list_data);

	if (gui_get_bmp(now_bmp, &(bmp_param.bmp_info)))
	{
		bmp_rect.size.height = get_bmp_icon_height(now_bmp);
		list_data->item_height = get_bmp_icon_height(now_bmp);
		if (get_bmp_icon_height(now_bmp) <= get_bmp_icon_height("NOHD"))
		{
			list_data->item_height = get_bmp_icon_height("NOHD");
			bmp_rect.size.height = get_bmp_icon_height("NOHD");
		}
		bmp_param.mode = DRAW_BMP_MODE_VAR_SIZE;
		bmp_param.mode_param.copy_pt = ui_point(get_bmp_icon_width(now_bmp) / 2, get_bmp_icon_height(now_bmp) / 2);
		draw_bitmap(painter, bmp_rect, &bmp_param);
	}

	if (index < list_data->total)
		draw_list_item_detail(list, theme_param, index, focus);

	if (list_data->bar_data.enable_bar)
	{
		draw_list_bar(list);
	}

	return TRUE;
}

BOOL draw_list_theme_rect(UIList *list, UIListThemeRect *theme_param, int index, BOOL focus)
{
	DrawTextParam param;
	UIRect rect, save_rect;
	int first_visiable_pos;
	UIPainter *painter = &(list->painter);
	UIListData *list_data = list->private_data;
	int zero_proportion_width, proportion_width, field_width;
	UIColor color = focus ? theme_param->light_bk_color : theme_param->bk_color;
	UIColor color1 = focus ? theme_param->light_bk_color1 : theme_param->bk_color1;

	if (list_data == NULL)
		return FALSE;

	read_column_data(list_data, index, focus);
	default_draw_text_param(&param);
	param.font_size = theme_param->common_data.font_size;
	param.color = focus ? theme_param->common_data.light_font_color : theme_param->common_data.font_color;
	list_data->bar_data.bar_width = init_bar_width(list_data);

	/* draw diffrent are rect by tag*/
	rect = get_item_rect(list, index, list_data->direction);
	save_rect = rect;
	zero_proportion_width = MAX(get_text_width("0000", -1, theme_param->common_data.font_size, 100), get_bmp_icon_width("NOHD"));
	proportion_width = calculate_proportion_width(list, theme_param->common_data.font_size);
	first_visiable_pos = get_first_visiable_column_pos(list_data);
	field_width = calculate_column_x_pos(list_data, first_visiable_pos, proportion_width, zero_proportion_width);

	save_rect.point.x = rect.point.x;
	save_rect.size.width = field_width;
	draw_rect(painter, save_rect, color);
	save_rect.point.x = rect.point.x + field_width;
	save_rect.size.width = rect.size.width - field_width;
	draw_rect(painter, save_rect, color1);

	if (index < list_data->total)
		draw_list_item_detail(list, theme_param, index, focus);

	if (list_data->bar_data.enable_bar)
	{
		draw_list_bar(list);
	}

	return TRUE;
}

static BOOL list_on_draw(UIList *self)
{
	int i, index;
	UIListData *list_data = self->private_data;
	UIListThemeBmpWithRect default_theme;

	default_theme.bk_color						= 0xff2e5571;
	default_theme.bk_color1						= 0xff2b80a5 ;
	default_theme.light_bk_color				= 0xffc9a12e;
	default_theme.light_bk_color1				= 0xff35ceff ;
	default_theme.common_data.font_color		= 0xffe2e2e2;
	default_theme.common_data.light_font_color	= 0xff080408 ;
	default_theme.common_data.font_size			= 18;
//	default_theme.unfocus_bmp					="FILE";
//	default_theme.focus_bmp						="MENUICONLIGHT4";

	for (i = 0; i < list_data->page_max_item; i++)
	{
		index = list_data->cur_page * list_data->page_max_item + i;
		draw_list_theme_bmp_with_rect(self, &default_theme, index, index == list_data->select);
	}
	return TRUE;
}

static BOOL list_on_destroy(UIList *self)
{
	delete_list_private_data(self->private_data);

	return TRUE;
}


//-------------------------------------------------------------------------------------------------
// interface
//-------------------------------------------------------------------------------------------------
UIList* create_list(const char *id)
{
	UIWindow *list = NULL;

	register_list_class();
	list = create_control(id, 0, 0, LIST_DEFAULT_WIDTH, LIST_DEFAULT_HEIGHT);
	if (list != NULL)
	{
		UIListData *list_data;

		list_data = new_list_private_data();
		if (list_data == NULL)
		{
			gui_free(list);
			return NULL;
		}
		list->focus_stop = TRUE;
		list->min_size.width = 525;
		list->on_draw = list_on_draw;
		list->private_data = list_data;
		list->class_name = LIST_CLASS_NAME;
		list->on_destroy = list_on_destroy;
	}

	return list;
}

BOOL set_list_total_num(UIList *list, int total)
{
	UIListData *list_data;

	if (!list || !list->private_data)
		return FALSE;

	list_data = list->private_data;

	if (total >= 0)
	{
		list_data->total = total;
		if (list_data->total < list_data->page_max_item)
			list_data->page_max_item = list_data->total;
		return TRUE;
	}
	gui_printf("incorrect total \n");
	return FALSE;
}

BOOL set_list_item_height(UIList *list, int height)
{
   UIListData *list_data;

	if (!list || !list->private_data)
		return FALSE;

	list_data = list->private_data;

	if (height >= 0)
	{
		list_data->item_height = height;
		return TRUE;
	}
	gui_printf("incorrect height \n");
	return FALSE;
}

BOOL set_list_item_space(UIList *list, int space)
{
	UIListData *list_data;

	if (!list || !list->private_data)
		return FALSE;

	list_data = list->private_data;

	if (space >= 0)
	{
		list_data->item_space = space;
		return TRUE;
	}

	gui_printf("incorrect space \n");
	return FALSE;
}

BOOL set_list_page_max_item(UIList *list, int max_item)
{
	UIListData *list_data;

	if (!list || !list->private_data)
		return FALSE;

	list_data = list->private_data;

	if (max_item > 0 && max_item <= list_data->total)
	{
		list_data->page_max_item = max_item;
		return TRUE;
	}
	else if (max_item > list_data->total)
	{
		list_data->page_max_item = list_data->total;
		return TRUE;
	}

	gui_printf("wrong max_item \n");
	return FALSE;
}

BOOL set_list_position(UIList *list, int col, int row)
{
	UIListData *list_data;
	if (!list || !list->private_data)
		return FALSE;

	list_data = list->private_data;

	if (col >= 0 && row >= 0)
	{
		list_data->iColStart = col;
		list_data->iRowStart = row;
		return TRUE;
	}

	gui_printf("wrong position \n");
	return FALSE;
}

BOOL set_list_bar_color(UIList *list, UIColor back_color, UIColor scroll_bar_color)
{
	UIListData *list_data;

	if (!list || !list->private_data)
		return FALSE;

	list_data = list->private_data;

	list_data->bar_data.bk_color = back_color;
	list_data->bar_data.scroll_bar_color = scroll_bar_color;

	return TRUE;
}

BOOL set_list_bar_scroll_bar_bmp(UIList *list, const char *bmp_name)
{
	UIListData *list_data;
	DrawBmpParam bmp_param;

	if (!list || !list->private_data)
		return FALSE;

	list_data = list->private_data;

	if (gui_get_bmp(bmp_name, &(bmp_param.bmp_info)))
	{
		list_data->bar_data.scroll_bar_bmp = bmp_name;
		return TRUE;
	}
	return FALSE;
}

BOOL enable_list_bar(UIList *list, BOOL enable)
{
	UIListData *list_data;

	if (!list || !list->private_data)
		return FALSE;

	list_data = list->private_data;
	list_data->bar_data.enable_bar = enable;
	return TRUE;
}

BOOL set_list_bar_offset(UIList *list, int bar_offset)
{
	UIListData *list_data;

	if (!list || !list->private_data)
		return FALSE;

	list_data = list->private_data;
	list_data->bar_data.bar_offset = bar_offset;
	return TRUE;
}

BOOL set_list_offset(UIList *list, int offset)
{
	UIListData *list_data;

	if (!list || !list->private_data)
		return FALSE;

	list_data = list->private_data;
	list_data->offset = offset;
	return TRUE;
}

BOOL set_list_total_width(UIList *list, int width)
{
	UIListData *list_data;

	if (!list || width <= 0)
		return FALSE;

	list->min_size.width = width;

	return TRUE;
}

BOOL set_list_read_column_data_function(UIList *list, BOOL (*read_column_data_function)(struct _ui_list_data_ *list_data, int col_tag, int row_index, char *buffer, BOOL focus))
{
	UIListData *list_data;

	if (!list || !list->private_data || read_column_data_function == NULL)
		return FALSE;

	list_data = list->private_data;
	list_data->read_column_data_function = read_column_data_function;

	return TRUE;
}

