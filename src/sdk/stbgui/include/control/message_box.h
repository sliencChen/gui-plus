/*
 * message_box.h
 *
 *  Created on: 2019-7-29
 *      Author: Silence
 */

#ifndef _MESSAGE_BOX_H_
#define _MESSAGE_BOX_H_

#ifdef __cplusplus
extern "C"
{
#endif


#include <gui.h>
#include <control/prompt.h>

typedef UIWindow						UIMessageBox;
#define MESSAGE_BOX_CLASS_NAME		PROMPT_CLASS_NAME "/MessageBox"

#define MESSAGE_BOX_DEFAULT_WIDTH	300
#define MESSAGE_BOX_DEFAULT_HEIGHT	150

typedef enum
{
	SELECT_OPTION_TYPE,
	INPUT_NUMERIC_TYPE,
}MessageBoxActionType;


typedef struct select_option
{
	char **option_string;
	U32 option_num			:8;
	U32	selected_option		:8;
	U32 reserved			:16;
}SelectOption;


typedef struct input_numeric
{
	char *input_numeric;
	U32	digit_num			:8;	  /*password-4 or password-6*/
	U32	input_position 		:8;
	U32 reserved			:16;
}InputNumeric;


typedef struct _ui_message_box_data_
{
	/* inherits from popup */
	UIPopupData popup;

	/* internal use */
	UIString content;
	int duration;       /*millisecond*/
	UITextField *text_field;
	UIBoxSizer *msg_box_sizer;
	MessageBoxActionType msg_box_action_type;
	union
	{
		SelectOption select;
		InputNumeric  input;  /*input password*/
	};
}UIMessageBoxData;


void default_message_box_data(UIMessageBoxData *message_box_data);


//-------------------------------------------------------------------------------------------------
// interface
//-------------------------------------------------------------------------------------------------
UIMessageBox *create_message_box(const char *id);
int set_message_box_title(UIMessageBox *message_box, const char *text);
int set_message_box_content(UIMessageBox *message_box, const char *text);
int	set_message_box_druation(UIMessageBox *message_box, int duration_ms);
int set_message_box_centered(UIMessageBox *message_box, BOOL centered);
int	set_message_action_type(UIMessageBox *message_box, BOOL msg_box_action_type);

int set_message_box_option_string_array(UIMessageBox *message_box, const char **option_string_array);
int set_message_box_option_num(UIMessageBox *message_box, int option_num);
int get_message_box_option_num(UIMessageBox *message_box);
int set_message_box_selected_option(UIMessageBox *message_box, int selected_option);
int get_message_box_selected_option(UIMessageBox *message_box);
char *get_message_box_selected_option_string(UIMessageBox *message_box);

int set_message_box_digit_num(UIMessageBox *message_box, int digit_num);
int get_message_box_digit_num(UIMessageBox *message_box);
int set_message_box_input_numeric(UIMessageBox *message_box, const char *input_numeric);
char *get_message_box_input_numeric(UIMessageBox *message_box);
int set_message_box_input_position(UIMessageBox *message_box, int input_position);

int set_message_box_sizer(UIMessageBox *message_box, UISizer *sizer);


//-------------------------------------------------------------------------------------------------
// typical theme
//-------------------------------------------------------------------------------------------------
typedef int (*UIMsgBoxSelectFieldInsetGraphicOnDraw)(UIMessageBox* message_box, void *theme_param, UIRect rect);


typedef struct _ui_visible_option_item_
{
	UIColor item_inset_high_light_color;
	UIColor item_inset_normal_color;
	UIColor item_text_color;
	UIColor item_border_color;
	int item_border_size;
	UISize item_size;
	int font_size;
	UIInset item_text_margin;
}VisibleOptionItem;


typedef struct _ui_message_box_theme_select_field_default_
{
	int visible_option_num;  /*how many options visible at the same time*/
	int option_item_space;
	VisibleOptionItem option_item;
	/*only use for visible_option_num = 1*/
	U32 inset_graphic_color;
	U16 inset_graphic_width;
	UIInset field_inset_left_graphic_margin;
	UIInset field_inset_righ_graphic_margin;
	UIMsgBoxSelectFieldInsetGraphicOnDraw inset_graphic_on_draw;
}UIMsgBoxThemeSelectFieldDefault;


typedef struct _ui_input_item_
{
	UIColor item_bkg_color;
	UIColor text_high_light_color;
	UIColor text_normal_color;
	UIColor border_color;
	int font_size;
	int border_size;
	UISize item_size;
}InputItem;


typedef struct _ui_message_box_theme_input_field_default_
{
	int input_item_space;
	U32 numeric_is_show		:1;
	U32 reserved			:31;
	InputItem input_item;
}UIMsgBoxThemeInputFieldDefault;


typedef struct _message_theme_default_
{
	UIPopupThemeDefaultData popup_theme_default_data;
	union
	{
		UIMsgBoxThemeSelectFieldDefault select_field_default_data;
		UIMsgBoxThemeInputFieldDefault input_field_default_data;
	};
}UIMessageBoxThemeDefaultData;


BOOL message_box_theme_default(UIMessageBox* message_box, UIMessageBoxThemeDefaultData *theme_param);

int message_box_select_field_inset_draw_graphic_default(UIMessageBox* message_box, void *theme_param, UIRect rect);


#ifdef __cplusplus
}
#endif

#endif
