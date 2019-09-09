/*
 * help.c
 *
 *  Created on: 2018-9-18
 *      Author: Grace
 */

#include <gui_window.h>
#include <control/help.h>


#if 1
static UIHelpInter help_internal_temp;
//-------------------------------------------------------------------------------------------------
// local variable
//-------------------------------------------------------------------------------------------------
static UIHelpData* new_help_private_data(void)
{
	UIHelpData *help_data;

	help_data = (UIHelpData *)gui_malloc(sizeof(UIHelpData));
	if (help_data == NULL)
	{
		return NULL;
	}

	/* default parameter */
	gui_memset(help_data, 0, sizeof(UIHelpData));

	return help_data;
}

static void delete_help_private_data(UIHelpData *help_data)
{
	if (help_data)
	{
		gui_free(help_data);
	}
}

//-------------------------------------------------------------------------------------------------
// interface
//-------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// check bmp name(for string type bmp name)
//-----------------------------------------------------------------------------
BOOL check_bmp_name(bmp_icon_type name, bmp_icon_type cur_name)
{
#if 1//GET_BMP_BY_NAME
	if (name != NULL && cur_name != NULL)
	{
		if (strcmp(name, cur_name) == 0)
		{
			return TRUE;
		}
	}
#else
	if (name == cur_name)
	{
		return TRUE;
	}
#endif

	return FALSE;
}

BOOL check_circle_bmp(bmp_icon_type name, int type)
{
	bmp_icon_type bmp_array[] = {BMP_RED_CIRCLE, BMP_GREEN_CIRCLE, BMP_YELLOW_CIRCLE, BMP_BLUE_CIRCLE};
	int i = 0;

	for (i = 0; i < ARRAY_SIZE(bmp_array); i++)
	{
		if (check_bmp_name(name, bmp_array[i]))
		{
			return TRUE;
		}
	}

	return FALSE;
}

static BOOL is_color_help_item(UIHelpItem help_item)
{
	BOOL is_color_item = FALSE;
	int loop;
	bmp_icon_type cur_bmp;

	for (loop = 0; loop < help_item.bmp_num && help_item.bmp_info != NULL; loop++)
	{
		cur_bmp = help_item.bmp_info[loop].bmp;
		if (check_bmp_name(BMP_BLUE_CIRCLE, cur_bmp) || check_bmp_name(BMP_RED_CIRCLE, cur_bmp)
			|| check_bmp_name(BMP_GREEN_CIRCLE, cur_bmp) || check_bmp_name(BMP_YELLOW_CIRCLE, cur_bmp))
		{
			is_color_item = TRUE;
			break;
		}
	}

	return is_color_item;
}

static BOOL is_sample_help_item(UIHelpItem help_item)
{
	BOOL b_item = FALSE;
	int i_loop;
	bmp_icon_type bmp_array[] = {BMP_MV, BMP_CH, BMP_EXIT, BMP_RECALL, BMP_REC_HELP, BMP_SUB_HELP, BMP_FAV, BMP_OK};
	bmp_icon_type e_cur_bmp;
	int i = 0;
    int check_bmp_num = ARRAY_SIZE(bmp_array);

	for (i_loop = 0; i_loop < help_item.bmp_num && help_item.bmp_info != NULL; i_loop++)
	{
		e_cur_bmp = help_item.bmp_info[i_loop].bmp;

		for (i = 0; i < check_bmp_num; i++)
		{
			if (check_bmp_name(e_cur_bmp, bmp_array[i]))
			{
				b_item = TRUE;
			}
		}
	}
	return b_item;
}

static void remove_help_invalid_item_gui(UIHelpData *help_data)
{
	int loop, loop2;
	BOOL remove;

	if (NULL == help_data)
	{
		return;
	}

	for (loop = 0; loop < help_data->item_number; loop++)
	{
		remove = need_remove_invalid_help_item(is_sample_help_item(help_data->help_item[loop]));

		if (!help_data->help_item[loop].visible)
		{
			remove = TRUE;
		}
		if (remove)
		{
			for (loop2 = loop; loop2 < help_data->item_number - 1; loop2++)
			{
				help_data->help_item[loop2] = help_data->help_item[loop2 + 1];
			}
			loop--;
			help_data->item_number--;
		}
	}
}

static ICON_TYPE get_help_icon_type_gui(ICON_TYPE icon_type, bmp_icon_type cur_bmp)
{
	ICON_TYPE icon_type_ret = ICON_TYPE_BMP;

	if (conver_bmp_to_text_gui(NULL, 0, cur_bmp) > 0)
	{
		icon_type_ret = ICON_TYPE_TEXT;
	}
	else if (check_circle_bmp(cur_bmp, 0))
	{
		icon_type_ret = get_help_item_icon_color_type();
	}
	else if (icon_type != ICON_TYPE_BMP)
	{
		icon_type_ret = icon_type;
	}

	return icon_type_ret;
}

static BOOL have_color_help_gui(UIHelpItem *help_item, int item_number)
{
	BOOL have_color_help = FALSE;
	int loop;

	for (loop = 0; loop < item_number; loop++)
	{
		if (is_color_help_item(help_item[loop]) && help_item[loop].visible)
		{
			have_color_help = TRUE;
			break;
		}
	}

	return have_color_help;
}

static int draw_help_item_bmp_gui(UIHelp *help, UIHelpItem *help_item, int index)
{
	UIPainter *painter = &(help->painter);
	bmp_icon_type cur_bmp = help_item->bmp_info[index].bmp;
	UIRect rect = ui_rect(help_item->rect.point.x, help_item->rect.point.y, help_item->rect.size.width, help_item->rect.size.height);
	DrawBmpParam param;
	UIBmp bmp_info;
	int item_width = 0;

	if (!gui_get_bmp(cur_bmp,&bmp_info))
	{
		return -1;
	}
	if (help_item->bmp_info)
	{
		param.bmp_info = bmp_info;
		param.mode = help_item->bmp_info[index].mode;
		param.mode_param = help_item->bmp_info[index].mode_param;

		draw_bitmap(painter, rect, &param);
		invalidate_window(help);
	}

	return get_bmp_icon_width(cur_bmp);
}

static int draw_help_circle_bmp_gui(UIHelp *help, UIHelpItem *help_item, int index)
{
	UIPainter *painter = &(help->painter);
	UIRect rect;
	bmp_icon_type cur_bmp = help_item->bmp_info[index].bmp;
	UIColor color_temp = help_item->bmp_info[index].icon_color;
	BOOL is_color_circle = FALSE;
	UIHelpData *help_data = help->private_data;
	short radius = help_data->help_internal.radius;
	bmp_icon_type bmp_array[] = {BMP_BLUE_CIRCLE, BMP_RED_CIRCLE, BMP_GREEN_CIRCLE, BMP_YELLOW_CIRCLE};
	U32 color_array[] = {BLUE_COLOR, RED_COLOR, GREEN_COLOR, YELLOW_COLOR};
	int i = 0;

	if (use_rect_to_replace_circle_for_help_gui())
	{
		painter->mode = COVER_BACKGROUND;
		rect = ui_rect(help_item->rect.point.x, help_item->rect.point.y, help_data->help_internal.radius, help_data->help_internal.radius);
		draw_rect(painter, rect, color_temp);
		radius = radius / 2;
	}
	else
	{
		rect = ui_rect(help_item->rect.point.x, help_item->rect.point.y, 2 * help_data->help_internal.radius, 2 * help_data->help_internal.radius);
		draw_round_rect(painter, rect, radius, color_temp, TRUE, 0);
	}
	invalidate_window(help);

	return radius * 2;
}

static int draw_help_text_bmp_gui(UIHelp *help, UIHelpItem *help_item, int index)
{
	char str_temp[100] = "";
	UIColor color = help_item->string_color;
	UIPainter *painter = &(help->painter);
	DrawTextParam param;
	UIRect text_rect = ui_rect(help_item->rect.point.x, help_item->rect.point.y, help->rect.size.width, get_text_height(18, 100));
	bmp_icon_type cur_bmp = help_item->bmp_info[index].bmp;

	if (conver_bmp_to_text_gui(str_temp, sizeof(str_temp), cur_bmp) == 0)
	{
		return 0;
	}

	if (check_bmp_name(BMP_REC_HELP, cur_bmp))
	{
		color = RED_COLOR;
	}

	default_draw_text_param(&param);
	param.color = color;

	draw_text(painter, text_rect, str_temp, -1, &param);
	invalidate_window(help);

	return get_text_width(str_temp, -1, -1, 100);
}

static int draw_help_extra_bmp_gui(UIHelp *help, UIHelpItem *help_item, int index)
{
	int item_width = 0;
	UIColor color = help_item->bmp_info[index].icon_color;
	bmp_icon_type cur_bmp = help_item->bmp_info[index].bmp;
	UIHelpData *help_data = help->private_data;
	UIPainter *painter = &(help->painter);
	int triangle_length = 6;
	UIRect rect = ui_rect(help_item->rect.point.x, help_item->rect.point.y, help->rect.size.width, help->rect.size.height);

	if (check_bmp_name(cur_bmp, BMP_MV))
	{
		draw_triangle(painter, rect.point.x + triangle_length, rect.point.y, 1, triangle_length, color);
		draw_triangle(painter, rect.point.x + triangle_length * 3, rect.point.y, 0, triangle_length, color);
		draw_triangle(painter, rect.point.x + triangle_length + triangle_length, rect.point.y - triangle_length, 2, triangle_length, color);
		draw_triangle(painter, rect.point.x + triangle_length + triangle_length, rect.point.y + triangle_length, 3, triangle_length, color);
		item_width = 3 * triangle_length;

		invalidate_window(help);
	}

	return item_width;
}

static int draw_help_bmp_common_gui(UIHelp *help, UIHelpItem *help_item, int index)
{
	int item_width = 0;

	if (!help || !help_item)
	{
		return 0;
	}

	switch (help_item->bmp_info[index].icon_type)
	{
		case ICON_TYPE_EXTRA:
			item_width = draw_help_extra_bmp_gui(help, help_item, index);
			break;
		case ICON_TYPE_COLOR:
			item_width = draw_help_circle_bmp_gui(help, help_item, index);
			break;
		case ICON_TYPE_TEXT:
			item_width = draw_help_text_bmp_gui(help, help_item, index);
			break;
		default:
			item_width = draw_help_item_bmp_gui(help, help_item, index);
			break;
	}

	return item_width;
}

static int get_help_item_bmp_height(UIHelpData *help_data, UIHelpItem help_item, int index)
{
	int item_height = 0;

	if (!help_data)
	{
		return 0;
	}

	switch (help_item.bmp_info[index].icon_type)
	{
		case ICON_TYPE_EXTRA:
			item_height = 0;
			break;
		case ICON_TYPE_COLOR:
			if (use_rect_to_replace_circle_for_help_gui())
			{
				item_height = help_data->help_internal.radius;
			}
			else
			{
				item_height = help_data->help_internal.radius * 2;
			}
			break;
		case ICON_TYPE_TEXT:
			item_height = 28;
			break;
		default:
			item_height = get_bmp_icon_height(help_item.bmp_info[index].bmp);
			break;
	}

	return item_height;
}

static int draw_help_item_bmp(UIHelp *help, UIHelpItem help_item)
{
	int loop = 0;
	int item_width = 0;
	int max_text_width = 0;
	UIHelpData *help_data = help->private_data;
	UIRect rect = ui_rect(help_item.rect.point.x, help_item.rect.point.y, help_item.rect.size.width, help_item.rect.size.height);

	if (NULL == help_data || help_item.bmp_num <= 0)
	{
		return item_width;
	}

	for (loop = 0; loop < help_item.bmp_num && help_item.bmp_info != NULL; loop++)
	{
		if (loop > 0)
		{
			item_width += help_data->help_internal.bmp_gap;
		}
		help_item.rect.point.x += help_data->help_internal.bmp_gap;
		help_item.rect.point.y += (help_item.rect.size.height - get_help_item_bmp_height(help_data, help_item, loop)) / 2;
		item_width += draw_help_bmp_common_gui(help, &help_item, loop);
	}

	return item_width;
}

static BOOL draw_help_item_string_gui(UIHelp *help, UIHelpItem *help_item)
{
	UIPainter *painter = &(help->painter);
	DrawTextParam param;
	UIRect rect = ui_rect(help_item->rect.point.x, help_item->rect.point.y, help_item->rect.size.width, help_item->rect.size.height);

	default_draw_text_param(&param);
	param.color = help_item->string_color;

	draw_text(painter, rect, help_item->string, -1, &param);
	invalidate_window(help);

	return TRUE;
}

static void draw_help_item_string(UIHelp *help, UIHelpItem help_item, U32 bmp_width)
{
	UIHelpData *help_data = help->private_data;
	UIRect rect = ui_rect(help_item.rect.point.x, help_item.rect.point.y, help_item.rect.size.width, help_item.rect.size.height);
	int max_text_width = 0;

	if (NULL == help_data || help_item.string <= 0)
	{
		return;
	}

	if (help_item.string != NULL)
	{
		max_text_width = help_item.max_width - bmp_width - help_data->help_internal.bmp_text_space;
		if (help_item.max_width <= 0)	/* no limit */
		{
			max_text_width = help_item.string_width;
		}
		if (help_data->help_internal.bmp_text_space > 0)
		{
			help_item.rect.point.x += bmp_width + help_data->help_internal.bmp_text_space;
		}
		else
		{
			help_item.rect.point.x += (help_item.rect.size.width - help_item.string_width) / 2;
		}
		help_item.rect.point.y += (help_item.rect.size.height - get_text_height(18, 100)) / 2;
		draw_help_item_string_gui(help, &help_item);
	}
}

static BOOL draw_default_help_item_fun(UIHelp *help)
{
	UIHelpData *help_data = (UIHelpData *)help->private_data;
	BOOL ret = FALSE;
	int index = 0;
	int item_width = 0;

	if (NULL == help_data)
	{
		return ret;
	}

	for (index = 0; index < help_data->item_number; index++)
	{
		item_width = draw_help_item_bmp(help, help_data->help_item[index]);
		draw_help_item_string(help, help_data->help_item[index], item_width);
	}

	return ret;
}

static void help_set_default_help_info(UIHelpData *help_data)
{
	BOOL have_color_item = have_color_help_gui(help_data->help_item, help_data->item_number);

	memset(&help_data->help_internal, 0, sizeof(UIHelpInter));

	help_data->help_internal.draw_style = get_help_default_draw_style();
	help_data->help_internal.bmp_text_space = get_default_bmp_text_space_gui();
	help_data->help_internal.bmp_gap = 5;
	help_data->help_internal.align_style = get_help_default_align_style(have_color_item);
	help_data->help_internal.align_to_grid = get_help_default_align_to_grid(have_color_item);

	help_data->help_internal.item_gap = 5;
	help_data->help_internal.border_space = 5;
	help_data->help_internal.radius = use_rect_to_replace_circle_for_help_gui() ? 20 : 7;
	help_data->help_internal.sort_color_item = TRUE;
	help_data->help_internal.skin_style = get_help_default_skin_style();
	help_data->help_internal.draw_default_item_fun = draw_default_help_item_fun;

	memcpy(&help_internal_temp, &help_data->help_internal, sizeof(UIHelpInter));
}

static int get_color_key_index_gui(UIHelpItem help_item)
{
	int loop_index = 0;
	int color_index = 0;
	bmp_icon_type bmp_array[] = {BMP_RED_CIRCLE, BMP_GREEN_CIRCLE, BMP_YELLOW_CIRCLE, BMP_BLUE_CIRCLE};
	U32 index_array[] = {0, 1, 2, 3};
	int i = 0;

	for (loop_index = 0; loop_index < help_item.bmp_num && help_item.bmp_info != NULL; loop_index++)
	{
		for (i = 0; i < sizeof(bmp_array)/sizeof(bmp_array[0]); i++)
		{
			if (check_bmp_name(help_item.bmp_info[loop_index].bmp, bmp_array[i]))
			{
				color_index = index_array[i];
				break;
			}
		}
	}

	return color_index;
}

int GetColorKeyPosition(COLOR_KEY iKey, int iRCU)
{
	int iPos = -1;

	switch (GetRCUColorKeyOrder(iRCU))
    {
        case COLOR_KEY_ORDER_RBGY:
            switch (iKey)
            {
                case RED_KEY:
                    iPos = 0;
                    break;
                case BLUE_KEY:
                    iPos = 1;
                    break;
                case GREEN_KEY:
                    iPos = 2;
                    break;
                case YELLOW_KEY:
                    iPos = 3;
                    break;
            }
            break;
        default:	//COLOR_KEY_ORDER_RGYB
            switch (iKey)
            {
                case RED_KEY:
                    iPos = 0;
                    break;
                case BLUE_KEY:
                    iPos = 3;
                    break;
                case GREEN_KEY:
                    iPos = 1;
                    break;
                case YELLOW_KEY:
                    iPos = 2;
                    break;
            }
            break;
    }
	return iPos;
}

int GetRCUColorKeyOrder(int rcu_order)
{
    int iOrder = COLOR_KEY_ORDER_RGYB;

	if (-1 == rcu_order)
	{
		//rcu_order = GetRcuOrder();//Need input rcu_order by designer tool;
	}
	switch (rcu_order)
	{
		case 0:
			iOrder = COLOR_KEY_ORDER_RBGY;
			break;
		default:
			break;
	}
	return iOrder;
}

static void sort_help_color_item_gui(UIHelpData *help_data)
{
	int color_key_pos[4];
	UIHelpItem temp;
	int i, j;

	switch (GetRCUColorKeyOrder(-1))
	{
		case COLOR_KEY_ORDER_RBGY:
			color_key_pos[0] = GetColorKeyPosition(RED_KEY, -1);
			color_key_pos[1] = GetColorKeyPosition(BLUE_KEY, -1);
			color_key_pos[2] = GetColorKeyPosition(GREEN_KEY, -1);
			color_key_pos[3] = GetColorKeyPosition(YELLOW_KEY, -1);
			break;
		default:
			color_key_pos[0] = GetColorKeyPosition(RED_KEY, -1);
			color_key_pos[1] = GetColorKeyPosition(GREEN_KEY, -1);
			color_key_pos[2] = GetColorKeyPosition(YELLOW_KEY, -1);
			color_key_pos[3] = GetColorKeyPosition(BLUE_KEY, -1);
			break;
	}

	for (i = 0; i < help_data->item_number - 1; i++)
	{
		if (is_color_help_item(help_data->help_item[i]))
		{
			for (j = i + 1; j < help_data->item_number; j++)
			{
				if (is_color_help_item(help_data->help_item[j]))
				{
					if (color_key_pos[get_color_key_index_gui(help_data->help_item[i])] > color_key_pos[get_color_key_index_gui(help_data->help_item[j])])
					{
						temp = help_data->help_item[i];
						help_data->help_item[i] = help_data->help_item[j];
						help_data->help_item[j] = temp;
					}
				}
			}
		}
	}
}

static int get_help_item_bmp_width(UIHelpData *help_data, int item_index, int bmp_index)
{
	int width = 0;
	ICON_TYPE icon_type;
	bmp_icon_type cur_bmp;
	DrawBmpMode mode;

	if (help_data == NULL)
	{
		return 0;
	}

	icon_type = help_data->help_item[item_index].bmp_info[bmp_index].icon_type;
	cur_bmp = help_data->help_item[item_index].bmp_info[bmp_index].bmp;
	mode = help_data->help_item[item_index].bmp_info[bmp_index].mode;
	switch (icon_type)
	{
		case ICON_TYPE_TEXT:
			width = conver_bmp_to_text_gui(NULL, 0, cur_bmp);
			break;
		case ICON_TYPE_COLOR:
			if (use_rect_to_replace_circle_for_help_gui())
			{
				width = help_data->help_internal.radius;
			}
			else
			{
				width = help_data->help_internal.radius * 2;
			}
			break;
		default:
			width = get_bmp_icon_width(cur_bmp);
			break;
	}

	return width;
}

static int get_help_items_max_line_width(UIHelpData *help_data, char start_index, char count)
{
	int sum_width = 0;
	int loop;
	int item_width;

	if ((NULL == help_data) || (count <= 0) || (start_index < 0) || (start_index > (help_data->item_number - 1)))
	{
		return 0;
	}

	for (loop = 0; loop < count; loop++)
	{
		item_width = 0;
		item_width = help_data->help_item[start_index + loop].item_width;
		sum_width += item_width;
	}
	sum_width += help_data->help_internal.border_space * 2 + (help_data->help_internal.item_gap * (count - 1));

	return sum_width;
}

static void get_help_item_col_max_width(UIHelpData *help_data, char *col_num, int max_col, short *col_width, int total_row)
{
	int loop, jloop, kloop;
	int temp, index;
	short max_col_width;

	if (help_data == NULL)
	{
		return;
	}

	for (loop = 0; loop < max_col; loop++)
	{
		temp = 0;
		max_col_width = 0;
		for (jloop = 0; jloop < total_row; jloop++)
		{
			if (loop < col_num[jloop])
			{
				index = 0;
				for (kloop = 0; kloop < jloop; kloop++)
				{
					index += col_num[kloop];
				}
				index += loop;	/* current item index */
				temp = help_data->help_item[index].item_width;
				if (max_col_width < temp)
				{
					max_col_width = temp;
				}
			}
		}
		col_width[loop] = max_col_width;
	}
}

static void init_ui_help_param_gui(UIHelp *help)
{
	int  loop;
	UIHelpData *help_data = (UIHelpData *)help->private_data;

	memset(help_data, 0, sizeof(UIHelpData));
	for (loop = 0; loop < MAX_HELP_ITEM; loop++)
	{
		help_data->help_item[loop].bmp_num = 1;
		help_data->help_item[loop].max_width = -1;	/* no limit */
	}
}

static int get_help_item_index_by_bmp_gui(UIHelp *help, bmp_icon_type bmp)
{
	int item_index, icon_index;
	UIHelpData *help_data = (UIHelpData *)help->private_data;

	if (NULL == help_data || !check_bmp_valid(bmp, 1))
	{
		return -1;
	}
	for (item_index = 0; item_index < help_data->item_number; item_index++)
	{
		for (icon_index = 0; icon_index < help_data->help_item[item_index].bmp_num; icon_index++)
		{
			if (check_bmp_name(help_data->help_item[item_index].bmp_info[icon_index].bmp, bmp))
			{
				return item_index;
			}
		}
	}
	return -1;
}

static int get_help_item_index_by_string_gui(UIHelp *help, char *string)
{
	UIHelpData *help_data = (UIHelpData *)help->private_data;
	int item_index;

	if (NULL == help_data || NULL == string)
	{
		return -1;
	}
	for (item_index = 0; item_index < help_data->item_number; item_index++)
	{
		if (strcmp(help_data->help_item[item_index].string, string) == 0)
		{
			return item_index;
		}
	}
	return -1;
}

static BOOL set_help_item_icon_type_by_index_gui(UIHelp *help, int item_index, ICON_TYPE type, UIColor color)
{
	UIHelpData *help_data = (UIHelpData *)help->private_data;

	if (item_index < 0 || NULL == help_data)
	{
		return FALSE;
	}
	if (item_index < 0 || item_index >= help_data->item_number)
	{
		return FALSE;
	}
	help_data->help_item[item_index].bmp_info[0].icon_type = type;
	help_data->help_item[item_index].bmp_info[0].icon_color = color;

	return TRUE;
}

static BOOL set_help_item_string_color_by_index_gui(UIHelp *help, int item_index, UIColor color)
{
	UIHelpData *help_data = (UIHelpData *)help->private_data;

	if (item_index < 0 || NULL == help_data)
	{
		return FALSE;
	}
	if (item_index < 0 || item_index >= help_data->item_number)
	{
		return FALSE;
	}
	help_data->help_item[item_index].string_color = color;

	return TRUE;
}

static void get_help_item_rect_point(UIHelpData *help_data, char *col_num, int max_col_num, short *col_max_width, short *row_width, int total_row, UIRect area_rect)
{
	int loop = 0, jloop = 0, index = 0;
	int max_width = 0;
	int item_space = 0;
	int temp_left = 0, temp_top = 0;
	int row_height = area_rect.size.height / total_row;
	BOOL align_to_grid = help_data->help_internal.align_to_grid;;
	int item_gap = help_data->help_internal.item_gap * max_col_num;

	for (loop = 0; loop < max_col_num; loop++)
	{
		max_width += col_max_width[loop];
	}

	if (max_width > (area_rect.size.width - help_data->help_internal.border_space * 2))
	{
		align_to_grid = FALSE;
	}

	if (align_to_grid && FULL_JUST == help_data->help_internal.align_style)
	{
		help_data->help_internal.align_style = LEFT_JUST;
	}

	if (max_col_num > 0)
	{
		item_space = (area_rect.size.width - max_width - help_data->help_internal.border_space * 2 - item_gap) / max_col_num;
	}
	index = 0;
	for (loop = 0; loop < total_row; loop++)
	{
		if (!align_to_grid)	//if not align to grid,calculate ItemSpace every time.
		{
			item_space = 0;
			if (FULL_JUST == help_data->help_internal.align_style && col_num[loop] > 1)
			{
				item_space = (area_rect.size.width - row_width[loop]) / (col_num[loop] - 1);
			}
			else if (col_num[loop] > 0)
			{
				item_space = (area_rect.size.width - row_width[loop]) / col_num[loop];
			}
		}

		if (LEFT_JUST == help_data->help_internal.align_style || FULL_JUST == help_data->help_internal.align_style)
		{
			temp_left = 0;
		}
		else if (RIGHT_JUST == help_data->help_internal.align_style)
		{
			temp_left = item_space;
		}
		else
		{
			temp_left = item_space / 2;
		}

		temp_top = loop * row_height;

		for (jloop = 0; jloop < col_num[loop]; jloop++)
		{
			help_data->help_item[index + jloop].rect.point.x = temp_left + help_data->help_internal.border_space;
			help_data->help_item[index + jloop].rect.size.height = row_height;
			help_data->help_item[index + jloop].rect.point.y = temp_top;
			if (help_data->help_internal.bmp_text_space > 0)
			{
				if (align_to_grid)
				{
					help_data->help_item[index + jloop].rect.size.width = col_max_width[jloop];
				}
				else
				{
					help_data->help_item[index + jloop].rect.size.width = help_data->help_item[index + jloop].item_width;
				}
				temp_left += help_data->help_item[index + jloop].rect.size.width + item_space;
			}
			else
			{
				help_data->help_item[index + jloop].rect.size.width = col_max_width[jloop] + (area_rect.size.width - max_width - help_data->help_internal.border_space * 2 - item_gap) / max_col_num;
				temp_left += help_data->help_item[index + jloop].rect.size.width + help_data->help_internal.item_gap;
			}
		}
		index += col_num[loop];
	}
}

static int get_help_item_col_row_info(UIHelpData *help_data, char *col_num, int *max_col_num, short *row_width, int area_width)
{
	int total_row = 1;
	int max_line_width = 0;
	int max_col_num_temp = 0;
	int loop = 0, temp = 0;
	int total_item_number = 0;

	if (help_data == NULL)
	{
		return 0;
	}

	total_item_number = help_data->item_number;
	max_line_width = get_help_items_max_line_width(help_data, 0, help_data->item_number);
	row_width[0] = max_line_width;
	max_col_num_temp = total_item_number;

	while(max_line_width > area_width)
	{
		total_row++;
		if (total_item_number % total_row == 0)
		{
			max_col_num_temp = total_item_number / total_row;
		}
		else
		{
			max_col_num_temp = total_item_number / total_row + 1;
		}
		max_line_width = 0;
		for (loop = 0; loop < total_row; loop++)
		{
			if (loop * max_col_num_temp + max_col_num_temp <= total_item_number)
			{
				temp = get_help_items_max_line_width(help_data, loop * max_col_num_temp, max_col_num_temp);
			}
			else
			{
				temp = get_help_items_max_line_width(help_data, loop * max_col_num_temp, total_item_number - loop * max_col_num_temp);
			}
			row_width[loop] = temp;
			if (max_line_width < temp)
			{
				max_line_width = temp;
			}
		}
	}

	for (loop = 0; loop < total_row; loop++)
	{
		if ((loop + 1) * max_col_num_temp < total_item_number)
		{
			col_num[loop] = max_col_num_temp;
		}
		else
		{
			col_num[loop] = total_item_number - loop * max_col_num_temp;
		}
	}

	*max_col_num = max_col_num_temp;

	return total_row;
}

static void get_help_item_bmp_string_width(UIHelp *help)
{
	UIHelpData *help_data = (UIHelpData *)help->private_data;
	int index = 0, index1;
	bmp_icon_type cur_bmp;
	ICON_TYPE cur_icon_type;
	int bmp_width = 0;

	if (help_data == NULL)
	{
		return;
	}

	for (index = 0; index < help_data->item_number; index++)
	{
		for (index1 = 0; index1 < help_data->help_item[index].bmp_num; index1++)
		{
			if (help_data->help_item[index].bmp_info == NULL)
			{
				break;
			}
			cur_bmp = help_data->help_item[index].bmp_info[index1].bmp;
			cur_icon_type = help_data->help_item[index].bmp_info[index1].icon_type;
			if ((check_bmp_valid(cur_bmp, 1)))
			{
				bmp_width = get_help_item_bmp_width(help_data, index, index1);
			}
			if (index1 > 0)
			{
				bmp_width += help_data->help_internal.bmp_gap;
			}

		}

		if (help_data->help_item[index].string != NULL)
		{
			help_data->help_item[index].string_width = get_text_width(help_data->help_item[index].string, -1, -1, -1);
		}
		if (help_data->help_internal.bmp_text_space > 0)
		{
			help_data->help_item[index].item_width = bmp_width + help_data->help_internal.bmp_text_space + help_data->help_item[index].string_width;// + help_data->help_internal.item_gap;
		}
		else
		{
			help_data->help_item[index].item_width = MAX(bmp_width, help_data->help_item[index].string_width);// + help_data->help_internal.item_gap;
		}
		if (help_data->help_item[index].max_width > 0)
		{
			help_data->help_item[index].item_width = MIN(help_data->help_item[index].item_width, help_data->help_item[index].max_width);
		}
	}
}

static void get_help_item_rect(UIHelp *help)
{
	UIHelpData *help_data = (UIHelpData *)help->private_data;
	int loop = 0, jloop = 0, index = 0;
	int max_line_width = 0;
	int temp = 0;
	int max_col_num = 0;
	short col_max_width[MAX_HELP_ITEM] = {0};
	short row_width[MAX_HELP_ITEM] = {0};
	char col_num[MAX_HELP_ITEM];
	int total_row = 0;

	if (help_data == NULL)
	{
		return;
	}

	get_help_item_bmp_string_width(help);
	total_row = get_help_item_col_row_info(help_data, col_num, &max_col_num, row_width, help->rect.size.width);
	get_help_item_col_max_width(help_data, col_num, max_col_num, col_max_width, total_row);
	get_help_item_rect_point(help_data, col_num, max_col_num, col_max_width, row_width, total_row, help->rect);
}

static void reset_help_item_info(UIHelp *help)
{
	UIHelpData *help_data = (UIHelpData *)help->private_data;
	int loop = 0, loop1 = 0;
	bmp_icon_type cur_bmp;
	ICON_TYPE icon_type;

	memcpy(&help_internal_temp, &help_data->help_internal, sizeof(UIHelpInter));
	for (loop = 0; loop < help_data->item_number; loop++)
	{
		for (loop1 = 0; loop1 < help_data->help_item[loop].bmp_num; loop1++)
		{
			cur_bmp = help_data->help_item[loop].bmp_info[loop1].bmp;
			cur_bmp = conver_extra_help_to_bmp_gui(cur_bmp);
			if (cur_bmp != help_data->help_item[loop].bmp_info[loop1].bmp)
			{
				help_data->help_item[loop].bmp_info[loop1].bmp = cur_bmp;
				if (help_data->help_internal.bmp_text_space <= 0)
				{
					help_data->help_item[loop].bmp_info[loop1].mode = DRAW_BMP_MODE_VAR_SIZE;
					help_data->help_item[loop].bmp_info[loop1].mode_param.copy_pt = ui_point(get_bmp_icon_width(cur_bmp) / 2, 0xFFFF);
				}
			}
			icon_type = get_help_icon_type_gui(ICON_TYPE_BMP, cur_bmp);
			if (help_data->help_item[loop].bmp_info[loop1].icon_type == 0 && icon_type != 0)
			{
				help_data->help_item[loop].bmp_info[loop1].icon_type = icon_type;
			}
		}
	}
}

static BOOL help_item_on_draw(UIHelp *self)
{
	UIHelpData *help_data = (UIHelpData *)self->private_data;
	BOOL ret = FALSE;
	int index = 0;

	if (NULL == help_data)
	{
		return ret;
	}

	if (help_data->help_internal.draw_default_item_fun)
	{
		help_data->help_internal.draw_default_item_fun(self);
	}

	return TRUE;
}

static BOOL help_item_on_destroy(UIHelp *self)
{
	delete_help_private_data(self->private_data);

	return TRUE;
}


//-------------------------------------------------------------------------------------------------
// interface
//-------------------------------------------------------------------------------------------------
UIHelp* create_help(char *id)
{
	UIWindow *help;

	help = create_control(id, 40, 600, 1200, 110);
	if (help != NULL)
	{
		UIHelpData *help_data = new_help_private_data();

		if (help_data == NULL)
		{
			gui_free(help);
			return NULL;
		}

		help->private_data = help_data;
		help->focus_stop = FALSE;
		help->has_trans = FALSE;
		help->class_name = HELP_CLASS_NAME;
		help->on_draw = help_item_on_draw;
		help->on_destroy = help_item_on_destroy;
	}
	return help;
}

int conver_bmp_to_text_gui(char *buf, int buf_len, bmp_icon_type cur_bmp)
{
	char buffer_temp[32] = "";
	char *str_format = "[%s]";
	bmp_icon_type bmp_array[] = {BMP_0_9, BMP_F1, BMP_F2, BMP_F3, BMP_F4, BMP_INFO, BMP_2_4_6_8, BMP_2_5_7_0, BMP_4_6, BMP_REC_HELP, BMP_SUB_HELP,
			BMP_FIND, BMP_MENU, BMP_RECALL, BMP_EPG_HELP, BMP_ZOOM_HELP, BMP_VOL_HELP, BMP_0, BMP_FULLSCREEN, BMP_SHIFT};
	char *str_value[] = {"0-9", "F1", "F2", "F3", "F4", "INFO", "2][4][6][8", "2][5][7][0", "4][6", "REC", "SUB",
			"FIND", "MENU", "RECALL", "EPG", "ZOOM", "VOL+/-", "0", "TV/R", "SHIFT"};
	int i = 0;

	if (cur_bmp == BMP_MAX)
	{
		return 0;
	}

	for (i = 0; i < sizeof(bmp_array)/sizeof(bmp_array[0]); i++)
	{
		if (check_bmp_name(cur_bmp, bmp_array[i]))
		{
			snprintf(buffer_temp, sizeof(buffer_temp), str_format, str_value[i]);
			break;
		}
	}

	if (buf && buf_len > 0)
	{
		memset(buf, 0, buf_len);
		snprintf(buf, buf_len, buffer_temp);
	}

	return get_text_width(buffer_temp, -1, -1, -1);
}

void init_help_params_gui(UIHelp *help, bmp_icon_type *bmp, string_table *string, char num)
{
	int loop;
	char mini_item_num;
	UIHelpData *help_data = (UIHelpData *)help->private_data;

	init_ui_help_param_gui(help);

	mini_item_num = MIN(num, MAX_HELP_ITEM);
	help_data->item_number = mini_item_num;
	for (loop = 0; loop < mini_item_num; loop++)
	{
		help_data->help_item[loop].bmp_info[0].bmp = bmp[loop];
		help_data->help_item[loop].bmp_num = 1;
		help_data->help_item[loop].visible = TRUE;
		help_data->help_item[loop].bmp_info[0].icon_type = ICON_TYPE_BMP;
		help_data->help_item[loop].string_color = get_help_item_string_color_by_bmp_gui(help_data->help_item[loop].bmp_info[0].bmp);;
		help_data->help_item[loop].bmp_info[0].icon_color = 0xFFFF0000;//Need input by designer tool
		if (string[loop] < MAX_HELP_ITEM)//not init pStrings
		{
			snprintf(help_data->help_item[loop].string, HELP_STRING_MAX_LENGTH, "%s", "Test");//need get string info from string data
		}
		else
		{
			snprintf(help_data->help_item[loop].string, HELP_STRING_MAX_LENGTH, "%s", "Unknown");
		}
	}

	help_set_default_help_info(help_data);
	sort_help_color_item_gui(help_data);
}

void set_help_item_align_to_grid(UIHelp *help, BOOL align_to_grid)
{
	UIHelpData *help_data = (UIHelpData *)help->private_data;

	if (help_data)
	{
		help_data->help_internal.align_to_grid = align_to_grid;		//default is TRUE
	}
}

void set_help_item_align_style_gui(UIHelp *help, U32 align_style)
{
	UIHelpData *help_data = (UIHelpData *)help->private_data;

	if (help_data)
	{
		help_data->help_internal.align_style = align_style;	//text_justif_mode_t//default is CENTER_JUST
	}
}

void set_help_item_draw_style(UIHelp *help, U32 draw_style)
{
	UIHelpData *help_data = (UIHelpData *)help->private_data;

	if (help_data)
	{
		help_data->help_internal.draw_style = draw_style;
	}
}

void set_help_item_skin_style(UIHelp *help, U32 skin_style)
{
	UIHelpData *help_data = (UIHelpData *)help->private_data;

	if (help_data)
	{
		help_data->help_internal.skin_style = skin_style;
	}
}

void set_help_item_bmp_text_space(UIHelp *help, U32 bmp_text_space)
{
	UIHelpData *help_data = (UIHelpData *)help->private_data;

	if (help_data)
	{
		help_data->help_internal.bmp_text_space = bmp_text_space;
	}
}

void set_help_item_gap(UIHelp *help, U32 item_gap)
{
	UIHelpData *help_data = (UIHelpData *)help->private_data;

	if (help_data)
	{
		help_data->help_internal.item_gap = item_gap;
	}
}

void set_help_item_border_space(UIHelp *help, U32 border_space)
{
	UIHelpData *help_data = (UIHelpData *)help->private_data;

	if (help_data)
	{
		help_data->help_internal.border_space = border_space;
	}
}

U32 get_help_item_align_style(void)
{
	return help_internal_temp.align_style;
}

U32 get_help_item_draw_style(void)
{
	return help_internal_temp.draw_style;
}

U32 get_help_item_skin_style(void)
{
	return help_internal_temp.skin_style;
}

BOOL set_help_item_visible_by_index_gui(UIHelp *help, int item_index, BOOL visible)
{
	UIHelpData *help_data = (UIHelpData *)help->private_data;

	if (item_index < 0 || NULL == help_data)
	{
		return FALSE;
	}

	if (item_index < 0 || item_index >= help_data->item_number)
	{
		return FALSE;
	}
	help_data->help_item[item_index].visible = visible;

	return TRUE;
}

BOOL set_help_item_visible_by_bmp_gui(UIHelp *help, bmp_icon_type bmp, BOOL visible)
{
	if (!check_bmp_valid(bmp, 1))
	{
		return FALSE;
	}

	return set_help_item_visible_by_index_gui(help, get_help_item_index_by_bmp_gui(help, bmp), visible);
}

BOOL set_help_item_visible_by_string_gui(UIHelp *help, char *string, BOOL visible)
{
	if (NULL == string)
	{
		return FALSE;
	}

	return set_help_item_visible_by_index_gui(help, get_help_item_index_by_string_gui(help, string), visible);
}


int get_total_visible_item_num_gui(UIHelp *help)
{
	UIHelpData *help_data = (UIHelpData *)help->private_data;
	int total_visible_num = 0;
	int loop;

	if (NULL == help_data)
	{
		return 0;
	}
	for (loop = 0; loop < help_data->item_number; ++loop)
	{
		if (help_data->help_item[loop].visible)
		{
			total_visible_num++;
		}
	}

	return total_visible_num;
}

BOOL hide_all_help_items_gui(UIHelp *help)
{
	UIHelpData *help_data = (UIHelpData *)help->private_data;
	int item_index = 0;

	if (NULL == help_data)
	{
		return FALSE;
	}

	for (item_index = 0; item_index < help_data->item_number; item_index++)
	{
		set_help_item_visible_by_index_gui(help, item_index, FALSE);
	}

	return TRUE;
}


BOOL set_help_item_bmp_gui(UIHelp *help, int item_index, bmp_icon_type* bmp)
{
	UIHelpData *help_data = (UIHelpData *)help->private_data;

	if (item_index < 0 || NULL == help_data || NULL == bmp || item_index >= help_data->item_number)
	{
		return FALSE;
	}

	help_data->help_item[item_index].bmp_info[0].bmp = bmp[0];

	return TRUE;
}

BOOL set_help_item_bmp_num_gui(UIHelp *help, int item_index, int bmp_num)
{
	UIHelpData *help_data = (UIHelpData *)help->private_data;

	if (NULL == help_data || item_index < 0 || item_index >= help_data->item_number)
	{
		return FALSE;
	}
	help_data->help_item[item_index].bmp_num = MIN(bmp_num, HELP_ITEM_BMP_MAX_NUM);

	return TRUE;
}

BOOL set_help_item_bmp_ex_gui(UIHelp *help, int item_index, int bmp_index, bmp_icon_type bmp)
{
	UIHelpData *help_data = (UIHelpData *)help->private_data;
	int bmp_num;

	if (NULL == help_data || item_index < 0 || item_index >= help_data->item_number)
	{
		return FALSE;
	}
	bmp_num = help_data->help_item[item_index].bmp_num;
	if (bmp_index < 0 || bmp_index >= bmp_num)
	{
		return FALSE;
	}
	help_data->help_item[item_index].bmp_info[bmp_index].bmp = bmp;

	return TRUE;
}

BOOL set_help_item_bmp_by_string_gui(UIHelp *help, char *string, bmp_icon_type* bmp)
{
	UIHelpData *help_data = (UIHelpData *)help->private_data;
	int item_index = 0;

	if (NULL == help_data || NULL == string || NULL == bmp)
	{
		return FALSE;
	}

	item_index = get_help_item_index_by_string_gui(help, string);

	return set_help_item_bmp_gui(help, item_index, bmp);
}

BOOL set_help_item_string_gui(UIHelp *help, int item_index, char *string)
{
	UIHelpData *help_data = (UIHelpData *)help->private_data;

	if (item_index < 0 || NULL == help_data || item_index >= help_data->item_number || NULL == string)
	{
		return FALSE;
	}

	snprintf(help_data->help_item[item_index].string, HELP_STRING_MAX_LENGTH, "%s", string);

	return TRUE;
}

BOOL set_help_item_string_by_bmp_gui(UIHelp *help, bmp_icon_type bmp, char *string)
{
	UIHelpData *help_data = (UIHelpData *)help->private_data;
	int item_index = 0;

	if (NULL == help_data || NULL == string || !check_bmp_valid(bmp, 1))
	{
		return FALSE;
	}

	item_index = get_help_item_index_by_bmp_gui(help, bmp);

	return set_help_item_string_gui(help, item_index, string);
}

BOOL set_help_item_info_gui(UIHelp *help, int item_index, bmp_icon_type *bmp, char *string)
{
	UIHelpData *help_data = (UIHelpData *)help->private_data;
	BOOL result = TRUE;

	if (item_index < 0 || NULL == help_data || NULL == bmp || NULL == string)
	{
		return FALSE;
	}
	if (item_index >= help_data->item_number)
	{
		if (item_index == help_data->item_number)
		{
			help_data->item_number++;
			help_data->help_item[item_index].bmp_num = 1;
			help_data->help_item[item_index].visible = TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	result = set_help_item_bmp_gui(help, item_index, bmp);
	result &= set_help_item_string_gui(help, item_index, string);

	return result;
}

BOOL set_help_item_max_width_by_index_gui(UIHelp *help, int item_index, int max_width)
{
	UIHelpData *help_data = (UIHelpData *)help->private_data;

	if (item_index < 0 || NULL == help_data)
	{
		return FALSE;
	}
	if (item_index < 0 || item_index >= help_data->item_number)
	{
		return FALSE;
	}
	help_data->help_item[item_index].max_width = max_width;

	return TRUE;
}

BOOL set_help_item_max_width_by_bmp_gui(UIHelp *help, bmp_icon_type bmp, int max_width)
{
	UIHelpData *help_data = (UIHelpData *)help->private_data;

	if (NULL == help_data || !check_bmp_valid(bmp, 1))
	{
		return FALSE;
	}

	return set_help_item_max_width_by_index_gui(help, get_help_item_index_by_bmp_gui(help, bmp), max_width);
}

BOOL set_help_item_icon_type_by_bmp_gui(UIHelp *help, bmp_icon_type bmp, ICON_TYPE type, UIColor color)
{
	UIHelpData *help_data = (UIHelpData *)help->private_data;

	if (NULL == help_data || !check_bmp_valid(bmp, 1))
	{
		return FALSE;
	}

	return set_help_item_icon_type_by_index_gui(help, get_help_item_index_by_bmp_gui(help, bmp), type, color);
}

BOOL set_help_item_string_color_by_bmp_gui(UIHelp *help, bmp_icon_type bmp, UIColor color)
{
	UIHelpData *help_data = (UIHelpData *)help->private_data;

	if (NULL == help_data || !check_bmp_valid(bmp, 1))
	{
		return FALSE;
	}

	return set_help_item_string_color_by_index_gui(help, get_help_item_index_by_bmp_gui(help, bmp), color);
}

void get_help_item_position_info(UIHelp *help)
{
	UIHelpData *help_data = (UIHelpData *)help->private_data;

	if (NULL == help_data)
	{
		return;
	}

	reset_help_item_info(help);
	remove_help_invalid_item_gui(help_data);
	get_help_item_rect(help);
}
#endif
