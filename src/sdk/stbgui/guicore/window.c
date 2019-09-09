/*
 * window.c
 *
 *  Created on: 2015-4-25
 *      Author: Brant
 */

#include <gui_common.h>
#include <gdi/graphic.h>
#include <guicore/event.h>
#include <guicore/class.h>
#include <guicore/window.h>
#include <guicore/winbase.h>
#include <sizer/sizer.h>
#include <control/container.h>


//-------------------------------------------------------------------------------------------------
// local variable
//-------------------------------------------------------------------------------------------------
static UIWindow *global_focus_window = NULL;


//-------------------------------------------------------------------------------------------------
// default window handler
//-------------------------------------------------------------------------------------------------
static BOOL window_on_erase_bkg(UIWindow *self)
{
	UIPainter *painter = &(self->painter);
	UIRect rect = ui_rect(0, 0, self->rect.size.width, self->rect.size.height);
	int draw_mode = painter->mode;;

	if (self->bkg_color == TRANS_COLOR)
	{
		return TRUE;
	}
	if (is_trans_color(self->painter.canvas->color_mode, self->bkg_color))
	{
		painter->mode = UI_DRAW_MODE_MIX;
	}
	if (!is_trans_color(self->painter.canvas->color_mode, self->bkg_color) || self->painter.canvas->color_mode == COLOR_MODE_ARGB8888)
	{
		draw_rect(painter, rect, self->bkg_color);
	}
	painter->mode = draw_mode;

	return TRUE;
}

static int window_on_destroy(UIWindow *self, Event event, EventData event_data)
{
	destroy_window(self);

	return 0;
}


//-------------------------------------------------------------------------------------------------
// create window
//-------------------------------------------------------------------------------------------------
static UIWindow* create_window(const char *id, int x, int y, int width, int height)
{
	UIWindow *win = NULL;

	win = (UIWindow *)gui_malloc(sizeof(UIWindow));
	if (!win)
	{
		return NULL;
	}

	/* init window */
	gui_memset(win, 0, sizeof(UIWindow));

	strncpy(win->id, id, GUI_WINDOW_ID_SIZE - 1);
	win->id[GUI_WINDOW_ID_SIZE - 1] = '\0';
	win->origin = ui_point(x, y);
	win->rect = ui_rect(x, y, width, height);
	win->min_size = ui_size(1, 1);
	win->visible = TRUE;
	win->clip_rgn_change = TRUE;
	win->bkg_color = TRANS_COLOR;
	win->has_trans = TRUE;

	win_init_region(win);
	win->on_erase_bkg = window_on_erase_bkg;

	/* default event handler */
	window_connect_handler(win, GUI_EVT_DESTROY, window_on_destroy);

	return win;
}


UIWindow* create_main_window(const char *id, int x, int y, int width, int height)
{
	return create_window(id, x, y, width, height);
}

UIWindow* create_sub_window(const char *id, int x, int y, int width, int height)
{
	UIWindow *win;

	win = create_window(id, x, y, width, height);
	if (win)
	{
		win->focus_stop = TRUE;
		win->type = WINDOW_TYPE_SUB;
		win->bkg_color = TRANS_COLOR;
	}
	return win;
}

UIWindow* create_control(const char *id, int x, int y, int width, int height)
{
	UIWindow *win;

	win = create_window(id, x, y, width, height);
	if (win)
	{
		win->type = WINDOW_TYPE_CONTROL;
		win->bkg_color = TRANS_COLOR;
		win->has_trans = TRUE;
	}
	return win;
}


//-------------------------------------------------------------------------------------------------
// destroy window
//-------------------------------------------------------------------------------------------------
static void do_destroy_window(UIWindow *window)
{
	/* destroy private data */
	if (window->on_destroy)
		window->on_destroy(window);

	/* destroy theme data */
	if (window->theme_data)
		gui_free(window->theme_data);

	/* destroy region */
	empty_clip_region(&(window->clip_region));
	empty_clip_region(&(window->painter.clip_region));
	empty_invalid_region(&(window->inv_region));
	delete_all_event_handler(window->handler_list);

	gui_free(window);
}

static BOOL destroy_main_window(UIWindow *window)
{
	UIWindow *child, *control, *next;

	child = window->sub_list;
	while (child)
	{
		control = child->control_list;
		while (control)
		{
			next = control->next;
			destroy_window(control);	/* destroy will cause next pointer incorrect */
			control = next;
		}
		next = child->next;
		destroy_window(child);
		child = next;
	}
	control = window->control_list;
	while (control)
	{
		next = control->next;
		destroy_window(control);
		control = next;
	}
	do_destroy_window(window);

	return TRUE;
}

static BOOL destroy_sub_window(UIWindow *win)
{
	UIWindow *control, *next;

	control = win->control_list;
	while (control)
	{
		next = control->next;
		destroy_window(control);
		control = next;
	}
	detach_window(win);
	do_destroy_window(win);

	return TRUE;
}

static BOOL destroy_control_window(UIWindow *win)
{
	detach_window(win);
	do_destroy_window(win);

	return TRUE;
}

BOOL destroy_window(UIWindow *window)
{
	BOOL res = FALSE;

	if (window == NULL)
		return FALSE;

	switch (window->type)
	{
		case WINDOW_TYPE_MAIN:
			res = destroy_main_window(window);
			break;
		case WINDOW_TYPE_SUB:
			res = destroy_sub_window(window);
			break;
		case WINDOW_TYPE_CONTROL:
			res = destroy_control_window(window);
			break;
		default:
			break;
	}
	return res;
}


//-------------------------------------------------------------------------------------------------
// focus window
//-------------------------------------------------------------------------------------------------
static int get_max_focus_index(UIWindow *win)
{
	int index = -1;

	if (win == NULL) return -1;

	while (win)
	{
		index = MAX(index, win->focus_index);
		win = win->next;
	}

	return index;
}

static BOOL win_resign_focus_act(UIWindow *win)
{
	if (win == NULL) return FALSE;

	win->focus = FALSE;
	if (1/* have_focus_style(win) */)
	{
		invalidate_window(win);
	}

	return TRUE;
}

static BOOL set_global_focus_window(UIWindow *win)
{
	/* same, do nothing */
	if (global_focus_window == win)
	{
		return TRUE;
	}

	win_resign_focus_act(global_focus_window);
	win->focus = TRUE;
	global_focus_window = win;
	if (1/* have_focus_style(win) */)
	{
		invalidate_window(win);
	}

	return TRUE;
}

static UIWindow* get_global_focus_window(void)
{
	return global_focus_window;
}

/*
 * focus_index < 0: find first focus window(focus index nearest 0)
 * focus_index >= 0: find next focus window(focus index nearest focus_index)
 *
 * if not find next focus window(maybe reach end or only 1 node), use first focus window.
 * return NULL: no window can stop focus
 */
static UIWindow* focus_index_near_with(UIWindow *head, int focus_index)
{
	UIWindow *node = head;
	UIWindow *next_focus_window = NULL;	/* greate than focus_index but nearest */
	int next_focus_index = 0xFFFF;
	UIWindow *first_focus_window = NULL;	/* fist focus window */
	int min_focus_index = 0xFFFF;

	while (node)
	{
		if (win_is_drawable(node) && win_is_focusable(node))
		{
			if (node->focus_index < min_focus_index)
			{
				first_focus_window = node;
				min_focus_index = node->focus_index;
				if (focus_index < 0 && node->focus_index == 0)
				{
					return first_focus_window;
				}
			}
			if (focus_index >= 0 && node->focus_index > focus_index && node->focus_index < next_focus_index)
			{
				next_focus_window = node;
				next_focus_index = node->focus_index;
				if (next_focus_index == focus_index + 1)
				{
					break;
				}
			}
		}
		node = node->next;
	}

	return next_focus_window ? next_focus_window : first_focus_window;
}

static BOOL win_resign_focus(UIWindow *win)
{
	UIWindow *focus;
	UIWindow *brothers = NULL;

	/* can not resign main window focus state */
	if (win_is_main(win))
	{
		return FALSE;
	}
	else if (win_is_sub(win) && win->parent != NULL)
	{
		brothers = win->parent->sub_list;
	}
	else if (win_is_control(win) && win->parent != NULL)
	{
		brothers = win->parent->control_list;
	}

	if (brothers == NULL)
	{
		return FALSE;
	}
	focus = focus_index_near_with(brothers, win->focus_index);
	if (focus)
	{
		set_global_focus_window(focus);
	}
	else
	{
		set_global_focus_window(win->parent);
	}

	return TRUE;
}

static BOOL win_set_focus_control(UIWindow *win)
{
	return set_global_focus_window(win);
}

static BOOL win_set_focus_sub(UIWindow *win)
{
	UIWindow *focus;

	/* check which control can accept focus */
	focus = focus_index_near_with(win->control_list, -1);
	if (focus)
	{
		return win_set_focus_control(focus);
	}

	/* focus to sub window itself */
	set_global_focus_window(win);

	return TRUE;

}

static BOOL win_set_focus_main(UIWindow *win)
{
	UIWindow *focus;

	/* check which sub window can accept focus */
	focus = focus_index_near_with(win->sub_list, -1);
	if (focus)
	{
		return win_set_focus_sub(focus);
	}
	/* if no sub window can accept focus, check controls */
	focus = focus_index_near_with(win->control_list, -1);
	if (focus)
	{
		return win_set_focus_control(focus);
	}

	/* focus to main window itself */
	set_global_focus_window(win);

	return TRUE;
}

BOOL set_window_focus(UIWindow *window)
{
	BOOL res = FALSE;

	if (!win_is_drawable(window) || !win_is_focusable(window))
		return FALSE;

	switch (window->type)
	{
		case WINDOW_TYPE_MAIN:
			res = win_set_focus_main(window);
			break;
		case WINDOW_TYPE_SUB:
			res = win_set_focus_sub(window);
			break;
		case WINDOW_TYPE_CONTROL:
		default:
			res = win_set_focus_control(window);
			break;
	}
	return res;
}


//-------------------------------------------------------------------------------------------------
// window size
//-------------------------------------------------------------------------------------------------
static UIPoint client_to_parent(UIWindow *window, UIPoint point)
{
	UIInset inset;

	if (window->parent == NULL)
		return point;

	inset = get_window_inset(window->parent);
	point = offset_point(point, window->parent->origin.x, window->parent->origin.y);
	point = offset_point(point, inset.left, inset.top);

	return  point;
}

UIPoint client_to_screen(UIWindow *window, UIPoint point)
{
	UIPoint result = point;

	while (window != NULL)
	{
        result = client_to_parent(window, result);
		window = window->parent;
	}

	return result;
}

UIPoint screen_to_client(UIWindow *window, UIPoint point)
{
	UIWindow *node;
	UIInset inset;

	node = window;
	while (node)
	{
		inset = get_window_inset(window);
		point = offset_point(point, -window->origin.x, -window->origin.y);
		point = offset_point(point, -inset.left, -inset.top);
		node = node->parent;
	}

	return point;
}

UISize get_window_min_size(UIWindow *window)
{
	if (window == NULL)
		return ui_size(1, 1);

	return window->min_size;
}

UIInset get_window_inset(UIWindow *window)
{
	UIInset inset = {0, 0, 0, 0};
	UIContainerData *cd;

	if (window == NULL || window->private_data == NULL)
		return inset;

	if (!is_kind_of_window(window, CONTAINER_CLASS_NAME))
		return inset;

	cd = (UIContainerData *)window->private_data;

	return cd->inset;
}

/*
 * rect should be screen position
 */
static void do_set_window_rect(UIWindow *window, UIRect rect)
{
	UIInvalidRegion inv_region;
	BOOL poistion_changed = TRUE;
	BOOL size_changed = TRUE;

	if (is_same_point(window->rect.point, rect.point))
		poistion_changed = FALSE;

	if (is_same_size(window->rect.size, rect.size))
		size_changed = FALSE;

	if (!poistion_changed && !size_changed)
		return;

	init_invalid_region(&inv_region);
	/* both old and new rect need refresh */
	add_rect_invalid_region(&inv_region, window->rect);
	add_rect_invalid_region(&inv_region, rect);
	window->rect = rect;
	invalidate_window_with_region(window, &inv_region, FALSE);
	empty_invalid_region(&inv_region);

	if (win_is_drawable(window))
		mark_clip_region_change(window);

	/* layout sub item if window is a container */
	if (size_changed)
		container_layout_sub_window(window);
}

static void do_move_window(UIWindow *window, UIPoint point)
{
	UIRect rect = window->rect;

	rect.point = point;
	do_set_window_rect(window, rect);
}

/*
 * called when move window or change window's parent
 * move window by it's original position
 */
static void window_reposition(UIWindow *window)
{
	do_move_window(window, client_to_screen(window, window->origin));
}

BOOL update_sub_window_position(UIWindow *window)
{
	UIWindow *sub, *control;

	if (window == NULL)
		return FALSE;

	sub = window->sub_list;
	while (sub)
	{
		window_reposition(sub);
		control = sub->control_list;
		while (control)
		{
			window_reposition(control);
			control = control->next;
		}
		sub = sub->next;
	}
	control = window->control_list;
	while (control)
	{
		window_reposition(control);
		control = control->next;
	}

	return TRUE;
}

BOOL set_window_rect(UIWindow *window, UIRect rect)
{
	UIPoint screen_point;
	BOOL moved = FALSE;

	if (window == NULL)
		return FALSE;

	window->origin = rect.point;
	screen_point = client_to_screen(window, rect.point);
	if (screen_point.x != window->rect.point.x || screen_point.y != window->rect.point.y)
		moved = TRUE;

	do_set_window_rect(window, ui_rect(screen_point.x, screen_point.y, rect.size.width, rect.size.height));
	if (moved)
		update_sub_window_position(window);

	return TRUE;
}

BOOL move_window_to(UIWindow *window, UIPoint point)
{
	UIRect rect;

	if (window == NULL)
		return FALSE;

	rect.point = point;
	rect.size = window->rect.size;

	return set_window_rect(window, rect);
}

BOOL set_window_size(UIWindow *window, UISize size)
{
	UIRect rect;

	if (window == NULL)
		return FALSE;

	rect.point = window->rect.point;
	rect.size = size;
	do_set_window_rect(window, rect);

	return TRUE;
}


//-------------------------------------------------------------------------------------------------
// window structure
//-------------------------------------------------------------------------------------------------
BOOL add_sub_window(UIWindow *window, UIWindow *sub_win)
{
	if (!window || !sub_win || window->type == WINDOW_TYPE_CONTROL)
		return FALSE;

	sub_win->parent = window;
	if (sub_win->type == WINDOW_TYPE_SUB)
	{
		if (win_is_focusable(sub_win))
		{
			sub_win->focus_index = get_max_focus_index(window->sub_list) + 1;
		}
		if (!window->sub_list)
		{
			window->sub_list = sub_win;
		}
		else
		{
		    window->sub_list->prev = sub_win;
			sub_win->next = window->sub_list;
			window->sub_list = sub_win;
		}
	}
	else if (sub_win->type == WINDOW_TYPE_CONTROL)
	{
		if (win_is_focusable(sub_win))
		{
			sub_win->focus_index = get_max_focus_index(window->control_list) + 1;
		}
		if (!window->control_list)
		{
			window->control_list = sub_win;
		}
		else
		{
		    window->control_list->prev = sub_win;
			sub_win->next = window->control_list;
			window->control_list = sub_win;
		}
	}
	window_reposition(sub_win);
	update_sub_window_position(sub_win);
	mark_clip_region_change(sub_win);

	return TRUE;
}

BOOL detach_window(UIWindow *window)
{
	UIWindow *node, *next;

	if (!window || !window->parent)
		return FALSE;

	set_window_visible(window, FALSE);
	if (window->type == WINDOW_TYPE_SUB)
	{
		/* need detach first */
		if (window->parent->sub_list == window)
		{
			window->parent->sub_list = window->next;
			if (window->next)
				window->next->prev = NULL;

			return TRUE;
		}
		/* search other */
		node = window->parent->sub_list;
		next = node->next;
		while (next)
		{
			if (next == window)
			{
				node->next = window->next;
				if (window->next)
					window->next->prev = node;

				break;
			}
			node = next;
			next = node->next;
		}
	}
	else if (window->type == WINDOW_TYPE_CONTROL)
	{
		/* need detach first */
		if (window->parent->control_list == window)
		{
			window->parent->control_list = window->next;
			if (window->next)
				window->next->prev = NULL;

			return TRUE;
		}
		/* search other */
		node = window->parent->control_list;
		next = node->next;
		while (next)
		{
			if (next == window)
			{
				node->next = window->next;
				if (window->next)
					window->next->prev = node;

				break;
			}
			node = next;
			next = node->next;
		}
	}
	window_reposition(window);
	update_sub_window_position(window);

	return TRUE;
}


//-------------------------------------------------------------------------------------------------
// window id
//-------------------------------------------------------------------------------------------------
static BOOL window_id_is(UIWindow *win, const char *id)
{
	return (gui_strcmp(win->id, id) == 0);
}

static UIWindow* search_window_in(UIWindow *window, const char *id)
{
	UIWindow *node, *result;

	if (window == NULL || id == NULL || gui_strlen(id) <= 0)
		return NULL;

	/* check current */
	if (window_id_is(window, id))
		return window;

	/* search in sub list */
	node = window->sub_list;
	while (node)
	{
		result = search_window_in(node, id);
		if (result)
			return result;

		node = node->next;
	}

	/* search in control list */
	node = window->control_list;
	while (node)
	{
		result = search_window_in(node, id);
		if (result)
			return result;

		node = node->next;
	}

	/* not found */
	return NULL;
}

/*
 * (comment was deprecated, need update later)
 * id can be window path seperated by dot(.)
 * if there is no dot in path, it will search in top main window
 *
 * for example:
 * get_window("systemsetting.edit1")	// search edit1 from main window named systemsetting
 * get_window(".edit1")					// search edit1 from top main window
 *
 */
UIWindow* find_window_in(UIWindow *root_window, const char *id)
{
	return search_window_in(root_window, id);
}


//-------------------------------------------------------------------------------------------------
// window class
//-------------------------------------------------------------------------------------------------
BOOL is_kind_of_window(UIWindow *window, const char *class_name)
{
	if (window == NULL || class_name == NULL)
		return FALSE;

	if (gui_strncmp(window->class_name, class_name, gui_strlen(class_name)) == 0)
		return TRUE;

	return FALSE;
}


//-------------------------------------------------------------------------------------------------
// window property
//-------------------------------------------------------------------------------------------------
BOOL set_window_color(UIWindow *win, UIColor color)
{
	if (win == NULL) return FALSE;

	if (win->bkg_color != color)
	{
		win->bkg_color = color;
		win->has_trans = is_trans_color(win->painter.mode, color);
		invalidate_window(win);
	}

	return TRUE;
}

BOOL set_window_visible(UIWindow *win, BOOL visible)
{
	if (!win || win->visible == visible)
	{
		return FALSE;
	}
	win->visible = visible;
	mark_clip_region_change(win);
	invalidate_window_ex(win, TRUE);
	/* hiding a focus window, need resign focus */
	if (get_global_focus_window() == win && !visible)
	{
		win_resign_focus(win);
	}
	return TRUE;
}


//-------------------------------------------------------------------------------------------------
// window event
//-------------------------------------------------------------------------------------------------
static EventHandlerMap* find_window_event_handler_node(UIWindow *window, Event event)
{
	EventHandlerMap *event_handler = NULL;

	if (window == NULL)
		return NULL;

	event_handler = find_event_handler_map_node(window->handler_list, event);

	if (event_handler == NULL && window->class_name)
		event_handler = find_class_event_handler(window->class_name, event);

	return event_handler;
}

BOOL window_connect_handler(UIWindow *window, Event event, Handler handler)
{
	if (window == NULL)
		return FALSE;

	return add_event_handler_into_list(&(window->handler_list), event, handler);
}

static Handler win_get_event_handler(UIWindow *win, Event event)
{
	EventHandlerMap *event_handler;

	if (win == NULL) return NULL;

	event_handler = find_window_event_handler_node(win, event);
	if (event_handler == NULL)
	{
		return NULL;
	}
	return event_handler->handler;
}

BOOL window_handle_event(UIWindow *window, Event event, EventData event_data)
{
	Handler handler;

	if (window == NULL)
		return FALSE;

	handler = win_get_event_handler(window, event);
	if (handler == NULL)
		return FALSE;

	printf("window: %s calling event: %d\n", window->id, event);

	handler(window, event, event_data);

	return TRUE;
}

BOOL window_dispatch_event(UIWindow *root_window, Event event, EventData event_data)
{
	return window_handle_event(root_window, event, event_data);

//	UIWindow *focus, *container, *control;

//	/* for the window who wants it in current container(main or sub window) */
//	focus = get_global_focus_window();
//	if (focus == NULL)
//		return FALSE;
//
//	/* focus window can handle this event */
//	if (win_is_control(focus))
//	{
//		if (window_handle_event(focus, event, event_data))
//			return TRUE;
//
//		container = focus->parent;
//	}
//	else
//	{
//		container = focus;
//	}
//
//	/* find in control list */
//	control = container->control_list;
//	while (control)
//	{
//		if (control != focus && win_is_visible(control))
//		{
//			if (window_handle_event(control, event, event_data))
//				return TRUE;
//		}
//		control = control->next;
//	}
//
//	/* find with container */
//	return window_handle_event(container, event, event_data);
}
