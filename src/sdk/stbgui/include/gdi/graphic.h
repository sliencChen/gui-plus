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
 * ����λ�õ����飬��������ͼ��
 * point_array[in] : ÿ���������λ�á�
 * total           : ��ĸ�����
 * line_color      : �ߵ���ɫ��
 * point_color     : �����ɫ��
 */
int draw_line_chart_gui(UIPainter *painter, UIPoint *point_array, int total_num, int line_width, U32 radius, UIColor line_color, UIColor point_color);

/*
 * draw coordinate line
 * ����Ԥ������ͼר�ú�����������Ԥ����ߡ�����¶�ֵ���¶ȵ�λ
 */
int draw_coordinate_line_gui(UIPainter *painter, UIRect rect, int low_boundry, int hign_boundry, UIColor line_color, UIColor point_color, UIColor data_color, char *unit);

/*
 * draw rectangle
 */
int draw_rect(UIPainter *painter, UIRect rect, UIColor color);

/*
 * draw rect frame
 * ���ƾ��α߿�
 */
int draw_rect_frame_gui(UIPainter *painter, UIRect rect, int border_width, UIColor color_outside, UIColor color_inside);

/*
 * draw box frame
 * ����Բ�Ǿ��α߿�
 */
int draw_box_frame_gui(UIPainter *painter, UIRect rect, int alpha, int border_width, UIColor border_color, UIColor panel_color);

/*
 * draw round rectangle(box button real)/circle. if with equals height and r = width/2, it will show as a circle
 */
int draw_round_rect(UIPainter *painter, UIRect rect, int radius, UIColor color, BOOL use_aa, BOOL type);

/*
 * draw a circle
 * ����Բ��
 */
int draw_circle(UIPainter *painter, int cx, int cy, U32 radius, UIColor color, BOOL use_aa);

/*
 * draw triangle
 * ����ָ����������Ρ�
 * iDirection = 0: ��������iLength���ȣ�(iXStart, iYStart)Ϊ������е�����
 * iDirection = 1: �������죬(iXStart, iYStart)Ϊ���Ҳ��е�����
 * iDirection = 2: �������죬(iXStart, iYStart)Ϊ��ײ��е�����
 * iDirection = 3: �������죬(iXStart, iYStart)Ϊ����е�����
 * iDirection = 4: �������죬(iXStart, iYStart)Ϊ�Ҷ������꣬���Ƶ���ֱ�������Σ�Ŀǰ�����ڱ��Calendar�е��Ƿ���Memo 2012.1.4
 */
int draw_triangle(UIPainter *painter, int cx, int cy, int direction, int length, UIColor color);

/*
 * draw sector
 * ��������
 * percentage: ���ΰٷֱ�
 */
int draw_sector_gui(UIPainter *painter, int center_x, int center_y, U32 radius, int percentage, UIColor color);

/*
 * ��������ΪԲ�ǵľ���
 * delta: Բ�ǵĻ���
 */
int draw_box_botton(UIPainter *painter, UIPoint start_pt, UIPoint end_pt, int delta, UIColor color);

/*
 * ��������Բ�ǵľ���
 * delta = 0: ���2����ΪԲ��
 * delta = 1: �Ҳ�2����ΪԲ��
 * delta = 2: ����2����ΪԲ��
 * delta = 3: �ײ�2����ΪԲ��
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
 * ��������ΪԲ�ǵľ��Σ���ɫ����
 * iDelta: Բ�ǵĻ���
 * bDirect: 0Ϊ��ֱ���䣬1Ϊˮƽ����
 * state:
 * 0-----four circle corner
 * 1-----up circle corner
 * 2-----down circle corner
 */
int draw_box_botton_gradual(UIPainter *painter, UIPoint start_pt, UIPoint end_pt, int iDelta, U32 stColorStart, U32 stColorStop,
        BOOL bDirect, U32 state);

 /*
 * ���ƶ����������
 * color�� Ϊÿ�εĽ���ɫֵ
 * total�� ���Ϊ3
 */
int draw_box_botton_multi_gradual(UIPainter *painter, UIRect rect, UIColor color1, UIColor color2, UIColor color3, UIColor color4,
        int total, BOOL direct);


#endif /* _GUI_GRAPHIC_H_ */
