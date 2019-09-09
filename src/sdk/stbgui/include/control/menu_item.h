/*
 * menu_item.h
 *
 *  Created on: 2018-11-05
 *      Author: Grace
 */
#ifndef _MENU_ITEM_H_
#define _MENU_ITEM_H_

#ifdef __cplusplus
extern "C"
{
#endif

typedef UIWindow UIMenuItem;
#define MENU_ITEM_CLASS_NAME "MenuItem"

#define MENU_ITEM_DEFAULT_WIDTH 300
#define MENU_ITEM_DEFAULT_HEIGHT 65


typedef int (*MenuItemActionFun)(void);


//private_data
typedef struct _ui_menu_item_data_
{
    /* property */
    U16 *text;
	const char *normal_icon;
	const char *light_icon ;
    MenuItemActionFun action;
    BOOL highlight;

    /* internal use */
	int text_size;        /* text content memory size */
} UIMenuItemData;


//interface
UIMenuItem* create_menu_item(const char *id);

int set_menu_item_text(UIMenuItem* self, const char *text);
int set_menu_item_normal_icon(UIMenuItem* self, const char *icon_name);
int set_menu_item_light_icon(UIMenuItem* self, const char *icon_name);

void set_menu_item_action(UIMenuItem* self, MenuItemActionFun menu_item_action);
void set_menu_item_highlight(UIMenuItem* self, BOOL value);


//-------------------------------------------------------------------------------------------------
// typical theme
//-------------------------------------------------------------------------------------------------
typedef struct _menu_item_theme_default_
{
	UIColor text_light_color;
	UIColor text_normal_color;
	int font_size;
	int icon_text_space;
} UIMenuItemThemeDefaultData;

BOOL menu_item_theme_default(UIMenuItem* self, UIMenuItemThemeDefaultData *theme_param);

#ifdef __cplusplus
}
#endif

#endif //END _MENU_ITEM_H_

