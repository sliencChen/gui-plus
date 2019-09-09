/*
 * line_chart.c
 *
 *  Created on: 2018-9-12
 *      Author: Shelly
 */

#include <gui_window.h>
#include <control/line_chart.h>


#if 1
//-------------------------------------------------------------------------------------------------
// local variable
//-------------------------------------------------------------------------------------------------
static int draw_line_chart_text(UILineChart *self)
{
	UILineChartData *line_chart_data;
    UIPainter *painter = &(self->painter);
    char buf[50];
    int index = 0, total_index = 0;
	DrawTextParam param;

	if (!self || !self->private_data)
	{
		return -1;
	}

	line_chart_data = (UILineChartData *)self->private_data;
    UILineChartSubData *line_sub_data = line_chart_data->line_data;

    for(total_index = 0; total_index < line_chart_data->total_num; total_index ++)
    {
        for (index = 0; index < line_sub_data->point_num; index++)
        {
            sprintf(buf, "%d", line_sub_data->point_value[index]);
            UIRect rect = ui_rect(line_sub_data->point_data[index].point_array.x, line_sub_data->point_data[index].point_array.y + 20,
                    get_text_width(buf, -1, 18, 100), get_text_height(18,100));
			if (line_sub_data->point_data[index].point_array.y + 20 + get_text_height(18,100) > (self->rect.size.height - self->rect.point.y))
			{
				rect = ui_rect(line_sub_data->point_data[index].point_array.x, line_sub_data->point_data[index].point_array.y - 20 - get_text_height(18,100),
                    get_text_width(buf, -1, 18, 100), get_text_height(18,100));
			}
            default_draw_text_param(&param);
            param.color = line_sub_data->string_color;
            draw_text(painter, rect, buf, -1, &param);
        }
        line_sub_data = line_sub_data->next;
    }

	invalidate_window(self);

	return 0;
}

static int draw_line_chart_point_bmp(UILineChart *self)
{
    UILineChartData *line_chart_data;
    UIPainter *painter = &(self->painter);
    char buf[50];
    int index = 0, total_index = 0;
	DrawBmpParam param;
	UIBmp bmp_info;

	if (!self || !self->private_data)
	{
		return -1;
	}

	line_chart_data = (UILineChartData *)self->private_data;
    UILineChartSubData *line_sub_data = line_chart_data->line_data;
    for(total_index = 0; total_index < line_chart_data->total_num; total_index ++)
    {
        for (index = 0; index < line_sub_data->point_num; index++)
        {
            bmp_icon_type cur_bmp = line_sub_data->point_data[index].bmp_info.bmp;
            UIRect rect = ui_rect(line_sub_data->point_data[index].point_array.x, line_sub_data->point_data[index].point_array.y + 20,
                    get_bmp_icon_width((cur_bmp)), get_bmp_icon_height(cur_bmp));
			if (line_sub_data->point_data[index].point_array.y + 20 + get_bmp_icon_height(cur_bmp) > (self->rect.size.height - self->rect.point.y))
			{
				rect = ui_rect(line_sub_data->point_data[index].point_array.x, line_sub_data->point_data[index].point_array.y - 20 - get_bmp_icon_height(cur_bmp),
                    get_bmp_icon_width((cur_bmp)), get_bmp_icon_height(cur_bmp));
			}
            if (!gui_get_bmp(cur_bmp, &bmp_info))
            {
                continue;
            }
            param.bmp_info = bmp_info;
            param.mode =line_sub_data->point_data[index].bmp_info.mode;
            draw_bitmap(painter, rect, &param);
        }
        line_sub_data = line_sub_data->next;
    }

	invalidate_window(self);

	return 0;
}

static int set_line_chart_text_internal(UILineChartData *line_chart_data)
{
	int point_index = 0;
	int total_index = 0;
	int point_num = line_chart_data->line_data->point_num;

    for (point_index = 0; point_index < point_num; point_index++)
    {
        line_chart_data->line_data->point_data[point_index].point_array.x = 10 + 50 * point_index;
        line_chart_data->line_data->point_data[point_index].point_array.y = 10 + 20 * point_index;
        line_chart_data->line_data->point_value[point_index] = 0;
        line_chart_data->line_data->point_data[point_index].bmp_info.bmp = "";
        line_chart_data->line_data->point_data[point_index].bmp_info.mode = DRAW_BMP_MODE_NORMAL;
    }

	return 0;
}

static UILineChartData* new_line_chart_private_data(void)
{
	UILineChartData *line_chart_data;
	UILineChartSubData *line_sub_data;
	int total_num = 0;

	line_chart_data = (UILineChartData *)gui_malloc(sizeof(UILineChartData));
	line_sub_data = (UILineChartSubData *)gui_malloc(sizeof(UILineChartSubData));

	if (line_chart_data == NULL)
	{
		return NULL;
	}
	/* default parameter */
	gui_memset(line_chart_data, 0, sizeof(UILineChartData));
	gui_memset(line_sub_data, 0, sizeof(UILineChartSubData));
    line_chart_data->line_data = line_sub_data;
	//line data
	line_chart_data->total_num = 1;
    line_sub_data->point_num = 5;
    line_sub_data->line_color = WHITE_COLOR;
    line_sub_data->point_color = WHITE_COLOR;
	line_sub_data->string_color = RED_COLOR;
    line_sub_data->line_width = 3;
    line_sub_data->radius = 4;
    line_sub_data->draw_default_bmp_info_fun = draw_line_chart_point_bmp;
    line_sub_data->draw_line_chart_text_fun = draw_line_chart_text;
    line_sub_data->next = NULL;
    set_line_chart_text_internal(line_chart_data);
	//point data

	return line_chart_data;
}

static void set_default_line_son_data(UILineChartSubData *line_sub_data)
{
    int index = 0;
    line_sub_data->point_num = 5;

	for (index = 0; index < line_sub_data->point_num; index++)
    {
        line_sub_data->point_data[index].point_array.x = 10 + 50 * index;
        line_sub_data->point_data[index].point_array.y = 10 + 20 * index;
        line_sub_data->point_value[index] = 0;
        line_sub_data->point_data[index].bmp_info.bmp = "";
        line_sub_data->point_data[index].bmp_info.mode = DRAW_BMP_MODE_NORMAL;
    }
    line_sub_data->line_color = WHITE_COLOR;
    line_sub_data->point_color = WHITE_COLOR;
	line_sub_data->string_color = RED_COLOR;
    line_sub_data->line_width = 3;
    line_sub_data->radius = 4;
    line_sub_data->draw_default_bmp_info_fun = draw_line_chart_point_bmp;
    line_sub_data->draw_line_chart_text_fun = draw_line_chart_text;
    line_sub_data->next = NULL;

}

static void delete_line_chart_private_data(UILineChartData *line_chart_data)
{
	UILineChartSubData *line_sub_data, *new_line_sub_data;
	line_sub_data = new_line_sub_data = line_chart_data->line_data;

    if (!line_sub_data)
    {
        return;
    }
	if (line_chart_data)
	{
	    if (line_sub_data->next != NULL)
        {
            while (line_sub_data->next != NULL)
            {
                new_line_sub_data = line_sub_data->next;
                gui_free(line_sub_data);
                line_sub_data = new_line_sub_data;
            }
        }
        else
        {
            gui_free(line_sub_data);
        }
		gui_free(line_chart_data);
	}
}

static void get_coordinate_boundry(int array[], int *low_boundry, int *hign_boundry, int total) // copy from ali_code
{
    int index = 0;
	int tmp = 0;

	int l_bound = 0, h_bound = 0;

	if (total <= 0)
	{
		return;
	}

	l_bound = *low_boundry;
	h_bound = *hign_boundry;
	for (index = 0; index < total; ++index)
	{
		l_bound = MIN(l_bound, array[index]);
		h_bound = MAX(h_bound, array[index]);
	}
	tmp = (h_bound - l_bound) * 15 / 100;
	*hign_boundry = h_bound + tmp;
	*low_boundry = l_bound - tmp;
}

static void get_point_coordinate(UIRect rect, int array[], int total, int low_boundry, int hign_boundry, UILineChartSubData *line_sub_data) // copy from ali_code
{
    int index = 0;
	int temp_y;

	for (index = 0; index < total; index++)
	{
		int y_offset;
		if (array[index] == low_boundry)
			y_offset = 10;
		else if (array[index] == hign_boundry)
			y_offset = -10;

		if (low_boundry == hign_boundry)
			temp_y = 10;
		else
			temp_y = rect.size.height * (array[index] - low_boundry) / (hign_boundry - low_boundry) + y_offset;
		line_sub_data->point_data[index].point_array.x = rect.point.x + (rect.size.width / total) * index + (rect.size.width / total) / 2;
		line_sub_data->point_data[index].point_array.y = rect.point.y + rect.size.height - temp_y;
	}
}


//-------------------------------------------------------------------------------------------------
// window manager callback
//-------------------------------------------------------------------------------------------------
static BOOL line_chart_on_draw(UILineChart *self)
{
	UIPainter *painter = &(self->painter);
	int index = 0;
	int point_index = 0;
	UILineChartData *line_chart_data = self->private_data;
	UILineChartSubData *line_sub_data = line_chart_data->line_data;

    if (!line_sub_data)
    {
        return -1;
    }

	for (index = 0; index < line_chart_data->total_num; index++)
    {
        for (point_index = 0; point_index < line_sub_data->point_num - 1; point_index++)
        {
            draw_line(painter, ui_point(line_sub_data->point_data[point_index].point_array.x, line_sub_data->point_data[point_index].point_array.y), ui_point(line_sub_data->point_data[point_index + 1].point_array.x,
                line_sub_data->point_data[point_index + 1].point_array.y), line_sub_data->line_width, line_sub_data->line_color, TRUE);
            draw_circle(painter, line_sub_data->point_data[point_index].point_array.x, line_sub_data->point_data[point_index].point_array.y, 10, line_sub_data->point_color, TRUE);
    //        draw_circle(painter, point_array[counter + 1].x, point_array[counter + 1].y, 3, point_color, TRUE);
        }
        draw_circle(painter, line_sub_data->point_data[point_index].point_array.x, line_sub_data->point_data[point_index].point_array.y, 10, line_sub_data->point_color, TRUE);
        if (line_sub_data->next != NULL)
        {
            line_sub_data = line_sub_data->next;
        }
    }
    if (line_sub_data->draw_default_bmp_info_fun)
    {
        line_sub_data->draw_default_bmp_info_fun(self);
    }
    if (line_sub_data->draw_line_chart_text_fun)
    {
        line_sub_data->draw_line_chart_text_fun(self);
    }

	return TRUE;
}

static BOOL line_chart_on_destroy(UILineChart *self)
{
	delete_line_chart_private_data(self->private_data);

	return TRUE;
}


//-------------------------------------------------------------------------------------------------
// interface
//-------------------------------------------------------------------------------------------------
UILineChart* create_line_chart(const char *id)
{
	UIWindow *line_chart;
	line_chart = create_control(id, 0, 0, LINE_CHART_DEFAULT_WIDTH, LINE_CHART_DEFAULT_HEIGHT);
	if (line_chart != NULL)
	{

		UILineChartData *line_chart_data = new_line_chart_private_data();
		if (line_chart_data == NULL)
		{
			gui_free(line_chart);
			return NULL;
		}
		line_chart->private_data = line_chart_data;
		line_chart->focus_stop = FALSE;
		line_chart->class_name = LINE_CHART_CLASS_NAME;
		line_chart->on_draw = line_chart_on_draw;
		line_chart->on_destroy = line_chart_on_destroy;
	}
	return line_chart;
}

int set_line_chart_point_bmp(UILineChart *line_chart, int line_chart_index, bmp_icon_type *bmp, char num)
{
    UILineChartData *line_chart_data;
    int index = 0;
	char mini_item_num;

	if (!line_chart || !line_chart->private_data)
	{
		return -1;
	}

	line_chart_data = (UILineChartData *)line_chart->private_data;
	UILineChartSubData *line_sub_data = line_chart_data->line_data;

	if (line_chart_index > line_chart_data->total_num || !line_sub_data)
    {
        return -1;
    }
    while (line_chart_index)
    {
        line_sub_data = line_sub_data->next;
        line_chart_index--;
    }

	mini_item_num = MIN(num, line_sub_data->point_num);

	for (index = 0; index < mini_item_num; index++)
    {
        line_sub_data->point_data[index].bmp_info.bmp = bmp[index];
        line_sub_data->point_data[index].bmp_info.icon_color = 0xFFFF0000;
        line_sub_data->point_data[index].bmp_info.mode = DRAW_BMP_MODE_NORMAL;
    }

	invalidate_window(line_chart);
	return 0;
}

int set_total_line_chart_num(UILineChart *line_chart, int total_num)
{
    UILineChartData *line_chart_data;
	UILineChartSubData *new_line_sub_data = NULL;
	int roop_num = 0;

	if (!line_chart || !line_chart->private_data || total_num <= 0 && total_num > MAX_LINE_CHART_NUM)
	{
		return -1;
	}

	line_chart_data = (UILineChartData *)line_chart->private_data;
    UILineChartSubData *line_sub_data = line_chart_data->line_data;
    if (!line_sub_data)
    {
        return -1;
    }

	if (total_num > line_chart_data->total_num)
    {
        roop_num = total_num - line_chart_data->total_num;
        while (line_sub_data->next != NULL)
        {
            line_sub_data = line_sub_data->next;
        }
        for (; roop_num >= 0; roop_num--)
        {
            new_line_sub_data = (UILineChartSubData *)gui_malloc(sizeof(UILineChartSubData));

            set_default_line_son_data(new_line_sub_data);
            line_sub_data->next = new_line_sub_data;
            line_sub_data = line_sub_data->next;
        }
        line_sub_data->next = NULL;
    }
    else if (total_num < line_chart_data->total_num)
    {
        while (total_num)
        {
            line_sub_data = line_sub_data->next;
            total_num --;
        }
        if (line_sub_data->next != NULL)
        {
            while (line_sub_data->next != NULL)
            {
                new_line_sub_data = line_sub_data->next;
                gui_free(line_sub_data);
                line_sub_data = new_line_sub_data;
            }
        }
        else
        {
            gui_free(line_sub_data);
        }
    }
	if (line_chart_data->total_num == total_num)
    {
        return 0;
    }
	line_chart_data->total_num = total_num;
	invalidate_window(line_chart);

	return 0;
}

int delete_line_chart_point_array(UILineChart *line_chart, int line_chart_index)
{
    UILineChartData *line_chart_data;
    int save_line_chart_index = line_chart_index;
	int index = 0;

	if (!line_chart || !line_chart->private_data)
	{
		return -1;
	}

	line_chart_data = (UILineChartData *)line_chart->private_data;
	UILineChartSubData *line_sub_data = line_chart_data->line_data;
	UILineChartSubData *line_before_data = NULL;

	if (line_chart_index > line_chart_data->total_num || !line_sub_data)
    {
        return -1;
    }
    while (line_chart_index)
    {
        line_before_data = line_sub_data;
        line_sub_data = line_sub_data->next;
        line_chart_index--;
    }
    if (save_line_chart_index != 0)
    {
        if(line_sub_data->next != NULL)
        {
            line_before_data->next = line_sub_data->next;
        }
        else
        {
            line_before_data->next = NULL;
        }
    }
    else
    {
        if(line_sub_data->next != NULL)
        {
            line_chart_data->line_data = line_sub_data->next;
        }
    }
    gui_free(line_sub_data);
    if (!line_chart_data->total_num --)
    {
        return -1;
    }
	invalidate_window(line_chart);
	return 0;

}

BOOL set_line_chart_rect(UILineChart *line_chart, UISize size)
{
    int index = 0;
    int lowboundry = 0 ,hignboundry = 0;
    UILineChartData *line_chart_data;

    if (!line_chart || !line_chart->private_data)
	{
		return FALSE;
	}

	line_chart_data = (UILineChartData *)line_chart->private_data;
	UILineChartSubData *line_sub_data_get_boundry = line_chart_data->line_data;
	UILineChartSubData *line_sub_data_get_realpoint = line_chart_data->line_data;
	set_window_size(line_chart, size);
	lowboundry = hignboundry = line_sub_data_get_boundry->point_value[0];
    for (index = 0; index < line_chart_data->total_num; index++)
    {
        get_coordinate_boundry(line_sub_data_get_boundry->point_value, &lowboundry, &hignboundry, line_sub_data_get_boundry->point_num);

        line_sub_data_get_boundry = line_sub_data_get_boundry->next;
    }

    for (index = 0; index < line_chart_data->total_num; index++)
    {
        get_point_coordinate(line_chart->rect, line_sub_data_get_realpoint->point_value, line_sub_data_get_realpoint->point_num, lowboundry, hignboundry, line_sub_data_get_realpoint);
        line_sub_data_get_realpoint = line_sub_data_get_realpoint->next;
    }

	invalidate_window(line_chart);
	return TRUE;
}


int set_line_chart_point_array(UILineChart *line_chart, int point_num, int line_chart_index, int *array_name)
{
    UILineChartData *line_chart_data;
    UILineChartSubData *line_sub_data_get_other_boundry;
    UILineChartSubData *line_sub_data_get_other_point;
    int index = 0;
    int lowboundry = 0 ,hignboundry = 0;

	if (!line_chart || !line_chart->private_data)
	{
		return -1;
	}

	line_chart_data = (UILineChartData *)line_chart->private_data;
	UILineChartSubData *line_sub_cur_data = line_chart_data->line_data;
	line_sub_data_get_other_boundry = line_sub_data_get_other_point = line_chart_data->line_data;

	if (line_chart_index > line_chart_data->total_num || !line_sub_cur_data)
    {
        return -1;
    }
    while (line_chart_index)
    {
        line_sub_cur_data = line_sub_cur_data->next;
        line_chart_index--;
    }

    gui_memset(line_sub_cur_data->point_data, 0, sizeof(UILineChartPointData));
    for (index = 0; index < point_num; index++)
    {
        line_sub_cur_data->point_data[index].point_array.x = 10 + 50 * index;
        line_sub_cur_data->point_data[index].point_array.y = 10 + 20 * index;
        line_sub_cur_data->point_value[index] = array_name[index];
    }

    line_sub_cur_data->point_num = point_num;
    lowboundry = hignboundry = array_name[0];
    get_coordinate_boundry(array_name, &lowboundry, &hignboundry, line_sub_cur_data->point_num);
    for (index = 0; index < line_chart_data->total_num; index ++)
	{
		if (line_sub_data_get_other_boundry != line_sub_cur_data)
		{
			get_coordinate_boundry(line_sub_data_get_other_boundry->point_value, &lowboundry, &hignboundry, line_sub_data_get_other_boundry->point_num);
		}
		line_sub_data_get_other_boundry = line_sub_data_get_other_boundry->next;
	}

    get_point_coordinate(line_chart->rect, array_name, line_sub_cur_data->point_num, lowboundry, hignboundry, line_sub_cur_data);
    for (index = 0; index < line_chart_data->total_num; index ++)
	{
		if (line_sub_data_get_other_point != line_sub_cur_data)
		{
			get_point_coordinate(line_chart->rect, line_sub_data_get_other_point->point_value, line_sub_data_get_other_point->point_num, lowboundry, hignboundry, line_sub_data_get_other_point);
		}
		line_sub_data_get_other_point = line_sub_data_get_other_point->next;
	}

	invalidate_window(line_chart);
	return 0;


}

int set_line_chart_color(UILineChart *line_chart, int line_chart_index, U32 line_color, U32 point_color)
{
    UILineChartData *line_chart_data;

	if (!line_chart || !line_chart->private_data)
	{
		return -1;
	}

	line_chart_data = (UILineChartData *)line_chart->private_data;
	UILineChartSubData *line_sub_data = line_chart_data->line_data;

	if (line_chart_index > line_chart_data->total_num || !line_sub_data)
    {
        return -1;
    }
    while (line_chart_index)
    {
        line_sub_data = line_sub_data->next;
        line_chart_index--;
    }
    if (line_sub_data->line_color == line_color && line_sub_data->point_color == point_color )
	{
		return 0;
	}
	line_sub_data->line_color = line_color;
    line_sub_data->point_color = point_color;
	invalidate_window(line_chart);

	return 0;

}

int set_line_chart_line_color(UILineChart *line_chart, int line_chart_index, U32 line_color)
{
	UILineChartData *line_chart_data;

	if (!line_chart || !line_chart->private_data)
	{
		return -1;
	}

	line_chart_data = (UILineChartData *)line_chart->private_data;
	UILineChartSubData *line_sub_data = line_chart_data->line_data;
	if (line_chart_index > line_chart_data->total_num || !line_sub_data)
    {
        return -1;
    }
    while (line_chart_index)
    {
        line_sub_data = line_sub_data->next;
        line_chart_index--;
    }
    if (line_sub_data->line_color == line_color)
	{
		return 0;
	}
	line_sub_data->line_color = line_color;
	invalidate_window(line_chart);

	return 0;
}

int set_line_chart_point_color(UILineChart *line_chart, int line_chart_index, U32 point_color)
{
	UILineChartData *line_chart_data;

	if (!line_chart || !line_chart->private_data)
	{
		return -1;
	}

	line_chart_data = (UILineChartData *)line_chart->private_data;

	UILineChartSubData *line_sub_data = line_chart_data->line_data;
	if (line_chart_index > line_chart_data->total_num || !line_sub_data)
    {
        return -1;
    }
    while (line_chart_index)
    {
        line_sub_data = line_sub_data->next;
        line_chart_index--;
    }

    if (line_sub_data->point_color == point_color)
	{
		return 0;
	}
    line_sub_data->point_color = point_color;
	invalidate_window(line_chart);

	return 0;
}

int set_line_chart_text_color(UILineChart *line_chart, int line_chart_index, U32 text_color)
{
	UILineChartData *line_chart_data;

	if (!line_chart || !line_chart->private_data)
	{
		return -1;
	}

	line_chart_data = (UILineChartData *)line_chart->private_data;

	UILineChartSubData *line_sub_data = line_chart_data->line_data;
	if (line_chart_index > line_chart_data->total_num || !line_sub_data)
    {
        return -1;
    }
    while (line_chart_index)
    {
        line_sub_data = line_sub_data->next;
        line_chart_index--;
    }

    if (line_sub_data->string_color == text_color)
	{
		return 0;
	}
    line_sub_data->string_color = text_color;
	invalidate_window(line_chart);

	return 0;
}

int set_line_chart_size(UILineChart *line_chart, int line_chart_index, int width, U32 radius)
{
	UILineChartData *line_chart_data;

	if (!line_chart || !line_chart->private_data)
	{
		return -1;
	}
	line_chart_data = (UILineChartData *)line_chart->private_data;

	UILineChartSubData *line_sub_data = line_chart_data->line_data;
	if (line_chart_index > line_chart_data->total_num || !line_sub_data)
    {
        return -1;
    }
    while (line_chart_index)
    {
        line_sub_data = line_sub_data->next;
        line_chart_index--;
    }
	if (line_sub_data->line_width == width && line_sub_data->radius == radius)
	{
		return 0;
	}
	line_sub_data->line_width = width;
	line_sub_data->radius = radius;
	invalidate_window(line_chart);

	return 0;
}

int set_line_chart_line_width(UILineChart *line_chart, int line_chart_index, int line_width)
{
    UILineChartData *line_chart_data;

	if (!line_chart || !line_chart->private_data)
	{
		return -1;
	}
	line_chart_data = (UILineChartData *)line_chart->private_data;

	UILineChartSubData *line_sub_data = line_chart_data->line_data;
	if (line_chart_index > line_chart_data->total_num || !line_sub_data)
    {
        return -1;
    }
    while (line_chart_index)
    {
        line_sub_data = line_sub_data->next;
        line_chart_index--;
    }
	if (line_sub_data->line_width == line_width)
	{
		return 0;
	}
	line_sub_data->line_width = line_width;
	invalidate_window(line_chart);

	return 0;
}
#endif
