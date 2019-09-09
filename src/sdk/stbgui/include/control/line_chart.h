/*
 * line_chart.h
 *
 *  Created on: 2018-9-12
 *      Author: Shelly
 */

#ifndef _GUI_LINE_CHART_H_
#define _GUI_LINE_CHART_H_

#ifdef __cplusplus
extern "C"
{
#endif
typedef UIWindow UILineChart;
#define LINE_CHART_CLASS_NAME "line_chart"
#define MAX_LINE_CHART_NUM				(10)
#define MAX_POINT_ARRAY_NUM              (100)
#define POINT_STRING_MAX_LENGTH          (50)

#define LINE_CHART_DEFAULT_HEIGHT		200
#define LINE_CHART_DEFAULT_WIDTH		500

typedef struct _ui_line_chart_item_bmp_
{
	bmp_icon_type bmp;
//	ICON_TYPE icon_type;
	UIColor icon_color;
	DrawBmpMode mode;
//	DrawModeParam mode_param;
}UILineChartItemBmp;

typedef struct _ui_line_chart_point_data_
{
	/* property */
    UILineChartItemBmp bmp_info;
	UIPoint point_array;
	char point_text[POINT_STRING_MAX_LENGTH];
	U16 string_width;
}UILineChartPointData;


typedef struct _ui_line_chart_sub_data_
{
	/* property */
    int point_num;
    UILineChartPointData point_data[MAX_POINT_ARRAY_NUM];
    int point_value[MAX_POINT_ARRAY_NUM];
    U32 line_color;
    U32 point_color;
	UIColor string_color;
    int line_width;
    U32 radius;
    BOOL (*draw_line_chart_text_fun)(UILineChart *self);
    BOOL (*draw_default_bmp_info_fun)(UILineChart *self);
    struct _ui_line_chart_sub_data_ *next;
}UILineChartSubData;

typedef struct _ui_line_chart_data_
{
	/* property */
	int total_num;
	UILineChartSubData *line_data;

}UILineChartData;

UILineChart* create_line_chart(const char *id);

//int set_line_chart_total_num(char *id, int total_num);
int set_total_line_chart_num(UILineChart *line_chart, int total_num);
int set_line_chart_color(UILineChart *line_chart, int line_chart_index, U32 line_color, U32 point_color);
int set_line_chart_line_color(UILineChart *line_chart, int line_chart_index, U32 line_color);
int set_line_chart_point_color(UILineChart *line_chart, int line_chart_index, U32 point_color);
int set_line_chart_text_color(UILineChart *line_chart, int line_chart_index, U32 text_color);
int set_line_chart_size(UILineChart *line_chart, int line_chart_index, int width, U32 radius);
int set_line_chart_line_width(UILineChart *line_chart, int line_chart_index, int line_width);
int set_line_chart_circle_radius(UILineChart *line_chart, int line_chart_index, U32 radius);
int delete_line_chart_point_array(UILineChart *line_chart, int line_chart_index);
int set_line_chart_point_array(UILineChart *line_chart, int point_num, int line_chart_index, int *array_name);
BOOL set_line_chart_rect(UILineChart *line_chart, UISize size);
int set_line_chart_point_bmp(UILineChart *line_chart, int line_chart_index, bmp_icon_type *bmp, char num);
#ifdef __cplusplus
}
#endif


#endif /* _GUI_LINE_CHART_H_ */
