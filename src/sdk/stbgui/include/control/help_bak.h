/*
 * help.h
 *
 *  Created on: 2018-9-11
 *      Author: Grace
 */

#ifndef _GUI_HELP_H_
#define _GUI_HELP_H_


#define GET_BMP_BY_NAME			(1)
typedef UIWindow UIHelp;
#define HELP_CLASS_NAME "help"

#define BMP_ARROWLEFT		("ARROWLEFT")
#define BMP_ARROWLR			("ARROWLR")
#define BMP_ARROWRIGHT		("ARROWRIGHT")
#define BMP_BACKWARD		("BACKWARD")
#define BMP_BLUEBALL		("BLUEBALL")
#define BMP_PLAY			("PLAY")
#define BMP_PREVIEW			("PREVIEW")
#define BMP_STOP			("STOP")
#define BMP_NEXT			("NEXT")
#define BMP_PAUSE			("PAUSE")
#define BMP_FORWARD			("FORWARD")
#define BMP_BLUE_CIRCLE		("BLUE_CIRCLE")		/* draw blue button for help banner, only used by function "DrawHelpCommon", same below. */
#define BMP_RED_CIRCLE		("RED_CIRCLE")		/* red button */
#define BMP_GREEN_CIRCLE	("GREEN_CIRCLE")	/* green button */
#define BMP_YELLOW_CIRCLE	("YELLOW_CIRCLE")	/* yellow button */
#define BMP_MV			    ("MV")
#define BMP_CH			    ("CH")
#define BMP_EXIT			("EXIT")
#define BMP_RECALL			("RECALL_HELP")
#define BMP_REC_HELP		("REC_HELP")
#define BMP_SUB_HELP		("SUB_HELP")
#define BMP_MENUICONLIGHT41	("MENUICONLIGHT41")
#define BMP_PICTURE1		("PICTURE1")
#define BMP_PICTURE2		("PICTURE2")
#define BMP_SOFTWARE1		("SOFTWARE1")
#define BMP_SOFTWARE2		("SOFTWARE2")
#define BMP_FAV			    ("FAV")
#define BMP_OK			    ("OK")
#define BMP_0_9				("0-9_HELP")		/* draw string "0-9" */
#define BMP_F1				("F1_HELP")		/* draw string "[F1]" */
#define BMP_F2				("F2_HELP")	/* draw string "[F2]" */
#define BMP_F3				("F3_HELP")	/* draw string "[F3]" */
#define BMP_F4				("F4_HELP")	/* draw string "[F4]" */
#define BMP_INFO			("INFO_HELP")	/* draw bmp "info" */
#define BMP_2_4_6_8			("2468_HELP")
#define BMP_4_6				("46_HELP")
#define BMP_2_5_7_0			("2570_HELP")
#define BMP_REC_HELP		("REC_HELP")
#define BMP_SUB_HELP		("SUB_HELP")
#define BMP_FIND			("FIND_HELP")
#define BMP_MENU			("MENU_HELP")
#define BMP_RECALL			("RECALL_HELP")
#define BMP_EPG_HELP		("EPG_HELP")
#define BMP_ZOOM_HELP		("ZOOM_HELP")
#define BMP_VOL_HELP		("VOL_HELP")
#define BMP_0				("0_HELP")
#define BMP_FULLSCREEN		("FULLSCREEN_HELP")
#define BMP_SHIFT			("SHIFT_HELP")
#define BMP_MINEBLUEHELP	("MINEBLUEHELP")
#define BMP_MINEGREENHELP	("MINEGREENHELP")
#define BMP_MINEREDHELP		("MINEREDHELP")
#define BMP_MINEYELLOWHELP	("MINEYELLOWHELP")
#define BMP_MAX				(0)

#define MAX_HELP_ITEM					(12)
#define NIGHTELF_MAX_HELP_ITEM			(9)
#define NIGHTELF_DEFAULT_HELPSTATUS		(0)

#define HELP_STRING_MAX_LENGTH			(80)
#define HELP_ITEM_BMP_MAX_NUM			(5)

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))
#define INVALID_BMP_ADDR	(0xffffffff)


typedef enum
{
    RED_KEY,
    GREEN_KEY,
    YELLOW_KEY, //10
    BLUE_KEY,
}COLOR_KEY;

typedef enum
{
	COLOR_KEY_ORDER_RGYB,
	COLOR_KEY_ORDER_RBGY,
	COLOR_KEY_ORDER_NUM
}COLOR_KEY_ORDER;

typedef enum
{
	LEFT_JUST,
	CENTER_JUST,
	RIGHT_JUST,
	FULL_JUST,
}text_justif_mode_t;

typedef enum
{
	DEFAULT_HELP_STYLE,
	CLASSIC_HELP_STYLE1,
	CLASSIC_HELP_STYLE2,
	WEATHER_HELP_STYLE,//for weather draw deferent help background
	INFOBAR_HELP_STYLE,//for draw infobar help common
} HELP_PANEL_STYLE;

typedef struct _ui_help_item_bmp_
{
	bmp_icon_type bmp;
	ICON_TYPE icon_type;
	UIColor icon_color;
	DrawBmpMode mode;
	DrawModeParam mode_param;
}UIHelpItemBmp;

enum
{
    HELP_ITEM_RED_KEY,
    HELP_ITEM_MENU_KEY,
    HELP_ITEM_SUB_KEY,
};

typedef struct _ui_help_item_
{
//	int keys[HELP_ITEM_BMP_MAX_NUM];
    UIHelpItemBmp bmp_info[HELP_ITEM_BMP_MAX_NUM];
	char string[HELP_STRING_MAX_LENGTH];
	UIColor string_color;
	UIRect rect;
	U16 string_width;
	U16 item_width;
	U32 max_width  :16;
	U32 bmp_num    :8;
	U32 visible    :1;
	U32 reserved   :7;
}UIHelpItem;

typedef struct _ui_help_inter_
{
	/* internal use */
	U32 bmp_text_space   :8;
	U32 bmp_gap          :8;
	U32 radius           :8;
	U32 border_space	 :8;

	U32 item_gap		 	:8;
	U32 skin_style       	:8;
	U32 draw_style       	:8;
	U32 align_style      	:4;
	U32 align_to_grid    	:1;
	U32 sort_color_item  	:1;
	U32 reserved			:2;

	BOOL (*draw_default_item_fun)(UIHelp *self);
}UIHelpInter;

typedef struct _ui_help_data_
{
	/* property */
	UIHelpItem help_item[MAX_HELP_ITEM];
	unsigned char item_number;

	/* internal use */
	UIHelpInter help_internal;
}UIHelpData;

UIHelp* create_help(char *id);
void init_help_params_gui(UIHelp *help, bmp_icon_type *bmp, string_table *string, char num);
void set_help_item_align_to_grid(UIHelp *help, BOOL align_to_grid);
void set_help_item_align_style_gui(UIHelp *help, U32 align_style);
void set_help_item_draw_style(UIHelp *help, U32 draw_style);
void set_help_item_skin_style(UIHelp *help, U32 skin_style);
void set_help_item_bmp_text_space(UIHelp *help, U32 bmp_text_space);
void set_help_item_gap(UIHelp *help, U32 item_gap);
void set_help_item_border_space(UIHelp *help, U32 border_space);
U32 get_help_item_align_style(void);
U32 get_help_item_draw_style(void);
U32 get_help_item_skin_style(void);


BOOL set_help_item_visible_by_index_gui(UIHelp *help, int item_index, BOOL visible);
BOOL set_help_item_visible_by_bmp_gui(UIHelp *help, bmp_icon_type bmp, BOOL visible);
BOOL set_help_item_visible_by_string_gui(UIHelp *help, char *string, BOOL visible);
//int get_total_visible_item_num_gui(char *id);
//BOOL hide_all_help_items_gui(char *id);
BOOL set_help_item_bmp_gui(UIHelp *help, int item_index, bmp_icon_type* bmp);
BOOL set_help_item_bmp_num_gui(UIHelp *help, int item_index, int bmp_num);
BOOL set_help_item_bmp_ex_gui(UIHelp *help, int item_index, int bmp_index, bmp_icon_type bmp);
BOOL set_help_item_bmp_by_string_gui(UIHelp *help, char *string, bmp_icon_type* bmp);
BOOL set_help_item_string_gui(UIHelp *help, int item_index, char *string);
BOOL set_help_item_string_by_bmp_gui(UIHelp *help, bmp_icon_type bmp, char *string);
BOOL set_help_item_info_gui(UIHelp *help, int item_index, bmp_icon_type *bmp, char *string);
//BOOL set_help_item_max_width_by_index_gui(char *id, int item_index, int max_width);
//BOOL set_help_item_max_width_by_bmp_gui(char *id, bmp_icon_type bmp, int max_width);
BOOL set_help_item_icon_type_by_bmp_gui(UIHelp *help, bmp_icon_type bmp, ICON_TYPE type, UIColor color);
BOOL set_help_item_string_color_by_bmp_gui(UIHelp *help, bmp_icon_type bmp, UIColor color);

void get_help_item_position_info(UIHelp *help);

int conver_bmp_to_text(char *buf, int buf_len, bmp_icon_type eBmp);
#endif /* _GUI_PAGE_H_ */
