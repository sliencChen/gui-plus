/*
 * text_field.h
 *
 *  Created on: 2019-6-17
 *      Author: Brant
 */

#ifndef _GUI_TEXTFIELD_H_
#define _GUI_TEXTFIELD_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define MAX_TEXT_LINE_NUM		(128)
#define TEXT_FIELD_TEXT_SIZE_DEFAULT (64)

typedef UIWindow UITextField;
#define TEXTFIELD_CLASS_NAME "text_field"

#define TEXTFIELD_DEFAULT_WIDTH  200
#define TEXTFIELD_DEFAULT_HEIGHT  50

typedef struct _line_range_
{
	U32 offset;
	U32 length;
} LineRange;

struct _ui_textfield_data_;
typedef int (*CheckLimitFunc)(struct _ui_textfield_data_ *textfield_data, char *utf8_string);

typedef struct _ui_textfield_data_
{
    /* appearance */
	U32 text_color;

	/* property */
	BOOL editable;
	BOOL multi_line;
	int font_size;
	int font_scale_x;	/* percent, font_scale_x = 100: 100% */
	int font_scale_y;
	float line_spacing;
	int text_direction;
	BOOL align_center;
	//BOOL draw_bkg;	/* sometimes can not draw background, such as usb subtitle(it use 256 color osd) */
	BOOL page_loop;	/* enable only when none edit mode */

	/* interface */
	CheckLimitFunc char_num_can_be_inserted;

	/* internal use */
	U16 *text;				/* use unicode */
	int char_num;
	int text_size;
	int caret_pos;
	LineRange line_info[MAX_TEXT_LINE_NUM];
	int line_num;
	int start_line;
	int prev_caret_line;

	BOOL need_draw_sidelong;
	int sidelong_x_offset;
	int sidelong_y_offset;
} UITextFieldData;


UITextField* create_text_field(const char *id);

/* property */
BOOL set_text_field_text(UITextField *textfield, const char *text, int encoding);
BOOL set_text_field_text_color(UITextField *textfield, U32 text_color);
BOOL set_text_field_with_size(UITextField *textfield, int size);
BOOL set_text_field_editable(UITextField *textfield, BOOL editable);
BOOL set_text_field_multi_line(UITextField *textfield, BOOL multi_line);
BOOL set_text_field_font_size(UITextField *textfield, int font_size);
BOOL set_text_field_font_scale(UITextField *textfield, int scale_x, int scale_y);
BOOL set_text_field_line_spacing(UITextField *textfield, float line_spacing);
BOOL set_text_field_text_direction(UITextField *textfield, int text_direction);
BOOL set_text_field_align_center(UITextField *textfield, BOOL align_center);
BOOL set_text_field_check_limit_callback(UITextField *textfield, CheckLimitFunc check_limit_cb);
BOOL set_text_field_page_loop(UITextField *textfield, BOOL loop);
BOOL set_text_region_size(UITextField *textfield, U32 width, U32 height);

int get_text_field_line_height(UITextField *textfield);
int get_text_field_cur_page(UITextField *textfield);
int get_text_field_page_num(UITextField *textfield);
int get_text_field_line_num(UITextField *textfield);
BOOL get_text_field_string(UITextField *textfield, int encoding, char *str_buf, int buf_size);
int get_text_field_string_length(UITextField *textfield);	/* in utf-8 encoding */
int get_text_field_text_size(UITextField *textfield);
/* for scroll bar */
int get_text_field_slider_percent(UITextField *textfield);
int get_text_field_bar_value(UITextField *textfield);


/* action */
BOOL text_field_page_up(UITextField *textfield);
BOOL text_field_page_down(UITextField *textfield);
BOOL text_field_page_end(UITextField *textfield);
BOOL text_field_minimize(UITextField *textfield);


/* edit text */
BOOL move_text_field_caret_forward(UITextField *textfield);
BOOL move_text_field_caret_backward(UITextField *textfield);
BOOL move_text_field_caret_up(UITextField *textfield);
BOOL move_text_field_caret_down(UITextField *textfield);
BOOL move_text_field_caret_to_home(UITextField *textfield);
BOOL move_text_field_caret_to_end(UITextField *textfield);

BOOL text_field_remove_character(UITextField *textfield);
BOOL text_field_remove_all_character(UITextField *textfield);
BOOL text_field_insert_string(UITextField *textfield, const char *utf8_string);
BOOL text_field_insert_character(UITextField *textfield, U16 ucs2_char);



#ifdef __cplusplus
}
#endif

#endif /* _GUI_TEXTFIELD_H_ */

