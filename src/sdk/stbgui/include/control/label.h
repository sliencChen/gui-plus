/*
 * label.h
 *
 *  Created on: 2018-7-1
 *      Author: Brant
 */

#ifndef _GUI_LABEL_H_
#define _GUI_LABEL_H_

#ifdef __cplusplus
extern "C"
{
#endif


typedef UIWindow UILabel;
#define LABEL_CLASS_NAME "label"

#define LABEL_DEFAULT_WIDTH  120
#define LABEL_DEFAULT_HEIGHT  30

typedef enum _LABEL_ALIGN_VER_STYLE_
{
	LABEL_ALIGN_CENTER_VER = 1<<4,
	LABEL_ALIGN_TOP = 1<<5,
	LABEL_ALIGN_BOTTOM = 1<<6,
	LABEL_ALIGN_NUM_VER,
} LABEL_VER_ALIGN_STYLE;

typedef enum _LABEL_ALIGN_HOR_STYLE_
{
	LABEL_ALIGN_AUTO = 1<<0,
	LABEL_ALIGN_LEFT = 1<<1,
	LABEL_ALIGN_CENTER_HOR = 1<<2,
	LABEL_ALIGN_RIGHT = 1<<3,
	LABEL_ALIGN_NUM_HOR
} LABEL_HOR_ALIGN_STYLE;

typedef enum _LABEL_TEXT_DIRECTION_
{
	LABEL_WRITE_LTR = 0,
	LABEL_WRITE_RTL,
	LABEL_WRITE_AUTO,
	LABEL_WRITE_NUM
} LABEL_TEXT_DIRECTION;

typedef struct _ui_label_data_
{
	/* property */
	U16 *text;
	U32 color;
	int font_size;
	BOOL auto_size;
	U8 align_style;
	WriteDirection text_direction;

	/* internal use */
	int text_size;      /* text content memory size */
} UILabelData;


UILabel* create_label(const char *id);

int set_label_text(UILabel *label, const char *text);
int set_label_auto_size(UILabel *label, BOOL auto_size);
int set_label_text_color(UILabel *label, U32 color);
int set_label_font_size(UILabel *label, int font_size);
int set_label_align_style(UILabel *label, LABEL_HOR_ALIGN_STYLE hor_style, LABEL_VER_ALIGN_STYLE ver_style);
int set_label_text_dir(UILabel *label, LABEL_TEXT_DIRECTION dir);


#ifdef __cplusplus
}
#endif

#endif /* _GUI_LABEL_H_ */
