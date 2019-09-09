/*
 * theme.h
 *
 *  Created on: 2019-6-25
 *      Author: Brant
 */

#ifndef GUI_THEME_H_
#define GUI_THEME_H_

#ifdef __cplusplus
extern "C"
{
#endif


#define DEFAULT_THEME_NAME		"Default"


typedef struct _ui_theme_data_
{
	U32 size;
	void (*init)(void *theme_data);
	BOOL (*on_draw)(UIWindow *window);
	void (*did_set)(UIWindow *window);
} UIThemeData;

typedef struct _ui_theme_
{
	const char *name;
	UIThemeData data;
	struct _ui_theme_ *next;
} UITheme;

typedef struct _ui_style_
{
	const char *name;
	UITheme *themes;
	struct _ui_style_ *next;
} UIStyle;

typedef struct _ui_control_styles_
{
	const char *class_name;
	UIStyle *styles;
} UIControlStyle;


/*
 * register a theme
 */
int gui_register_theme(const char *class_name, const char *style, const char *theme, int data_size, \
		void (*init_func)(void*), BOOL (*on_draw_func)(UIWindow*), void (*did_set_func)(UIWindow*));

/*
 * regiester all themes for gui system
 */
void gui_register_themes(UITheme *themes, int num);

/*
 * always use such theme if it exists(for customize), otherwise use default theme
 */
void gui_prefer_theme(const char *theme_name);

/*
 * use theme match name and sub theme.
 */
BOOL window_use_style_theme(UIWindow *win, const char *style, const char *theme);

/*
 * always use preferred sub theme, use default if no preferred sub theme.
 */
BOOL window_use_style(UIWindow *win, const char *style);


#ifdef __cplusplus
}
#endif

#endif /* GUI_THEME_H_ */
