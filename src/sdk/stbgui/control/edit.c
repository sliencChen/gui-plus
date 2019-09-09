/*
 * edit.c
 *
 *  Created on: 2019-7-23
 *      Author: Silence
 */

#include <gui_window.h>
#include <control/edit.h>
#include <control/edit_item.h>


//-------------------------------------------------------------------------------------------------
// local variable
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
// internal use
//-------------------------------------------------------------------------------------------------
static UIEditData* new_edit_private_data(void)
{
	UIEditData *edit_data;

	edit_data = (UIEditData *)gui_malloc(sizeof(UIEditData));
	if (edit_data == NULL)
	{
		return NULL;
	}
	edit_data->list_sizer = create_list_sizer(UI_SIZER_DIR_VER);
	if (edit_data->list_sizer == NULL)
	{
		return NULL;
	}
	default_edit_data(edit_data);
	return edit_data;
}


static void delete_edit_private_data(UIEditData *edit_data)
{
	if (edit_data)
	{
		delete_container_data(&(edit_data->container));
		gui_free(edit_data);
	}
}


//-------------------------------------------------------------------------------------------------
// class event
//-------------------------------------------------------------------------------------------------
static UIWindow* get_high_light_edit_item(UIEdit *edit)
{
	UIEditItem *edit_item;
	UIEditItemData *edit_item_data;
	if (!edit || !edit->control_list)
	{
		return NULL;
	}
	edit_item = edit->control_list;
	while(edit_item)
	{
		edit_item_data = edit_item->private_data;
		if (edit_item_data == NULL)
		{
			return NULL;
		}
		if (edit_item_data->high_light)
		{
			return edit_item;
		}
		edit_item = edit_item->next;
	}

	return edit_item;
}


static UIWindow* get_next_edit_item(UIEdit *edit)
{
	UIEditItem *high_light_edit_item = get_high_light_edit_item(edit);
	UIEditData *edit_data= edit->private_data;
    if (!high_light_edit_item)
    {
        return NULL;
    }
	int total_item = get_item_count(edit_data->list_sizer);
    UIWindow *prev_node;
    UIWindow *current_node = high_light_edit_item;
    UIWindow *last_node = current_node->next;

    /*get the last node*/
    if (last_node == NULL)
    {
    	last_node = current_node;
    }
    else
    {
        while(last_node)
        {
        	if (last_node->next == NULL)
        	{
        		break;
        	}
        	last_node = last_node->next;
        }
    }

	prev_node = current_node->prev;
	edit_data->select++;
	if (NULL == prev_node && edit_data->select > total_item - 1)
	{
		edit_data->select = 0;
		prev_node = last_node;
	}

	set_edit_item_high_light(high_light_edit_item, FALSE);
	set_edit_item_high_light(prev_node, TRUE);

	return prev_node;
}


static UIWindow* get_prev_edit_item(UIEdit *edit)
{
	UIEditItem *high_light_edit_item = get_high_light_edit_item(edit);
	UIEditData *edit_data = edit->private_data;
    if (!high_light_edit_item)
    {
        return NULL;
    }

    UIWindow *next_node;
    UIWindow *current_node = high_light_edit_item;
    UIWindow *first_node = edit->control_list;

    next_node = current_node->next;
    edit_data->select--;
    if (NULL == next_node && edit_data->select < 0)
    {
    	next_node = first_node;
    	edit_data->select = get_item_count(edit_data->list_sizer) - 1;
    }

	set_edit_item_high_light(high_light_edit_item, FALSE);
	set_edit_item_high_light(next_node, TRUE);

    return next_node;
}


static void update_edit_page_if_needed(UIEdit *edit)
{
	UIEditData *edit_data = edit->private_data;
	UIListSizer *sizer = edit_data->list_sizer;
	UISizerItemNode *node;
	int index;
	int total_item = get_item_count(edit_data->list_sizer);
	int total_page = total_item/edit_data->page_max_item ;
	BOOL update_page_needed = FALSE;
	if (total_item > edit_data->page_max_item)
	{
		if (edit_data->select/edit_data->page_max_item != edit_data->current_page)
		{
			edit_data->current_page = edit_data->select/edit_data->page_max_item;
			update_page_needed = TRUE;
		}
	}

	printf("\nedit_data->current_page = %d",edit_data->current_page);
	printf("\ntotal_page = %d\n",total_page);
	if (update_page_needed)
	{
		/*invisible previous page*/
		for (index = 0, node = sizer->items; index < total_item && node != NULL; index++, node = node->next)
		{
			if (index/edit_data->page_max_item != edit_data->current_page)
			{
				set_window_visible(node->item.window, FALSE);
				printf("\n node->item.window->id = %s,node->item.window->visible = %d\n",node->item.window->id,node->item.window->visible);
			}
		}

		/*visible next page*/
		for (index = 0, node = sizer->items; index < total_item && node != NULL; index++, node = node->next)
		{
			if (index/edit_data->page_max_item == edit_data->current_page)
			{
				set_window_visible(node->item.window, TRUE);
				printf("\n node->item.window->id = %s,node->item.window->visible = %d\n",node->item.window->id,node->item.window->visible);
			}
		}

		container_layout_sub_window(edit);
		adjust_container_size(edit);
	}
}


static int edit_on_down_key(struct _ui_window_ *window, Event event, EventData event_data)
{
	get_next_edit_item(window);
	update_edit_page_if_needed(window);
	return 0;
}


static int edit_on_up_key(struct _ui_window_ *window, Event event, EventData event_data)
{
	get_prev_edit_item(window);
	update_edit_page_if_needed(window);
	return 0;
}


static int edit_on_select_key(struct _ui_window_ *window, Event event, EventData event_data)
{
	UIEditItem *hight_light_edit_item = get_high_light_edit_item(window);
	send_window_event(hight_light_edit_item, GUI_EVT_VK_SELECT, event_data);
	return 0;
}


static BOOL register_edit_event(void)
{
	BOOL result = TRUE;

	result &= register_class_event_handler(EDIT_CLASS_NAME, GUI_EVT_VK_SELECT, edit_on_select_key);
	result &= register_class_event_handler(EDIT_CLASS_NAME, GUI_EVT_VK_DOWN, edit_on_down_key);
	result &= register_class_event_handler(EDIT_CLASS_NAME, GUI_EVT_VK_UP, edit_on_up_key);

	return result;
}


static BOOL register_edit_class(void)
{
	static BOOL registered= FALSE;
	BOOL res;

	if (registered)
		return TRUE;

	res = register_edit_event();
	registered = TRUE;

	return res;
}


//-------------------------------------------------------------------------------------------------
// window manager callback
//-------------------------------------------------------------------------------------------------
static BOOL edit_on_draw(UIEdit *self)
{
    UIEditThemeDefaultData default_edit;
    default_edit.border_color = BLACK_COLOR;//0xFF9932CC;
    default_edit.inside_bkg_color = BLACK_COLOR;//0xFF98F5FF;
    default_edit.border_size = 2;

	return menu_theme_default(self, &default_edit);
}


static BOOL edit_on_destroy(UIEdit *self)
{
	delete_edit_private_data(self->private_data);

	return TRUE;
}


//-------------------------------------------------------------------------------------------------
// interface
//-------------------------------------------------------------------------------------------------
void default_edit_data(UIEditData *edit_data)
{
	default_container_data(&(edit_data->container));
	edit_data->current_page = 0;
	edit_data->select = 0;
	edit_data->page_max_item = 5;
}


UIEdit* create_edit(const char *id)
{
	UIEdit *edit;
	register_edit_class();
	edit = create_sub_window(id, 0, 0, EDIT_DEFAULT_WIDTH, EDIT_DEFAULT_HEIGHT);
	if (edit)
	{
		UIEditData *edit_data = new_edit_private_data();
        if (edit_data == NULL)
		{
			destroy_window(edit);
			return NULL;
		}
        edit->private_data = edit_data;
        edit->focus_stop = TRUE;
        edit->class_name = EDIT_CLASS_NAME;
        edit->on_draw = edit_on_draw;
        edit->on_destroy = edit_on_destroy;
	}

	return edit;
}


int set_edit_inset(UIEdit* edit, UIInset inset)
{
	if (!edit || !edit->private_data)
	{
		return -1;
	}

	set_container_inset(edit, inset);

    return 0;
}


int set_edit_max_item(UIEdit* edit, int max_item)
{
	if (!edit || !edit->private_data)
	{
		return -1;
	}
	UIEditData *edit_data = edit->private_data;
	edit_data->page_max_item = max_item;

	return 0;
}


int set_edit_sizer(UIEdit *edit)
{
	if (!edit || !edit->private_data)
	{
		return -1;
	}
	UIEditData *edit_data = edit->private_data;
	int index;
	UISizerItemNode *node;
	int total_item = get_item_count(edit_data->list_sizer);
	for (index = 0, node = edit_data->list_sizer->items; index < total_item && node != NULL; index++, node = node->next)
	{
		if (index/edit_data->page_max_item != 0)  /*only the first page visible*/
		{
			set_window_visible(node->item.window, FALSE);
		}
	}
	set_container_sizer(edit, edit_data->list_sizer);

    return 0;
}


BOOL edit_add_window(UIEdit *edit, UIWindow *sub_win)
{
	if ( !edit || !edit->private_data || !sub_win )
	{
		return FALSE;
	}

	//add sub to menu
    add_sub_window(edit, sub_win);

	return TRUE;
}


