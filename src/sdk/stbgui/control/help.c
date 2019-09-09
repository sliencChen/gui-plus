/*
 * help.c
 *
 *  Created on: 2018-9-18
 *      Author: Grace
 */

#include <gui_window.h>
#include <control/help.h>


//-------------------------------------------------------------------------------------------------
// internal use
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


//static int draw_help_item_bmp_gui(UIHelp *help, UIHelpItem *help_item, int index)
//{
//	UIPainter *painter = &(help->painter);
//	bmp_icon_type cur_bmp = help_item->bmp_info[index].bmp;
//	UIRect rect = ui_rect(help_item->rect.point.x, help_item->rect.point.y, help_item->rect.size.width, help_item->rect.size.height);
//	DrawBmpParam param;
//	UIBmp bmp_info;
//	int item_width = 0;
//
//	if (!gui_get_bmp(cur_bmp,&bmp_info))
//	{
//		return -1;
//	}
//	if (help_item->bmp_info)
//	{
//		param.bmp_info = bmp_info;
//		param.mode = help_item->bmp_info[index].mode;
//		param.mode_param = help_item->bmp_info[index].mode_param;
//
//		draw_bitmap(painter, rect, &param);
//		invalidate_window(help);
//	}
//
//	return get_bmp_icon_width(cur_bmp);
//}
//
//static int draw_help_circle_bmp_gui(UIHelp *help, UIHelpItem *help_item, int index)
//{
//	UIPainter *painter = &(help->painter);
//	UIRect rect;
//	bmp_icon_type cur_bmp = help_item->bmp_info[index].bmp;
//	UIColor color_temp = help_item->bmp_info[index].icon_color;
//	BOOL is_color_circle = FALSE;
//	UIHelpData *help_data = help->private_data;
//	short radius = help_data->help_internal.radius;
//	bmp_icon_type bmp_array[] = {BMP_BLUE_CIRCLE, BMP_RED_CIRCLE, BMP_GREEN_CIRCLE, BMP_YELLOW_CIRCLE};
//	U32 color_array[] = {BLUE_COLOR, RED_COLOR, GREEN_COLOR, YELLOW_COLOR};
//	int i = 0;
//
//	if (use_rect_to_replace_circle_for_help_gui())
//	{
//		painter->mode = COVER_BACKGROUND;
//		rect = ui_rect(help_item->rect.point.x, help_item->rect.point.y, help_data->help_internal.radius, help_data->help_internal.radius);
//		draw_rect(painter, rect, color_temp);
//		radius = radius / 2;
//	}
//	else
//	{
//		rect = ui_rect(help_item->rect.point.x, help_item->rect.point.y, 2 * help_data->help_internal.radius, 2 * help_data->help_internal.radius);
//		draw_round_rect(painter, rect, radius, color_temp, TRUE, 0);
//	}
//	invalidate_window(help);
//
//	return radius * 2;
//}
//
//static int draw_help_text_bmp_gui(UIHelp *help, UIHelpItem *help_item, int index)
//{
//	char str_temp[100] = "";
//	UIColor color = help_item->string_color;
//	UIPainter *painter = &(help->painter);
//	DrawTextParam param;
//	UIRect text_rect = ui_rect(help_item->rect.point.x, help_item->rect.point.y, help->rect.size.width, get_text_height(18, 100));
//	bmp_icon_type cur_bmp = help_item->bmp_info[index].bmp;
//
//	if (conver_bmp_to_text_gui(str_temp, sizeof(str_temp), cur_bmp) == 0)
//	{
//		return 0;
//	}
//
//	if (check_bmp_name(BMP_REC_HELP, cur_bmp))
//	{
//		color = RED_COLOR;
//	}
//
//	default_draw_text_param(&param);
//	param.color = color;
//
//	draw_text(painter, text_rect, str_temp, -1, &param);
//	invalidate_window(help);
//
//	return get_text_width(str_temp, -1, -1, 100);
//}
//
//static int draw_help_extra_bmp_gui(UIHelp *help, UIHelpItem *help_item, int index)
//{
//	int item_width = 0;
//	UIColor color = help_item->bmp_info[index].icon_color;
//	bmp_icon_type cur_bmp = help_item->bmp_info[index].bmp;
//	UIHelpData *help_data = help->private_data;
//	UIPainter *painter = &(help->painter);
//	int triangle_length = 6;
//	UIRect rect = ui_rect(help_item->rect.point.x, help_item->rect.point.y, help->rect.size.width, help->rect.size.height);
//
//	if (check_bmp_name(cur_bmp, BMP_MV))
//	{
//		draw_triangle(painter, rect.point.x + triangle_length, rect.point.y, 1, triangle_length, color);
//		draw_triangle(painter, rect.point.x + triangle_length * 3, rect.point.y, 0, triangle_length, color);
//		draw_triangle(painter, rect.point.x + triangle_length + triangle_length, rect.point.y - triangle_length, 2, triangle_length, color);
//		draw_triangle(painter, rect.point.x + triangle_length + triangle_length, rect.point.y + triangle_length, 3, triangle_length, color);
//		item_width = 3 * triangle_length;
//
//		invalidate_window(help);
//	}
//
//	return item_width;
//}
//
//static int draw_help_bmp_common_gui(UIHelp *help, UIHelpItem *help_item, int index)
//{
//	int item_width = 0;
//
//	if (!help || !help_item)
//	{
//		return 0;
//	}
//
//	switch (help_item->bmp_info[index].icon_type)
//	{
//		case ICON_TYPE_EXTRA:
//			item_width = draw_help_extra_bmp_gui(help, help_item, index);
//			break;
//		case ICON_TYPE_COLOR:
//			item_width = draw_help_circle_bmp_gui(help, help_item, index);
//			break;
//		case ICON_TYPE_TEXT:
//			item_width = draw_help_text_bmp_gui(help, help_item, index);
//			break;
//		default:
//			item_width = draw_help_item_bmp_gui(help, help_item, index);
//			break;
//	}
//
//	return item_width;
//}
//
//static int get_help_item_bmp_height(UIHelpData *help_data, UIHelpItem help_item, int index)
//{
//	int item_height = 0;
//
//	if (!help_data)
//	{
//		return 0;
//	}
//
//	switch (help_item.bmp_info[index].icon_type)
//	{
//		case ICON_TYPE_EXTRA:
//			item_height = 0;
//			break;
//		case ICON_TYPE_COLOR:
//			if (use_rect_to_replace_circle_for_help_gui())
//			{
//				item_height = help_data->help_internal.radius;
//			}
//			else
//			{
//				item_height = help_data->help_internal.radius * 2;
//			}
//			break;
//		case ICON_TYPE_TEXT:
//			item_height = 28;
//			break;
//		default:
//			item_height = get_bmp_icon_height(help_item.bmp_info[index].bmp);
//			break;
//	}
//
//	return item_height;
//}
//
//static int draw_help_item_bmp(UIHelp *help, UIHelpItem help_item)
//{
//	int loop = 0;
//	int item_width = 0;
//	int max_text_width = 0;
//	UIHelpData *help_data = help->private_data;
//	UIRect rect = ui_rect(help_item.rect.point.x, help_item.rect.point.y, help_item.rect.size.width, help_item.rect.size.height);
//
//	if (NULL == help_data || help_item.bmp_num <= 0)
//	{
//		return item_width;
//	}
//
//	for (loop = 0; loop < help_item.bmp_num && help_item.bmp_info != NULL; loop++)
//	{
//		if (loop > 0)
//		{
//			item_width += help_data->help_internal.bmp_gap;
//		}
//		help_item.rect.point.x += help_data->help_internal.bmp_gap;
//		help_item.rect.point.y += (help_item.rect.size.height - get_help_item_bmp_height(help_data, help_item, loop)) / 2;
//		item_width += draw_help_bmp_common_gui(help, &help_item, loop);
//	}
//
//	return item_width;
//}
//
//static BOOL draw_help_item_string_gui(UIHelp *help, UIHelpItem *help_item)
//{
//	UIPainter *painter = &(help->painter);
//	DrawTextParam param;
//	UIRect rect = ui_rect(help_item->rect.point.x, help_item->rect.point.y, help_item->rect.size.width, help_item->rect.size.height);
//
//	default_draw_text_param(&param);
//	param.color = help_item->string_color;
//
//	draw_text(painter, rect, help_item->string, -1, &param);
//	invalidate_window(help);
//
//	return TRUE;
//}
//
//static void draw_help_item_string(UIHelp *help, UIHelpItem help_item, U32 bmp_width)
//{
//	UIHelpData *help_data = help->private_data;
//	UIRect rect = ui_rect(help_item.rect.point.x, help_item.rect.point.y, help_item.rect.size.width, help_item.rect.size.height);
//	int max_text_width = 0;
//
//	if (NULL == help_data || help_item.string <= 0)
//	{
//		return;
//	}
//
//	if (help_item.string != NULL)
//	{
//		max_text_width = help_item.max_width - bmp_width - help_data->help_internal.bmp_text_space;
//		if (help_item.max_width <= 0)	/* no limit */
//		{
//			max_text_width = help_item.string_width;
//		}
//		if (help_data->help_internal.bmp_text_space > 0)
//		{
//			help_item.rect.point.x += bmp_width + help_data->help_internal.bmp_text_space;
//		}
//		else
//		{
//			help_item.rect.point.x += (help_item.rect.size.width - help_item.string_width) / 2;
//		}
//		help_item.rect.point.y += (help_item.rect.size.height - get_text_height(18, 100)) / 2;
//		draw_help_item_string_gui(help, &help_item);
//	}
//}
//
//static BOOL draw_default_help_item_fun(UIHelp *help)
//{
//	UIHelpData *help_data = (UIHelpData *)help->private_data;
//	BOOL ret = FALSE;
//	int index = 0;
//	int item_width = 0;
//
//	if (NULL == help_data)
//	{
//		return ret;
//	}
//
//	for (index = 0; index < help_data->item_number; index++)
//	{
//		item_width = draw_help_item_bmp(help, help_data->help_item[index]);
//		draw_help_item_string(help, help_data->help_item[index], item_width);
//	}
//
//	return ret;
//}
//
//static int get_help_item_bmp_width(UIHelpData *help_data, int item_index, int bmp_index)
//{
//	int width = 0;
//	ICON_TYPE icon_type;
//	bmp_icon_type cur_bmp;
//	DrawBmpMode mode;
//
//	if (help_data == NULL)
//	{
//		return 0;
//	}
//
//	icon_type = help_data->help_item[item_index].bmp_info[bmp_index].icon_type;
//	cur_bmp = help_data->help_item[item_index].bmp_info[bmp_index].bmp;
//	mode = help_data->help_item[item_index].bmp_info[bmp_index].mode;
//	switch (icon_type)
//	{
//		case ICON_TYPE_TEXT:
//			width = conver_bmp_to_text_gui(NULL, 0, cur_bmp);
//			break;
//		case ICON_TYPE_COLOR:
//			if (use_rect_to_replace_circle_for_help_gui())
//			{
//				width = help_data->help_internal.radius;
//			}
//			else
//			{
//				width = help_data->help_internal.radius * 2;
//			}
//			break;
//		default:
//			width = get_bmp_icon_width(cur_bmp);
//			break;
//	}
//
//	return width;
//}
//
//static int get_help_items_max_line_width(UIHelpData *help_data, char start_index, char count)
//{
//	int sum_width = 0;
//	int loop;
//	int item_width;
//
//	if ((NULL == help_data) || (count <= 0) || (start_index < 0) || (start_index > (help_data->item_number - 1)))
//	{
//		return 0;
//	}
//
//	for (loop = 0; loop < count; loop++)
//	{
//		item_width = 0;
//		item_width = help_data->help_item[start_index + loop].item_width;
//		sum_width += item_width;
//	}
//	sum_width += help_data->help_internal.border_space * 2 + (help_data->help_internal.item_gap * (count - 1));
//
//	return sum_width;
//}
//
//static void get_help_item_col_max_width(UIHelpData *help_data, char *col_num, int max_col, short *col_width, int total_row)
//{
//	int loop, jloop, kloop;
//	int temp, index;
//	short max_col_width;
//
//	if (help_data == NULL)
//	{
//		return;
//	}
//
//	for (loop = 0; loop < max_col; loop++)
//	{
//		temp = 0;
//		max_col_width = 0;
//		for (jloop = 0; jloop < total_row; jloop++)
//		{
//			if (loop < col_num[jloop])
//			{
//				index = 0;
//				for (kloop = 0; kloop < jloop; kloop++)
//				{
//					index += col_num[kloop];
//				}
//				index += loop;	/* current item index */
//				temp = help_data->help_item[index].item_width;
//				if (max_col_width < temp)
//				{
//					max_col_width = temp;
//				}
//			}
//		}
//		col_width[loop] = max_col_width;
//	}
//}
//
//static void init_ui_help_param_gui(UIHelp *help)
//{
//	int  loop;
//	UIHelpData *help_data = (UIHelpData *)help->private_data;
//
//	memset(help_data, 0, sizeof(UIHelpData));
//	for (loop = 0; loop < MAX_HELP_ITEM; loop++)
//	{
//		help_data->help_item[loop].bmp_num = 1;
//		help_data->help_item[loop].max_width = -1;	/* no limit */
//	}
//}
//
//static int get_help_item_index_by_bmp_gui(UIHelp *help, bmp_icon_type bmp)
//{
//	int item_index, icon_index;
//	UIHelpData *help_data = (UIHelpData *)help->private_data;
//
//	if (NULL == help_data || !check_bmp_valid(bmp, 1))
//	{
//		return -1;
//	}
//	for (item_index = 0; item_index < help_data->item_number; item_index++)
//	{
//		for (icon_index = 0; icon_index < help_data->help_item[item_index].bmp_num; icon_index++)
//		{
//			if (check_bmp_name(help_data->help_item[item_index].bmp_info[icon_index].bmp, bmp))
//			{
//				return item_index;
//			}
//		}
//	}
//	return -1;
//}
//
//static int get_help_item_index_by_string_gui(UIHelp *help, char *string)
//{
//	UIHelpData *help_data = (UIHelpData *)help->private_data;
//	int item_index;
//
//	if (NULL == help_data || NULL == string)
//	{
//		return -1;
//	}
//	for (item_index = 0; item_index < help_data->item_number; item_index++)
//	{
//		if (strcmp(help_data->help_item[item_index].string, string) == 0)
//		{
//			return item_index;
//		}
//	}
//	return -1;
//}
//
//static BOOL set_help_item_icon_type_by_index_gui(UIHelp *help, int item_index, ICON_TYPE type, UIColor color)
//{
//	UIHelpData *help_data = (UIHelpData *)help->private_data;
//
//	if (item_index < 0 || NULL == help_data)
//	{
//		return FALSE;
//	}
//	if (item_index < 0 || item_index >= help_data->item_number)
//	{
//		return FALSE;
//	}
//	help_data->help_item[item_index].bmp_info[0].icon_type = type;
//	help_data->help_item[item_index].bmp_info[0].icon_color = color;
//
//	return TRUE;
//}
//
//static BOOL set_help_item_string_color_by_index_gui(UIHelp *help, int item_index, UIColor color)
//{
//	UIHelpData *help_data = (UIHelpData *)help->private_data;
//
//	if (item_index < 0 || NULL == help_data)
//	{
//		return FALSE;
//	}
//	if (item_index < 0 || item_index >= help_data->item_number)
//	{
//		return FALSE;
//	}
//	help_data->help_item[item_index].string_color = color;
//
//	return TRUE;
//}
//
//static void get_help_item_rect_point(UIHelpData *help_data, char *col_num, int max_col_num, short *col_max_width, short *row_width, int total_row, UIRect area_rect)
//{
//	int loop = 0, jloop = 0, index = 0;
//	int max_width = 0;
//	int item_space = 0;
//	int temp_left = 0, temp_top = 0;
//	int row_height = area_rect.size.height / total_row;
//	BOOL align_to_grid = help_data->help_internal.align_to_grid;;
//	int item_gap = help_data->help_internal.item_gap * max_col_num;
//
//	for (loop = 0; loop < max_col_num; loop++)
//	{
//		max_width += col_max_width[loop];
//	}
//
//	if (max_width > (area_rect.size.width - help_data->help_internal.border_space * 2))
//	{
//		align_to_grid = FALSE;
//	}
//
//	if (align_to_grid && FULL_JUST == help_data->help_internal.align_style)
//	{
//		help_data->help_internal.align_style = LEFT_JUST;
//	}
//
//	if (max_col_num > 0)
//	{
//		item_space = (area_rect.size.width - max_width - help_data->help_internal.border_space * 2 - item_gap) / max_col_num;
//	}
//	index = 0;
//	for (loop = 0; loop < total_row; loop++)
//	{
//		if (!align_to_grid)	//if not align to grid,calculate ItemSpace every time.
//		{
//			item_space = 0;
//			if (FULL_JUST == help_data->help_internal.align_style && col_num[loop] > 1)
//			{
//				item_space = (area_rect.size.width - row_width[loop]) / (col_num[loop] - 1);
//			}
//			else if (col_num[loop] > 0)
//			{
//				item_space = (area_rect.size.width - row_width[loop]) / col_num[loop];
//			}
//		}
//
//		if (LEFT_JUST == help_data->help_internal.align_style || FULL_JUST == help_data->help_internal.align_style)
//		{
//			temp_left = 0;
//		}
//		else if (RIGHT_JUST == help_data->help_internal.align_style)
//		{
//			temp_left = item_space;
//		}
//		else
//		{
//			temp_left = item_space / 2;
//		}
//
//		temp_top = loop * row_height;
//
//		for (jloop = 0; jloop < col_num[loop]; jloop++)
//		{
//			help_data->help_item[index + jloop].rect.point.x = temp_left + help_data->help_internal.border_space;
//			help_data->help_item[index + jloop].rect.size.height = row_height;
//			help_data->help_item[index + jloop].rect.point.y = temp_top;
//			if (help_data->help_internal.bmp_text_space > 0)
//			{
//				if (align_to_grid)
//				{
//					help_data->help_item[index + jloop].rect.size.width = col_max_width[jloop];
//				}
//				else
//				{
//					help_data->help_item[index + jloop].rect.size.width = help_data->help_item[index + jloop].item_width;
//				}
//				temp_left += help_data->help_item[index + jloop].rect.size.width + item_space;
//			}
//			else
//			{
//				help_data->help_item[index + jloop].rect.size.width = col_max_width[jloop] + (area_rect.size.width - max_width - help_data->help_internal.border_space * 2 - item_gap) / max_col_num;
//				temp_left += help_data->help_item[index + jloop].rect.size.width + help_data->help_internal.item_gap;
//			}
//		}
//		index += col_num[loop];
//	}
//}
//
//static int get_help_item_col_row_info(UIHelpData *help_data, char *col_num, int *max_col_num, short *row_width, int area_width)
//{
//	int total_row = 1;
//	int max_line_width = 0;
//	int max_col_num_temp = 0;
//	int loop = 0, temp = 0;
//	int total_item_number = 0;
//
//	if (help_data == NULL)
//	{
//		return 0;
//	}
//
//	total_item_number = help_data->item_number;
//	max_line_width = get_help_items_max_line_width(help_data, 0, help_data->item_number);
//	row_width[0] = max_line_width;
//	max_col_num_temp = total_item_number;
//
//	while(max_line_width > area_width)
//	{
//		total_row++;
//		if (total_item_number % total_row == 0)
//		{
//			max_col_num_temp = total_item_number / total_row;
//		}
//		else
//		{
//			max_col_num_temp = total_item_number / total_row + 1;
//		}
//		max_line_width = 0;
//		for (loop = 0; loop < total_row; loop++)
//		{
//			if (loop * max_col_num_temp + max_col_num_temp <= total_item_number)
//			{
//				temp = get_help_items_max_line_width(help_data, loop * max_col_num_temp, max_col_num_temp);
//			}
//			else
//			{
//				temp = get_help_items_max_line_width(help_data, loop * max_col_num_temp, total_item_number - loop * max_col_num_temp);
//			}
//			row_width[loop] = temp;
//			if (max_line_width < temp)
//			{
//				max_line_width = temp;
//			}
//		}
//	}
//
//	for (loop = 0; loop < total_row; loop++)
//	{
//		if ((loop + 1) * max_col_num_temp < total_item_number)
//		{
//			col_num[loop] = max_col_num_temp;
//		}
//		else
//		{
//			col_num[loop] = total_item_number - loop * max_col_num_temp;
//		}
//	}
//
//	*max_col_num = max_col_num_temp;
//
//	return total_row;
//}
//
//static void get_help_item_bmp_string_width(UIHelp *help)
//{
//	UIHelpData *help_data = (UIHelpData *)help->private_data;
//	int index = 0, index1;
//	bmp_icon_type cur_bmp;
//	ICON_TYPE cur_icon_type;
//	int bmp_width = 0;
//
//	if (help_data == NULL)
//	{
//		return;
//	}
//
//	for (index = 0; index < help_data->item_number; index++)
//	{
//		for (index1 = 0; index1 < help_data->help_item[index].bmp_num; index1++)
//		{
//			if (help_data->help_item[index].bmp_info == NULL)
//			{
//				break;
//			}
//			cur_bmp = help_data->help_item[index].bmp_info[index1].bmp;
//			cur_icon_type = help_data->help_item[index].bmp_info[index1].icon_type;
//			if ((check_bmp_valid(cur_bmp, 1)))
//			{
//				bmp_width = get_help_item_bmp_width(help_data, index, index1);
//			}
//			if (index1 > 0)
//			{
//				bmp_width += help_data->help_internal.bmp_gap;
//			}
//
//		}
//
//		if (help_data->help_item[index].string != NULL)
//		{
//			help_data->help_item[index].string_width = get_text_width(help_data->help_item[index].string, -1, -1, -1);
//		}
//		if (help_data->help_internal.bmp_text_space > 0)
//		{
//			help_data->help_item[index].item_width = bmp_width + help_data->help_internal.bmp_text_space + help_data->help_item[index].string_width;// + help_data->help_internal.item_gap;
//		}
//		else
//		{
//			help_data->help_item[index].item_width = MAX(bmp_width, help_data->help_item[index].string_width);// + help_data->help_internal.item_gap;
//		}
//		if (help_data->help_item[index].max_width > 0)
//		{
//			help_data->help_item[index].item_width = MIN(help_data->help_item[index].item_width, help_data->help_item[index].max_width);
//		}
//	}
//}
//
//static void get_help_item_rect(UIHelp *help)
//{
//	UIHelpData *help_data = (UIHelpData *)help->private_data;
//	int loop = 0, jloop = 0, index = 0;
//	int max_line_width = 0;
//	int temp = 0;
//	int max_col_num = 0;
//	short col_max_width[MAX_HELP_ITEM] = {0};
//	short row_width[MAX_HELP_ITEM] = {0};
//	char col_num[MAX_HELP_ITEM];
//	int total_row = 0;
//
//	if (help_data == NULL)
//	{
//		return;
//	}
//
//	get_help_item_bmp_string_width(help);
//	total_row = get_help_item_col_row_info(help_data, col_num, &max_col_num, row_width, help->rect.size.width);
//	get_help_item_col_max_width(help_data, col_num, max_col_num, col_max_width, total_row);
//	get_help_item_rect_point(help_data, col_num, max_col_num, col_max_width, row_width, total_row, help->rect);
//}
//
//static void reset_help_item_info(UIHelp *help)
//{
//	UIHelpData *help_data = (UIHelpData *)help->private_data;
//	int loop = 0, loop1 = 0;
//	bmp_icon_type cur_bmp;
//	ICON_TYPE icon_type;
//
//	memcpy(&help_internal_temp, &help_data->help_internal, sizeof(UIHelpInter));
//	for (loop = 0; loop < help_data->item_number; loop++)
//	{
//		for (loop1 = 0; loop1 < help_data->help_item[loop].bmp_num; loop1++)
//		{
//			cur_bmp = help_data->help_item[loop].bmp_info[loop1].bmp;
//			cur_bmp = conver_extra_help_to_bmp_gui(cur_bmp);
//			if (cur_bmp != help_data->help_item[loop].bmp_info[loop1].bmp)
//			{
//				help_data->help_item[loop].bmp_info[loop1].bmp = cur_bmp;
//				if (help_data->help_internal.bmp_text_space <= 0)
//				{
//					help_data->help_item[loop].bmp_info[loop1].mode = DRAW_BMP_MODE_VAR_SIZE;
//					help_data->help_item[loop].bmp_info[loop1].mode_param.copy_pt = ui_point(get_bmp_icon_width(cur_bmp) / 2, 0xFFFF);
//				}
//			}
//			icon_type = get_help_icon_type_gui(ICON_TYPE_BMP, cur_bmp);
//			if (help_data->help_item[loop].bmp_info[loop1].icon_type == 0 && icon_type != 0)
//			{
//				help_data->help_item[loop].bmp_info[loop1].icon_type = icon_type;
//			}
//		}
//	}
//}


//-------------------------------------------------------------------------------------------------
// window manager callback
//-------------------------------------------------------------------------------------------------
static BOOL help_item_on_draw(UIHelp *self)
{
	UIHelpThemeNormal default_help;

	default_help.base.icon_text_space = 15;
	default_help.base.icon_space = 5;
	default_help.base.item_space = 20;
	default_help.base.text_color = WHITE_COLOR;
	default_help.conv_func = NULL;

	return help_theme_normal(self, &default_help);
}

static BOOL help_item_on_destroy(UIHelp *self)
{
	delete_help_private_data(self->private_data);

	return TRUE;
}


//-------------------------------------------------------------------------------------------------
// interface
//-------------------------------------------------------------------------------------------------
UIHelp* create_help(const char *id)
{
	UIWindow *help;

	help = create_control(id, 0, 0, 500, 100);
	if (help != NULL)
	{
		UIHelpData *help_data = new_help_private_data();

		if (help_data == NULL)
		{
			destroy_window(help);
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


//-------------------------------------------------------------------------------------------------
// convert icon id
//-------------------------------------------------------------------------------------------------
static UISize get_text_help_icon_size(struct _ui_help_item_icon_ *self)
{
	UISize size = {0, 0};

	if (self)
	{
		size.width = get_text_width(self->data.text, -1, -1, -1);
		size.height = get_text_height(-1, -1);
	}

	return size;
}

void draw_text_help_icon(struct _ui_help_item_icon_ *self, UIPainter *painter, UIRect rect)
{
	gui_draw_text(painter, rect, self->data.text, self->color);
}

void text_help_icon(UIHelpItemIcon *icon, const char *text)
{
	icon->data.text = text;
	icon->get_size = get_text_help_icon_size;
	icon->draw = draw_text_help_icon;
}

void bitmap_help_icon(UIHelpItemIcon *icon, const char *bmp_name)
{

}

void circle_help_icon(UIHelpItemIcon *icon, int radius)
{

}

UIHelpItemIcon default_convert_function(int icon_id)
{
	UIHelpItemIcon icon;
	int i;
	int ids[] =
	{
		GUI_EVT_VK_UP,		GUI_EVT_VK_DOWN,		GUI_EVT_VK_LEFT,	GUI_EVT_VK_RIGHT,
		GUI_EVT_VK_SELECT,	GUI_EVT_VK_MENU,		GUI_EVT_VK_EXIT,	GUI_EVT_VK_RED,
		GUI_EVT_VK_GREEN,	GUI_EVT_VK_YELLOW,		GUI_EVT_VK_BLUE,
	};
	const char *names[] =
	{
		"[UP]",			"[DOWN]",		"[LEFT]",		"[RIGHT]",
		"[SELECT]",		"[MENU]",		"[EXIT]",		"[RED]",
		"[GREEN]",		"[YELLO]",		"[BLUE]",
	};

	gui_memset(&icon, 0, sizeof(UIHelpItemIcon));
	for (i = 0; i < sizeof(ids) / sizeof(ids[0]); i++)
	{
		if (ids[i] == icon_id)
		{
			text_help_icon(&icon, names[i]);
			break;
		}
	}

	return icon;
}


//-------------------------------------------------------------------------------------------------
// typical theme
//-------------------------------------------------------------------------------------------------
BOOL help_theme_normal(UIHelp *self, UIHelpThemeNormal *theme_param)
{
	return TRUE;
}



//void init_help_params_gui(UIHelp *help, bmp_icon_type *bmp, string_table *string, char num)
//{
//	int loop;
//	char mini_item_num;
//	UIHelpData *help_data = (UIHelpData *)help->private_data;
//
//	init_ui_help_param_gui(help);
//
//	mini_item_num = MIN(num, MAX_HELP_ITEM);
//	help_data->item_number = mini_item_num;
//	for (loop = 0; loop < mini_item_num; loop++)
//	{
//		help_data->help_item[loop].bmp_info[0].bmp = bmp[loop];
//		help_data->help_item[loop].bmp_num = 1;
//		help_data->help_item[loop].visible = TRUE;
//		help_data->help_item[loop].bmp_info[0].icon_type = ICON_TYPE_BMP;
//		help_data->help_item[loop].string_color = get_help_item_string_color_by_bmp_gui(help_data->help_item[loop].bmp_info[0].bmp);;
//		help_data->help_item[loop].bmp_info[0].icon_color = 0xFFFF0000;//Need input by designer tool
//		if (string[loop] < MAX_HELP_ITEM)//not init pStrings
//		{
//			snprintf(help_data->help_item[loop].string, HELP_STRING_MAX_LENGTH, "%s", "Test");//need get string info from string data
//		}
//		else
//		{
//			snprintf(help_data->help_item[loop].string, HELP_STRING_MAX_LENGTH, "%s", "Unknown");
//		}
//	}
//
//	help_set_default_help_info(help_data);
//	sort_help_color_item_gui(help_data);
//}
