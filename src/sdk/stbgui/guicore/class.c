/*
 * class.c
 *
 *  Created on: 2018-8-4
 *      Author: Brant
 */

#include <gdi/gui_interface.h>
#include <guicore/event.h>
#include <guicore/class.h>


typedef struct _ui_class_event_
{
	const char *class_name;
	EventHandlerMap *handler_list;
	struct _ui_class_event_ *next;
} UIClassEvent;


static UIClassEvent *class_event_handler_list = NULL;


BOOL register_class_event_handler(const char *class_name, Event event, Handler handler)
{
	UIClassEvent *node, *tail, *temp;

	tail = node = class_event_handler_list;
	while (node)
	{
		tail = node;
		if (gui_strcmp(node->class_name, class_name) == 0)
		{
			return add_event_handler_into_list(&(node->handler_list), event, handler);
		}
		node = node->next;
	}
	temp = gui_malloc(sizeof(UIClassEvent));
	gui_memset(temp, 0, sizeof(UIClassEvent));
	temp->class_name = class_name;
	if (class_event_handler_list)
	{
		tail->next = temp;
	}
	else
	{
		class_event_handler_list = temp;
	}
	return add_event_handler_into_list(&(temp->handler_list), event, handler);
}


static EventHandlerMap* find_class_event_handler_map_node(const char *class_name, Event event)
{
	UIClassEvent *node;

	node = class_event_handler_list;
	while (node)
	{
		if (gui_strcmp(node->class_name, class_name) == 0)
		{
			return find_event_handler_map_node(node->handler_list, event);
		}
		node = node->next;
	}
	return NULL;
}

EventHandlerMap* find_class_event_handler(const char *class_name, Event event)
{
	return find_class_event_handler_map_node(class_name, event);
}
