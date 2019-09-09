/*
 * graphic.h
 *
 *  Created on: 2017-12-29
 *      Author: Brant
 */

#ifndef _GUI_GRAPHIC_H_
#define _GUI_GRAPHIC_H_

typedef enum
{
	FIRST_AREA_GUI,
	SECOND_AREA_GUI,
	THIRD_AREA_GUI,
	FOURTH_AREA_GUI,
	FIFTH_AREA_GUI,
	SIXTH_AREA_GUI,
	SEVENTH_AREA_GUI,
	EIGHTH_AREA_GUI,
	NINTH_AREA_GUI,
}AREA_TYPE_GUI;

/*
 * set pixel color
 */
int set_pixel(UIPainter *painter, UIPoint point, UIColor color);

/*
 * draw a line
 */
int draw_line(UIPainter *painter, UIPoint start_pt, UIPoint end_pt, int width, UIColor color, BOOL use_aa);

/*
 * draw chart line
 * 根据位置点数组，画出折线图。
 * point_array[in] : 每个点的坐标位置。
 * total           : 点的个数。
 * line_color      : 线的颜色。
 * point_color     : 点的颜色。
 */
int draw_line_chart_gui(UIPainter *painter, UIPoint *point_array, int total_num, int line_width, U32 radius, UIColor line_color, UIColor point_color);

/*
 * draw coordinate line
 * 天气预报折线图专用函数，画天气预报最高、最低温度值及温度单位
 */
int draw_coordinate_line_gui(UIPainter *painter, UIRect rect, int low_boundry, int hign_boundry, UIColor line_color, UIColor point_color, UIColor data_color, char *unit);

/*
 * draw rectangle
 */
int draw_rect(UIPainter *painter, UIRect rect, UIColor color);

/*
 * draw rect frame
 * 绘制矩形边框
 */
int draw_rect_frame_gui(UIPainter *painter, UIRect rect, int border_width, UIColor color_outside, UIColor color_inside);

/*
 * draw box frame
 * 绘制圆角矩形边框
 */
int draw_box_frame_gui(UIPainter *painter, UIRect rect, int alpha, int border_width, UIColor border_color, UIColor panel_color);

/*
 * draw round rectangle(box button real)/circle. if with equals height and r = width/2, it will show as a circle
 */
int draw_round_rect(UIPainter *painter, UIRect rect, int radius, UIColor color, BOOL use_aa, BOOL type);

/*
 * draw a circle
 * 绘制圆形
 */
int draw_circle(UIPainter *painter, int cx, int cy, U32 radius, UIColor color, BOOL use_aa);

/*
 * draw triangle
 * 绘制指定宽高三角形。
 * iDirection = 0: 向右延伸iLength长度，(iXStart, iYStart)为最左侧中点坐标
 * iDirection = 1: 向左延伸，(iXStart, iYStart)为最右侧中点坐标
 * iDirection = 2: 向上延伸，(iXStart, iYStart)为最底部中点坐标
 * iDirection = 3: 向下延伸，(iXStart, iYStart)为最顶部中点坐标
 * iDirection = 4: 向下延伸，(iXStart, iYStart)为右顶点坐标，绘制等腰直角三角形，目前仅用于标记Calendar中的是否有Memo 2012.1.4
 */
int draw_triangle(UIPainter *painter, int cx, int cy, int direction, int length, UIColor color);

/*
 * draw sector
 * 绘制扇形
 * percentage: 扇形百分比
 */
int draw_sector_gui(UIPainter *painter, int center_x, int center_y, U32 radius, int percentage, UIColor color);

/*
 * 绘制四周为圆角的矩形
 * delta: 圆角的弧度
 */
int draw_box_botton(UIPainter *painter, UIPoint start_pt, UIPoint end_pt, int delta, UIColor color);

/*
 * 绘制两个圆角的矩形
 * delta = 0: 左侧2个角为圆角
 * delta = 1: 右侧2个角为圆角
 * delta = 2: 顶部2个角为圆角
 * delta = 3: 底部2个角为圆角
 */
int draw_box_botton_half(UIPainter *painter, UIPoint start_pt, UIPoint end_pt, int delta, UIColor color, int direct);

/*
 * the corner is circle
 * type=0: four corner is circle
 * type=1: LeftTop is circle,2:RightTop,4:LeftBottom,8:RightBottom
*/
int draw_box_botton_real(UIPainter *painter, UIPoint start_pt, UIPoint end_pt, UIColor color, int radius, int type);
int draw_box_botton_real_aa(UIPainter *painter, UIPoint start_pt, UIPoint end_pt, UIColor color, int radius, int type);

/*
 * 绘制四周为圆角的矩形，颜色渐变
 * iDelta: 圆角的弧度
 * bDirect: 0为垂直渐变，1为水平渐变
 * state:
 * 0-----four circle corner
 * 1-----up circle corner
 * 2-----down circle corner
 */
int draw_box_botton_gradual(UIPainter *painter, UIPoint start_pt, UIPoint end_pt, int iDelta, U32 stColorStart, U32 stColorStop,
        BOOL bDirect, U32 state);

 /*
 * 绘制多次连续渐变
 * color： 为每段的渐变色值
 * total： 最大为3
 */
int draw_box_botton_multi_gradual(UIPainter *painter, UIRect rect, UIColor color1, UIColor color2, UIColor color3, UIColor color4,
        int total, BOOL direct);


#endif /* _GUI_GRAPHIC_H_ */
