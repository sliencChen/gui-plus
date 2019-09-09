/*
 * menu.c
 *
 *  Created on: 2018-11-05
 *      Author: Grace
 */

#include <gui_window.h>
#include <control/menu.h>
#include <control/menu_item.h>

//-------------------------------------------------------------------------------------------------
// local variable
//-------------------------------------------------------------------------------------------------
typedef enum
{
    DIRECTION_UP = 0,
    DIRECTION_DOWN,
    DIRECTION_LEFT,
    DIRECTION_RIGHT,
    DIRECTION_MAX,
}DIRECTION_MODE;

//-------------------------------------------------------------------------------------------------
// internal use
//-------------------------------------------------------------------------------------------------
static UIMenuData* new_menu_private_data(void)
{
	UIMenuData *menu_data;

	menu_data = (UIMenuData *)gui_malloc(sizeof(UIMenuData));
	if (menu_data == NULL)
	{
		return NULL;
	}

	default_menu_data(menu_data);

	return menu_data;
}


static void delete_menu_private_data(UIMenuData *menu_data)
{
	if (menu_data)
	{
		gui_free(menu_data);
	}
}


static UIWindow* get_menu_item_by_id(UIWindow *self, char* id)
{
    int len = gui_strlen(id);
    if(!self || !self->control_list)
    {
        return NULL;
    }

    UIWindow *node = self->control_list;
    while(node)
    {
        int node_len = gui_strlen(node->id);
        if (gui_memcmp(node->id, id, node_len > len ? node_len:len) == 0)
        {
            printf("get menu id=%s\n", node->id);
            break;
        }

        node = node->next;
    }

    return node;
}


static UIWindow* get_highlight_menu_item(UIWindow *self)
{
    UIMenuItemData *menu_item_data;

    if(!self || !self->control_list)
    {
        return NULL;
    }

    UIWindow *node = self->control_list;
    while(node)
    {
        menu_item_data = (UIMenuItemData *)node->private_data;
        if (menu_item_data->highlight)
        {
            break;
        }

        node = node->next;
    }

    return node;
}

static UIWindow* get_next_highlight_menu_item(UIWindow *self, DIRECTION_MODE direction)
{
    int len1,len2;
    if(!self)
    {
        return NULL;
    }

    UIWindow *brother_node = self;
    UIWindow *first_node = self->parent->control_list;

    len1 = gui_strlen(self->id);
    if((direction == DIRECTION_UP) || (direction == DIRECTION_LEFT))
    {
        brother_node = brother_node->next;
        while(brother_node)
        {
            if(brother_node->visible)
            {
                printf("get next focus = %s\n", brother_node->id);
                return brother_node;
            }
            brother_node = brother_node->next;
        }

        brother_node = first_node;
        while(brother_node)
        {
            len2 = gui_strlen(brother_node->id);
            if(gui_memcmp(brother_node->id, self->id, len1>len2?len1:len2) == 0)
            {
                break;
            }

            if(brother_node->visible)
            {
                break;
            }
            brother_node = brother_node->next;
        }
    }
    else if((direction == DIRECTION_DOWN) || (direction == DIRECTION_RIGHT))
    {
        UIWindow *last_node = brother_node;
        brother_node = brother_node->prev;
        while(brother_node)
        {
            if(brother_node->visible)
            {
                printf("get next focus = %s\n", brother_node->id);
                return brother_node;
            }
            brother_node = brother_node->prev;
        }

        brother_node = last_node;
        while(brother_node)
        {
            last_node = brother_node;
            brother_node = brother_node->next;
        }

        brother_node = last_node;

        while(brother_node)
        {
            if(brother_node->visible)
            {
                break;
            }
            brother_node = brother_node->prev;
        }
    }

    printf("get next focus = %s\n", brother_node->id);
    return brother_node;
}

static int set_menu_item_focus(UIWindow *self, DIRECTION_MODE direction)
{
    UIWindow *current_sub_node = get_highlight_menu_item(self);
    UIWindow *next_sub_node = get_next_highlight_menu_item(current_sub_node, direction);

    if(!next_sub_node)
    {
        return -1;
    }

    int len1 = gui_strlen(current_sub_node->id);
    int len2 = gui_strlen(next_sub_node->id);

    if(gui_memcmp(current_sub_node->id, next_sub_node->id, len1>len2 ? len1:len2) != 0)
    {
        set_menu_item_highlight(current_sub_node, FALSE);
        set_menu_item_highlight(next_sub_node, TRUE);
        send_window_event(NULL, GUI_EVT_MENU_SLECTETION_CHANGED, (U32)next_sub_node->id);
    }

    return 0;
}


//-------------------------------------------------------------------------------------------------
// class event
//-------------------------------------------------------------------------------------------------
static int menu_on_up_key(UIWindow *self, Event event, EventData event_data)
{
	if (!self)
	{
		return -1;
	}

    set_menu_item_focus(self, DIRECTION_UP);

	return 0;
}

static int menu_on_down_key(UIWindow *self, Event event, EventData event_data)
{
	if (!self)
	{
		return -1;
	}

	set_menu_item_focus(self, DIRECTION_DOWN);

	return 0;
}

static int menu_on_left_key(UIWindow *self, Event event, EventData event_data)
{
	if (!self)
	{
		return -1;
	}

	set_menu_item_focus(self, DIRECTION_LEFT);

	return 0;
}

static int menu_on_right_key(UIWindow *self, Event event, EventData event_data)
{
	if (!self)
	{
		return -1;
	}

	set_menu_item_focus(self, DIRECTION_RIGHT);

	return 0;
}


BOOL register_vertical_menu_event(void)
{
	BOOL result = TRUE;

	result &= register_class_event_handler(MENU_CLASS_NAME, GUI_EVT_VK_UP, menu_on_up_key);
	result &= register_class_event_handler(MENU_CLASS_NAME, GUI_EVT_VK_DOWN, menu_on_down_key);

	return result;
}

static BOOL register_vertical_menu_class(void)
{
	static BOOL registered_vertical = FALSE;
	BOOL res;

	if (registered_vertical)
		return TRUE;

	res = register_vertical_menu_event();
	registered_vertical = TRUE;

	return res;
}


BOOL register_horizontal_menu_event(void)
{
	BOOL result = TRUE;

	result &= register_class_event_handler(MENU_CLASS_NAME, GUI_EVT_VK_LEFT, menu_on_left_key);
	result &= register_class_event_handler(MENU_CLASS_NAME, GUI_EVT_VK_RIGHT, menu_on_right_key);

	return result;
}

static BOOL register_horizontal_menu_class(void)
{
	static BOOL registered_horizontal = FALSE;
	BOOL res;

	if (registered_horizontal)
		return TRUE;

	res = register_horizontal_menu_event();
	registered_horizontal = TRUE;

	return res;
}


BOOL register_omnidirectional_menu_event(void)
{
	BOOL result = TRUE;

	result &= register_class_event_handler(MENU_CLASS_NAME, GUI_EVT_VK_UP, menu_on_up_key);
	result &= register_class_event_handler(MENU_CLASS_NAME, GUI_EVT_VK_DOWN, menu_on_down_key);
	result &= register_class_event_handler(MENU_CLASS_NAME, GUI_EVT_VK_LEFT, menu_on_left_key);
	result &= register_class_event_handler(MENU_CLASS_NAME, GUI_EVT_VK_RIGHT, menu_on_right_key);

	return result;
}

static BOOL register_omnidirectional_menu_class(void)
{
	static BOOL registered_omnidirectional = FALSE;
	BOOL res;

	if (registered_omnidirectional)
		return TRUE;

	res = register_omnidirectional_menu_event();
	registered_omnidirectional = TRUE;

	return res;
}


//-------------------------------------------------------------------------------------------------
// window manager callback
//-------------------------------------------------------------------------------------------------
static BOOL menu_on_draw(UIMenu *self)
{
    UIMenuThemeDefaultData default_menu;

	default_menu.border_color = 0xFF212225;//BLACK_COLOR;//0xFF9932CC;
	default_menu.inside_bkg_color = 0xFF212225;//BLACK_COLOR;//0xFF98F5FF;
	default_menu.border_size = 2;

	return menu_theme_default(self, &default_menu);
}

static BOOL menu_on_destroy(UIMenu *self)
{
	delete_menu_private_data(self->private_data);

	return TRUE;
}


//-------------------------------------------------------------------------------------------------
// interface
//-------------------------------------------------------------------------------------------------
void default_menu_data(UIMenuData *menu_data)
{
	/* parameter inherits from container */
	default_container_data(&(menu_data->container));

	/* special for menu */
}


UIMenu* create_vertical_menu(const char *id)
{
	UIMenu *menu;

	register_vertical_menu_class();
	menu = create_sub_window(id, 20, 10, MENU_DEFAULT_WIDTH, MENU_DEFAULT_HEIGHT);
	if (menu)
	{
        UIMenuData *menu_data = new_menu_private_data();
        if (menu_data == NULL)
		{
			destroy_window(menu);
			return NULL;
		}

		menu->private_data = menu_data;
		menu->focus_stop = TRUE;
		menu->class_name = MENU_CLASS_NAME;
		menu->on_draw = menu_on_draw;
		menu->on_destroy = menu_on_destroy;
	}

	return menu;
}


UIMenu* create_horizontal_menu(const char *id)
{
	UIMenu *menu;

	register_horizontal_menu_class();
	menu = create_sub_window(id, 0, 0, MENU_DEFAULT_WIDTH, MENU_DEFAULT_HEIGHT);
	if (menu)
	{
        UIMenuData *menu_data = new_menu_private_data();
        if (menu_data == NULL)
		{
			destroy_window(menu);
			return NULL;
		}

		menu->private_data = menu_data;
		menu->focus_stop = TRUE;
		menu->class_name = MENU_CLASS_NAME;
		menu->on_draw = menu_on_draw;
		menu->on_destroy = menu_on_destroy;
	}

	return menu;
}

UIMenu* create_omnidirectional_menu(const char *id)
{
	UIMenu *menu;

	register_omnidirectional_menu_class();
	menu = create_sub_window(id, 0, 0, MENU_DEFAULT_WIDTH, MENU_DEFAULT_HEIGHT);
	if (menu)
	{
        UIMenuData *menu_data = new_menu_private_data();
        if (menu_data == NULL)
		{
			destroy_window(menu);
			return NULL;
		}

		menu->private_data = menu_data;
		menu->focus_stop = TRUE;
		menu->class_name = MENU_CLASS_NAME;
		menu->on_draw = menu_on_draw;
		menu->on_destroy = menu_on_destroy;
	}

	return menu;
}


int set_menu_inset(UIMenu* menu, UIInset inset)
{
	if (!menu || !menu->private_data)
	{
		return -1;
	}

	set_container_inset(menu, inset);

    return 0;
}

int set_menu_sizer(UIMenu *menu, UISizer *sizer)
{
	if (!menu || !menu->private_data)
	{
		return -1;
	}

	set_container_sizer(menu, sizer);

    return 0;
}


int set_menu_item_visible(UIMenu* menu, char* menu_item_id, BOOL is_visible)
{
    if (!menu)
	{
		return -1;
	}

    UIWindow* menu_item = get_menu_item_by_id(menu, menu_item_id);
    if(!menu_item )
    {
        return -1;
    }

    if (menu_item->visible != is_visible)
    {
        set_window_visible(menu_item, is_visible);
        container_layout_sub_window(menu);
        invalidate_window(menu);
    }

    return 0;
}


//-------------------------------------------------------------------------------------------------
// add menu item
//-------------------------------------------------------------------------------------------------
BOOL menu_add_window(UIMenu* menu, UIWindow *sub_win)
{
	if ( !menu || !menu->private_data || !sub_win )
	{
		return FALSE;
	}

	//add sub to menu
    add_sub_window(menu, sub_win);

	return TRUE;
}


//-------------------------------------------------------------------------------------------------
// typical theme
//-------------------------------------------------------------------------------------------------
BOOL menu_theme_default(UIMenu* menu, UIMenuThemeDefaultData *theme_param)
{
	UIPainter *painter = &(menu->painter);

    draw_rect_frame_gui(painter, ui_rect(0, 0, menu->rect.size.width, menu->rect.size.height), \
                        theme_param->border_size, theme_param->border_color, theme_param->inside_bkg_color);

    return TRUE;
}


