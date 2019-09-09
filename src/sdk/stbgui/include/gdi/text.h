/*
 * text.h
 *
 *  Created on: 2018-6-29
 *      Author: Brant
 */

#ifndef _GUI_TEXT_H_
#define _GUI_TEXT_H_


typedef enum
{
	WRITE_DIRECTION_LTR,
	WRITE_DIRECTION_RTL,
	WRITE_DIRECTION_NEUTRAL,
	WRITE_DIRECTION_AUTO,
	WRITE_DIRECTION_NUM
} WriteDirection;

typedef struct _draw_text_param_
{
	int font_size;
	int scale_x;
	int scale_y;
	U32 color;
	int direction;
	BOOL rtl_align_right;
	int caret_pos;
} DrawTextParam;


int get_text_width(const char *text, int stop, int font_size, int scale);
int get_ucs2_text_width(const U16 *text, int stop, int font_size, int scale);
int get_text_height(int font_size, int scale);

void default_draw_text_param(DrawTextParam *param);
/* support windows & utf-8 encoding, prefer utf-8 */
int draw_text(UIPainter *painter, UIRect rect, const char *text, int stop, DrawTextParam *param);
int draw_ucs2_text(UIPainter *painter, UIRect rect, U16 *text, int stop, DrawTextParam *param);
BOOL have_rtl_character(U16 *ucs2_text, int total);


int gui_draw_text(UIPainter *painter, UIRect rect, const char *text, UIColor color);



#endif /* _GUI_TEXT_H_ */
