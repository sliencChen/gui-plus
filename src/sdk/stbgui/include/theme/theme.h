/*
 * theme.h
 *
 *  Created on: 2019-6-25
 *      Author: Brant
 */

#ifndef GUI_THEME_H_
#define GUI_THEME_H_


#define DEFAULT_SUB_THEME		"Default"


typedef struct _ui_theme_
{
	const char *class_name;
	const char *theme_name;
	const char *sub_name;
	BOOL (*theme_on_draw)(UIWindow *window);

	struct _ui_theme_ *next;
} UITheme;


/*
 * register a theme
 */
int gui_register_theme(const char *class_name, const char *theme, const char *sub_name, BOOL (*on_draw_cb)(UIWindow *window));

/*
 * regiester all themes for gui system
 */
void gui_register_themes(UITheme *themes, int num);

/*
 * if have such sub theme, always use sub theme(for customize), otherwise use default theme
 */
void gui_prefer_sub_theme(const char *sub_theme_name);

/*
 * use theme match name and sub theme.
 */
BOOL window_use_theme_ex(UIWindow *win, const char *theme, const char *sub_theme);

/*
 * always use preferred sub theme, use default if no preferred sub theme.
 */
BOOL window_use_theme(UIWindow *win, const char *theme);


#endif /* GUI_THEME_H_ */
