/*
 * theme.c
 *
 *  Created on: 2019-6-25
 *      Author: Brant
 */

#include <gui_window.h>
#include <theme/theme.h>


static UITheme *gui_themes = NULL;
static char preferred_sub_theme[40] = DEFAULT_SUB_THEME;


static BOOL is_same_theme(UITheme *theme1, UITheme *theme2)
{
	if (gui_strcmp(theme1->class_name, theme2->class_name) != 0)
		return FALSE;

	if (gui_strcmp(theme1->theme_name, theme2->theme_name) != 0)
		return FALSE;

	if (gui_strcmp(theme1->sub_name, theme2->sub_name) != 0)
		return FALSE;

	return TRUE;
}

static int add_new_theme(UITheme *theme)
{
	UITheme *node, *last;

	if (gui_themes == NULL)
	{
		gui_themes = theme;
		return 0;
	}

	last = node = gui_themes;
	while (node)
	{
		if (is_same_theme(node, theme))
		{
			return -1;
		}
		last = node;
		node = node->next;
	}
	last->next = theme;

	return 0;
}

int gui_register_theme(const char *class_name, const char *theme, const char *sub_name, BOOL (*on_draw_cb)(UIWindow *window))
{
	UITheme *new_theme;

	if (class_name == NULL || gui_strlen(class_name) <= 0 || theme == NULL || gui_strlen(theme) <= 0)
		return -1;

	new_theme = (UITheme *)gui_malloc(sizeof(UITheme));
	if (new_theme == NULL)
		return -2;

	if (sub_name == NULL || gui_strlen(sub_name) <= 0)
		sub_name = DEFAULT_SUB_THEME;

	gui_memset(new_theme, 0, sizeof(UITheme));
	new_theme->class_name = class_name;
	new_theme->theme_name = theme;
	new_theme->sub_name = sub_name;
	new_theme->theme_on_draw = on_draw_cb;

	if (add_new_theme(new_theme) != 0)
	{
		gui_free(new_theme);
		return -3;
	}
	return 0;
}

void gui_register_themes(UITheme *themes, int num)
{
	gui_themes = themes;
}

void gui_prefer_sub_theme(const char *sub_theme_name)
{
	gui_snprintf(preferred_sub_theme, sizeof(preferred_sub_theme), "%s", sub_theme_name);
}


static UITheme* find_theme(const char *class_name, const char *theme, const char *sub_theme)
{
	UITheme *node = gui_themes;
	UITheme find;

	if (class_name == NULL || theme == NULL || sub_theme == NULL)
		return NULL;

	find.class_name = class_name;
	find.theme_name = theme;
	find.sub_name = sub_theme;
	while (node)
	{
		if (is_same_theme(node, &find))
			return node;

		node = node->next;
	}
	return NULL;
}

BOOL window_use_theme_ex(UIWindow *win, const char *theme, const char *sub_theme)
{
	UITheme *theme_node = find_theme(win->class_name, theme, sub_theme);

	if (!theme_node)
		return FALSE;

	win->on_draw = theme_node->theme_on_draw;

	return TRUE;
}

BOOL window_use_theme(UIWindow *win, const char *theme)
{
	BOOL res;

	res = window_use_theme_ex(win, theme, preferred_sub_theme);
	if (!res && gui_strcmp(preferred_sub_theme, DEFAULT_SUB_THEME) != 0)
		res = window_use_theme_ex(win, theme, DEFAULT_SUB_THEME);

	return res;
}
