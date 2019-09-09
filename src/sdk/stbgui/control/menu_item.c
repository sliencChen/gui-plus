/*
 * menu.c
 *
 *  Created on: 2018-11-05
 *      Author: Grace
 */

#include <gui_window.h>
#include <control/menu_item.h>


//-------------------------------------------------------------------------------------------------
// local variable
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
// internal use
//-------------------------------------------------------------------------------------------------
static UIMenuItemData* new_menu_item_private_data(void)
{
	UIMenuItemData *menu_item_data;

	menu_item_data = (UIMenuItemData *)gui_malloc(sizeof(UIMenuItemData));
	if (menu_item_data == NULL)
	{
		return NULL;
	}

	/* default parameter */
	gui_memset(menu_item_data, 0, sizeof(UIMenuItemData));

	return menu_item_data;
}

static void delete_menu_item_private_data(UIMenuItemData *menu_item_data)
{
	if (menu_item_data)
	{
	    if(menu_item_data->text)
        {
            gui_free(menu_item_data->text);
        }

		gui_free(menu_item_data);
	}
}

static int set_menu_item_text_internal(UIMenuItemData *menu_item_data, const char *text)
{
	int text_len = strlen(text);
	BOOL need_new_space = FALSE;

	if (menu_item_data->text == NULL || menu_item_data->text_size <= text_len)
	{
		need_new_space = TRUE;
	}

	if (need_new_space)
	{
		if (menu_item_data->text != NULL)
		{
			gui_free(menu_item_data->text);
			menu_item_data->text_size = 0;
		}
		menu_item_data->text = (U16 *)gui_malloc((text_len + 1)* sizeof(U16));
		if (menu_item_data->text == NULL)
		{
			return -1;
		}
		menu_item_data->text_size = text_len + 1;
	}

	utf8_to_ucs2(text, text_len, menu_item_data->text, text_len + 1);

	return 0;
}

//-------------------------------------------------------------------------------------------------
// class event
//-------------------------------------------------------------------------------------------------
static int menu_on_select_key(UIWindow *self, Event event, EventData event_data)
{
	UIMenuItemData *menu_item_data;

	if (!self || !self->private_data)
	{
		return -1;
	}
	menu_item_data = (UIMenuItemData *)self->private_data;

	if ((menu_item_data->highlight)&&(menu_item_data->action))
    {
        menu_item_data->action();
    }

	return 0;
}


BOOL register_menu_item_event(void)
{
	BOOL result = TRUE;

	result &= register_class_event_handler(MENU_ITEM_CLASS_NAME, GUI_EVT_VK_SELECT, menu_on_select_key);

	return result;
}

static BOOL register_menu_item_class(void)
{
	static BOOL registered = FALSE;
	BOOL res;

	if (registered)
		return TRUE;

	res = register_menu_item_event();
	registered = TRUE;

	return res;
}

//-------------------------------------------------------------------------------------------------
// window manager callback
//-------------------------------------------------------------------------------------------------
static BOOL menu_item_on_draw(UIMenuItem *self)
{
	UIMenuItemThemeDefaultData default_theme;

	default_theme.text_light_color = YELLOW_COLOR;
	default_theme.text_normal_color = WHITE_COLOR;
	default_theme.font_size = -1;
	default_theme.icon_text_space = 20;

	return menu_item_theme_default(self, &default_theme);
}

static BOOL menu_item_on_destroy(UIMenuItem *self)
{
	delete_menu_item_private_data(self->private_data);

	return TRUE;
}


//-------------------------------------------------------------------------------------------------
// interface
//-------------------------------------------------------------------------------------------------
UIMenuItem* create_menu_item(const char *id)
{
	UIWindow *menu_item = NULL;

	register_menu_item_class();
	menu_item = create_control(id, 0, 0, MENU_ITEM_DEFAULT_WIDTH, MENU_ITEM_DEFAULT_HEIGHT);
	if (menu_item != NULL)
	{
		UIMenuItemData *menu_item_data;

		menu_item_data = new_menu_item_private_data();
		if (menu_item_data == NULL)
		{
			gui_free(menu_item);
			return NULL;
		}

		menu_item->private_data = menu_item_data;
		menu_item->focus_stop = TRUE;
		menu_item->class_name = MENU_ITEM_CLASS_NAME;
		menu_item->on_draw = menu_item_on_draw;
		menu_item->on_destroy = menu_item_on_destroy;

		set_menu_item_action(menu_item, NULL);
	}

	return menu_item;
}

int set_menu_item_text(UIMenuItem* self, const char *text)
{
    UIMenuItemData *menu_item_data;

	if (!self || !self->private_data || !text)
	{
		return -1;
	}

	menu_item_data = (UIMenuItemData *)self->private_data;
	set_menu_item_text_internal(menu_item_data, text);

	invalidate_window(self);

	return 0;
}

int set_menu_item_normal_icon(UIMenuItem* self, const char *icon_name)
{
    UIMenuItemData *menu_item_data;

	if (!self || !self->private_data || !icon_name)
	{
		return -1;
	}
	menu_item_data = (UIMenuItemData *)self->private_data;

	if (menu_item_data->normal_icon == icon_name)
    {
        return -1;
    }
    menu_item_data->normal_icon = icon_name;

    invalidate_window(self);
    return 0;
}

int set_menu_item_light_icon(UIMenuItem* self, const char *icon_name)
{
    UIMenuItemData *menu_item_data;

	if (!self || !self->private_data || !icon_name)
	{
		return -1;
	}
	menu_item_data = (UIMenuItemData *)self->private_data;

	if (menu_item_data->light_icon == icon_name)
    {
        return -1;
    }
    menu_item_data->light_icon = icon_name;

    invalidate_window(self);

    return 0;
}

void set_menu_item_highlight(UIMenuItem* self, BOOL value)
{
    UIMenuItemData *menu_item_data;

	if (!self || !self->private_data)
	{
		return;
	}
	menu_item_data = (UIMenuItemData *)self->private_data;

	if(menu_item_data->highlight == value)
    {
        return;
    }

	menu_item_data->highlight = value;

    invalidate_window(self);
}


void set_menu_item_action(UIMenuItem* self, MenuItemActionFun menu_item_action)
{
    UIMenuItemData *menu_item_data;

    if (!self || !self->private_data || !menu_item_action)
    {
        return;
    }

    menu_item_data = (UIMenuItemData *)self->private_data;
    menu_item_data->action = menu_item_action;
}


//-------------------------------------------------------------------------------------------------
// typical theme - internal
//-------------------------------------------------------------------------------------------------
static int draw_menu_item_icon_internal(UIMenuItem* self, UIMenuItemThemeDefaultData *theme_param)
{
    UIMenuItemData *menu_item_data = self->private_data;
	UIPainter *painter = &(self->painter);

	DrawBmpParam param;
	UIRect icon_rect ;
	const char *icon_name = NULL;

    if (menu_item_data->highlight == TRUE)
    {
        icon_name = menu_item_data->light_icon;
    }
    else
    {
        icon_name = menu_item_data->normal_icon;
    }

    if (icon_name == NULL)
    {
        return 0;
    }

    if (!gui_get_bmp(icon_name,&(param.bmp_info)))
    {
        return 0;
    }

    icon_rect.point.x = 0;
    if( self->rect.size.height > param.bmp_info.pixel_buf.size.height)
    {
        icon_rect.point.y = (self->rect.size.height - param.bmp_info.pixel_buf.size.height)/2 ;
        icon_rect.size.height = param.bmp_info.pixel_buf.size.height;
    }
    else
    {
        icon_rect.point.y = 0;
        icon_rect.size.height = self->rect.size.height;
    }

    if (self->rect.size.width > param.bmp_info.pixel_buf.size.width + theme_param->icon_text_space)
    {
        icon_rect.size.width = param.bmp_info.pixel_buf.size.width;
    }
    else
    {
        icon_rect.size.width = self->rect.size.width - theme_param->icon_text_space;
    }

    param.mode = DRAW_BMP_MODE_NORMAL;
    draw_bitmap(painter, icon_rect, &param);

    return icon_rect.size.width;
}


//-------------------------------------------------------------------------------------------------
// typical theme
//-------------------------------------------------------------------------------------------------
BOOL menu_item_theme_default(UIMenuItem* self, UIMenuItemThemeDefaultData *theme_param)
{
    UIMenuItemData *menu_item_data = self->private_data;
	UIPainter *painter = &(self->painter);
	UIRect text_rect ;
	U16 icon_width = 0, text_height = 0;
	DrawTextParam param;

	icon_width = draw_menu_item_icon_internal(self, theme_param);

    default_draw_text_param(&param);

    if(menu_item_data->highlight)
    {
        param.color = theme_param->text_light_color;
    }
    else
    {
        param.color = theme_param->text_normal_color;
    }

	param.font_size = theme_param->font_size;

	text_height = get_text_height(param.font_size, -1);
	text_rect.point.x = icon_width + theme_param->icon_text_space;
    if( self->rect.size.height > text_height)
    {
        text_rect.point.y = (self->rect.size.height - text_height)/2 ;
        text_rect.size.height = text_height;
    }
    else
    {
        text_rect.point.y = 0;
        text_rect.size.height = self->rect.size.height;
    }

    text_rect.size.width = self->rect.size.width - text_rect.point.x;
    draw_ucs2_text(painter, text_rect, menu_item_data->text, -1, &param);

    return TRUE;
}



