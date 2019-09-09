/*
 * winapi.c
 *
 *  Created on: 2018-8-8
 *      Author: Brant
 */

#include <gui_common.h>
#include <gdi/graphic.h>
#include <guicore/event.h>
#include <guicore/class.h>
#include <guicore/window.h>
#include <guicore/winbase.h>
#include <guicore/winapi.h>


BOOL add_sub_window(char *parent, char *sub)
{
	UIWindow *win, *child;

	win = get_window(parent);
	child = get_window(sub);
	if (!win || !sub) return FALSE;

	return win_add_sub(win, child);
}


BOOL detach_window(char *id)
{
	return win_detach_from_parent(get_window(id));
}


void set_window_visible(char *id, BOOL visible)
{
	win_set_visible(get_window(id), visible);
}


BOOL get_window_visible(char *id)
{
	return win_is_visible(get_window(id));
}


void set_window_color(char *id, UIColor color)
{
	win_set_color(get_window(id), color);
}


void set_window_rect(char *id, UIRect rect)
{
	win_set_rect(get_window(id), rect);
}


void set_window_size(char *id, UISize size)
{
	win_set_size(get_window(id), size);
}


BOOL set_window_focus(char *id)
{
	return win_set_focus(get_window(id));
}


void destroy_window(char *id)
{
	win_destroy(get_window(id));
}


BOOL window_connect_handler(char *id, Event event, Handler handler)
{
	return win_connect_handler(get_window(id), event, handler);
}


//BOOL handle_window_event(char *id, Event event, EventData event_data)
//{
//	return win_handle_event(get_window(id), event, event_data);
//}
