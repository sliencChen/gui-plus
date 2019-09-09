/*
 * menu.h
 *
 *  Created on: 2018-11-05
 *      Author: Grace
 */
#ifndef _MENU_H_
#define _MENU_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <control/container.h>

typedef UIWindow UIMenu;
#define MENU_CLASS_NAME CONTAINER_CLASS_NAME "/Menu"

#define MENU_DEFAULT_WIDTH 300
#define MENU_DEFAULT_HEIGHT 420

//private_data
typedef struct _ui_menu_data_
{
	/* inherits from container */
	UIContainerData container;

} UIMenuData;

void default_menu_data(UIMenuData *menu_data);


//-------------------------------------------------------------------------------------------------
// interface
//-------------------------------------------------------------------------------------------------
UIMenu* create_vertical_menu(const char *id);
UIMenu* create_horizontal_menu(const char *id);
UIMenu* create_omnidirectional_menu(const char *id);


int set_menu_item_visible(UIMenu* self, char* menu_item_id, BOOL is_visible);
int set_menu_sizer(UIMenu *menu, UISizer *sizer);
int set_menu_inset(UIMenu* menu, UIInset inset);

BOOL menu_add_window(UIMenu* menu, UIWindow *sub_win);


//-------------------------------------------------------------------------------------------------
// typical theme
//-------------------------------------------------------------------------------------------------
typedef struct _menu_theme_default_
{
	UIColor border_color;
	UIColor inside_bkg_color;
	int border_size;
} UIMenuThemeDefaultData;

BOOL menu_theme_default(UIMenu* self, UIMenuThemeDefaultData *theme_param);

#ifdef __cplusplus
}
#endif

#endif //END _MENU_H_

