/*
 * help.h
 *
 *  Created on: 2018-9-11
 *      Author: Grace
 */

#ifndef _GUI_HELP_H_
#define _GUI_HELP_H_

#ifdef __cplusplus
extern "C"
{
#endif


typedef UIWindow			UIHelp;
#define HELP_CLASS_NAME		"Help"

#define HELP_ITEM_MAX_ID	(5)
#define MAX_HELP_ITEM		(12)


typedef struct _ui_help_item_
{
	int ids[HELP_ITEM_MAX_ID];
	UIString text;
	UISize size;
	U32 id_num		:8;
	U32 visible		:1;
	U32 reserved	:23;
} UIHelpItem;

typedef struct _ui_help_item_icon_
{
	union
	{
		const char *text;		/* text icon */
		const char *bmp_name;	/* bmp icon */
		int radius;				/* circle icon */
	} data;
	UIColor color;
	UISize (*get_size)(struct _ui_help_item_icon_ *self);
	void (*draw)(struct _ui_help_item_icon_ *self, UIPainter *painter, UIRect rect);
} UIHelpItemIcon;

typedef struct _ui_help_data_
{
	/* property */
	UIHelpItem help_item[MAX_HELP_ITEM];
	U32 item_num	:8;
	U32 reserved	:24;
} UIHelpData;

void text_help_icon(UIHelpItemIcon *icon, const char *text);
void bitmap_help_icon(UIHelpItemIcon *icon, const char *bmp_name);
void circle_help_icon(UIHelpItemIcon *icon, int radius);
UIHelpItemIcon default_convert_function(int icon_id);


//-------------------------------------------------------------------------------------------------
// interface
//-------------------------------------------------------------------------------------------------
UIHelp* create_help(const char *id);



//-------------------------------------------------------------------------------------------------
// typical theme
//-------------------------------------------------------------------------------------------------
typedef struct _ui_help_theme_base_
{
	U16 icon_text_space;
	U16 icon_space;
	U16 item_space;
	U16 reserved;
	UIColor text_color;
} UIHelpThemeBase;

typedef struct _ui_help_theme_normal_
{
	UIHelpThemeBase base;
	UIHelpItemIcon (*conv_func)(int icon_id);
} UIHelpThemeNormal;

BOOL help_theme_normal(UIHelp *self, UIHelpThemeNormal *theme_param);


#ifdef __cplusplus
}
#endif

#endif /* _GUI_HELP_H_ */
