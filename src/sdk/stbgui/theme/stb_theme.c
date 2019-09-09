/*
 * theme.c
 *
 *  Created on: 2019-6-25
 *      Author: Brant
 */

#include <gui_window.h>
#include <theme/stb_theme.h>


//-------------------------------------------------------------------------------------------------
// macro define
//-------------------------------------------------------------------------------------------------
#define MAX_CONTROL_TYPE		30


//-------------------------------------------------------------------------------------------------
// local variable
//-------------------------------------------------------------------------------------------------
static UIControlStyle gui_all_control_styles[MAX_CONTROL_TYPE];
static int gui_control_style_num = 0;
static char gui_preferred_theme_name[40] = DEFAULT_THEME_NAME;


//-------------------------------------------------------------------------------------------------
// internal use
//-------------------------------------------------------------------------------------------------
static UIControlStyle* find_control_style_list(const char *class_name)
{
	int i;

	for (i = 0; i < gui_control_style_num; i++)
		if (gui_strcmp(gui_all_control_styles[i].class_name, class_name) == 0)
			return gui_all_control_styles + i;

	return NULL;
}

static UIControlStyle* insert_control_style(const char *class_name)
{
	int insert_index;

	if (gui_control_style_num >= MAX_CONTROL_TYPE)
		return NULL;

	insert_index = gui_control_style_num;
	gui_all_control_styles[insert_index].class_name = class_name;
	gui_control_style_num++;

	return gui_all_control_styles + insert_index;
}

static UIStyle* find_style_in_control(UIControlStyle *control_style, const char *style_name)
{
	UIStyle *node = control_style->styles;

	while (node)
	{
		if (gui_strcmp(node->name, style_name) == 0)
			return node;

		node = node->next;
	}
	return NULL;
}

static int add_theme_into_style(UIStyle *style, UITheme *theme)
{
	UITheme *node, *tail;

	if (style->themes == NULL)
	{
		style->themes = theme;
		return 0;
	}

	tail = node = style->themes;
	while (node != NULL)
	{
		if (gui_strcmp(node->name, theme->name) == 0)	/* already exists */
		{
			return -1;
		}
		tail = node;
		node = node->next;
	}
	tail->next = theme;

	return 0;
}

static void add_style_into_control(UIControlStyle *control_style, UIStyle *style)
{
	UIStyle *node;

	if (control_style->styles == NULL)
	{
		control_style->styles = style;
		return;
	}

	node = control_style->styles;
	while (node->next != NULL)
	{
		node = node->next;
	}
	node->next = style;
}

static int add_theme_into_control(UIControlStyle *control_style, const char *style_name, UITheme *theme)
{
	UIStyle *style;

	style = find_style_in_control(control_style, style_name);
	if (style == NULL)
	{
		style = (UIStyle *)gui_malloc(sizeof(UIStyle));
		if (style == NULL)
			return -1;

		gui_memset(style, 0, sizeof(UIStyle));
		style->name = style_name;

		add_style_into_control(control_style, style);
	}

	return add_theme_into_style(style, theme);
}

/*
 * get default theme if there no specific theme.
 */
static UITheme* find_theme(const char *class_name, const char *style, const char *theme)
{
	UIControlStyle *style_list;
	UIStyle *style_node;
	UITheme *node, *default_theme = NULL;

	style_list = find_control_style_list(class_name);
	if (style_list == NULL)
		return NULL;

	style_node = find_style_in_control(style_list, style);
	if (style_node == NULL)
		return NULL;

	node = style_node->themes;
	while (node)
	{
		if (gui_strcmp(node->name, theme) == 0)
		{
			return node;
		}
		else if (gui_strcmp(node->name, DEFAULT_THEME_NAME) == 0)
		{
			default_theme = node;
		}
		node = node->next;
	}

	/* not find theme, use default */
	return default_theme;
}

static BOOL window_set_theme(UIWindow *window, UITheme *theme)
{
	void *theme_data;

	if (theme->data.size > 0)
	{
		if (theme->data.init == NULL)	/* if theme has data, must also has a initialize func */
			return FALSE;

		theme_data = gui_malloc(theme->data.size);
		if (theme_data == NULL)
			return FALSE;

		if (window->theme_data)
			gui_free(window->theme_data);

		theme->data.init(theme_data);
		window->theme_data = theme_data;
	}

	window->on_draw = theme->data.on_draw;
	if (theme->data.did_set)
		theme->data.did_set(window);

	return TRUE;
}


//-------------------------------------------------------------------------------------------------
// interface
//-------------------------------------------------------------------------------------------------
int gui_register_theme(const char *class_name, const char *style, const char *theme, int data_size, \
		void (*init_func)(void*), BOOL (*on_draw_func)(UIWindow*), void (*did_set_func)(UIWindow*))
{
	UIControlStyle *control_style_list;
	UITheme *theme_node;

	if (class_name == NULL || style == NULL)
		return -1;
	if ((data_size > 0 && init_func == NULL) || on_draw_func == NULL)
		return -1;

	if (theme == NULL || gui_strlen(theme) <= 0)
		theme = DEFAULT_THEME_NAME;

	control_style_list = find_control_style_list(class_name);
	if (control_style_list == NULL)
		control_style_list = insert_control_style(class_name);

	if (control_style_list == NULL)
		return -2;

	theme_node = (UITheme *)gui_malloc(sizeof(UITheme));
	if (theme_node == NULL)
		return -3;

	gui_memset(theme_node, 0, sizeof(UITheme));
	theme_node->name = theme;
	theme_node->data.size = data_size;
	theme_node->data.init = init_func;
	theme_node->data.on_draw = on_draw_func;
	theme_node->data.did_set = did_set_func;

	return add_theme_into_control(control_style_list, style, theme_node);
}

void gui_prefer_theme(const char *sub_theme_name)
{
	gui_snprintf(gui_preferred_theme_name, sizeof(gui_preferred_theme_name), "%s", sub_theme_name);
}

BOOL window_use_style_theme(UIWindow *win, const char *style, const char *theme)
{
	UITheme *theme_node;

	if (win == NULL || style == NULL)
		return FALSE;

	if (theme == NULL || gui_strlen(theme) <= 0)
		theme = DEFAULT_THEME_NAME;

	theme_node = find_theme(win->class_name, style, theme);
	if (!theme_node)
		return FALSE;

	return window_set_theme(win, theme_node);
}

BOOL window_use_style(UIWindow *win, const char *style)
{
	return window_use_style_theme(win, style, gui_preferred_theme_name);
}
