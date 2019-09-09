/*
 * edit_item.h
 *
 *  Created on: 2019-7-23
 *      Author: Silence
 */


#ifndef _EDIT_ITEM_H_
#define _EDIT_ITEM_H_

#ifdef __cplusplus
extern "C"
{
#endif


#include <gui.h>
typedef UIWindow 				UIEditItem;
#define EDIT_ITEM_CLASS_NAME 	"EditItem"

#define EDIT_ITEM_DEFAULT_WIDTH		900
#define EDIT_ITEM_DEFAULT_HEIGHT  	36


typedef enum
{
	EDIT_FIELD_TYPE_SELECT,
	EDIT_FIELD_TYPE_INPUT,
}EditItemFieldType;

typedef int (*InputActionFun)(UIEditItem *, void*);
typedef int (*SelectActionFun)(UIPage *, UIEditItem *);

typedef struct _ui_edit_item_select_field_
{
	UIString select_data;
	SelectActionFun select_action;
}SelectField;

typedef struct _ui_edit_item_input_field_
{
	UIString input_data;
	U16 char_pos;
	U16 max_value;
	InputActionFun input_action;
}InputField;

typedef struct _ui_edit_item_data_
{
	UIString header;
	U16 header_width;
	U32 high_light 		:1;
	U32	reserved        :31;
	EditItemFieldType field_type;
	union
	{
		SelectField select_field;
		InputField input_field;
	}edit_field;
}UIEditItemData;

UIEditItem* create_edit_item(const char *id);
int set_edit_item_header_text(UIEditItem *edit, const char *text);
int set_edit_item_field_text(UIEditItem *edit, const char *text);
int set_edit_item_field_type(UIEditItem *edit, BOOL field_type);
int set_edit_item_high_light(UIEditItem *edit, BOOL high_light);
int set_edit_item_header_width(UIEditItem *edit, U32 header_width);


//-------------------------------------------------------------------------------------------------
// typical theme
//-------------------------------------------------------------------------------------------------
typedef struct _ui_edit_theme_common_
{
	int font_size;
	U32 header_centered		:1;
	U32 field_centered		:1;
	U32	reserved 			:30;
	UIInset header_margin;
	UIInset field_margin;
	UIColor header_bkg_normal_color;
	UIColor header_bkg_light_color;
	UIColor field_bkg_normal_color;
	UIColor field_bkg_light_color;
	U32 header_normal_color;
	U32 header_light_color;
	U32 field_normal_color;
	U32 field_light_color;
}UIEditItemThemeCommon;


typedef int (*EditItemFieldInsetGraphicOnDraw)(UIEditItem *, void *);

typedef struct _ui_edit_field_inset_draw_
{
	U32 inset_graphic_light_color;
	U32 inset_graphic_normal_color;
	U16 inset_light_graphic_width;
	U16 inset_normal_graphic_width;
	U32 normal_inset_show			:1;              /*some edit just show light inset graphic*/
	U32	reserved					:31;

	UIInset field_light_inset_left_graphic_margin;
	UIInset field_light_inset_righ_graphic_margin;

	UIInset field_normal_inset_left_graphic_margin;
	UIInset field_normal_inset_righ_graphic_margin;

	EditItemFieldInsetGraphicOnDraw inset_graphic_on_draw;
}EditItemFieldInsetGraphicDraw;

typedef struct _ui_edit_theme_default_
{
	UIEditItemThemeCommon edit_default_rect;
	EditItemFieldInsetGraphicDraw inset_graphic_draw;
}UIEditItemThemeDefault;

typedef enum
{
	EDIT_FIELD_INSET_TYPE_BMP,
	EDIT_FIELD_INSET_TYPE_DRAW,
}EditItemFieldInsetType;

typedef struct _ui_edit_header_skin_
{
	char *header_normal_left_icon;
	char *header_normal_mid_icon;
	char *header_normal_righ_icon;

	char *header_light_left_icon;
	char *header_light_mid_icon;
	char *header_light_righ_icon;
}EditItemHeaderSkin;

typedef struct _ui_edit_field_inset_skin_
{
	UIInset field_light_inset_left_icon_margin;
	UIInset field_light_inset_righ_icon_margin;
	UIInset field_light_inset_mid_icon_margin;
	char *field_light_inset_left_icon;  //arrow left
	char *field_light_inset_mid_icon;
	char *field_light_inset_righ_icon;	//arrow right

	UIInset field_normal_inset_left_icon_margin;
	UIInset field_normal_inset_righ_icon_margin;
	UIInset field_normal_inset_mid_icon_margin;
	char *field_normal_inset_left_icon;  //arrow left
	char *field_normal_inset_mid_icon;
	char *field_normal_inset_righ_icon;	//arrow right
}EditItemFieldInsetSkin;

typedef struct _ui_edit_field_skin_
{
	char *field_normal_left_icon;
	char *field_normal_mid_icon;
	char *field_normal_righ_icon;

	char *field_light_left_icon;
	char *field_light_mid_icon;
	char *field_light_righ_icon;
	EditItemFieldInsetType inset_type;
	union
	{
		EditItemFieldInsetSkin inset_skin;     /*bmp*/
		EditItemFieldInsetGraphicDraw inset_graphic_draw; /*draw graphic*/
	}edit_field_inset;
}EditItemFieldSkin;

typedef struct _ui_edit_theme_bmp
{
	UIEditItemThemeCommon edit_theme_rect;
	EditItemHeaderSkin header_skin;
	EditItemFieldSkin field_skin;
}UIEditItemThemeBmp;

BOOL edit_item_theme_rect(UIEditItem *self, UIEditItemThemeDefault *theme_param);
BOOL edit_item_theme_bmp(UIEditItem *self, UIEditItemThemeBmp *theme_param);

int edit_item_field_inset_draw_graphic_triangle(UIEditItem *edit, void *param);
int edit_item_field_inset_draw_graphic_arrow(UIEditItem *edit, void *param);


//-------------------------------------------------------------------------------------------------
// edit action theme
//-------------------------------------------------------------------------------------------------
UIRect * calc_adapt_popup_rect(UIPage *page, UIEditItem *edit, UIRect *popu_rect);
int edit_item_popup_metal_customer_action(UIPage *page, UIEditItem *edit_item);


#ifdef __cplusplus
}
#endif

#endif // _EDIT_ITEM_H_
