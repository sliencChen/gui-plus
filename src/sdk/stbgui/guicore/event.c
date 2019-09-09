/*
 * event.c
 *
 *  Created on: 2015-10-29
 *      Author: Brant
 */

#include <gdi/gui_interface.h>
#include <guicore/event.h>


BOOL send_window_event(struct _ui_window_ *window, Event event, EventData event_data)
{
	int res;

	res = gui_message_send((U32)event, (U32)event_data, 0, (U32)window);

	return res == 0 ? TRUE : FALSE;
}

BOOL receive_window_event(struct _ui_window_ **window, Event *event, EventData *event_data, U32 timeout)
{
	int res = 0;
	U32 win = 0;

	res = gui_message_receive(event, event_data, NULL, &win, timeout);
	*window = (struct _ui_window_ *)win;

	return res == 0 ? TRUE : FALSE;
}


EventHandlerMap* find_event_handler_map_node(EventHandlerMap *map_list, Event event)
{
	EventHandlerMap *head;

	head = map_list;
	while (head)
	{
		if (head->event == event)
		{
			return head;
		}
		head = head->next;
	}
	return NULL;
}


BOOL add_event_handler_into_list(EventHandlerMap **map_list, Event event, Handler handler)
{
	EventHandlerMap *head, *tail, *node;
	BOOL find = FALSE;

	tail = head = *map_list;
	while (head)
	{
		tail = head;
		if (head->event == event)
		{
			head->handler = handler;
			find = TRUE;
			break;
		}
		head = head->next;
	}
	if (!find)
	{
		node = gui_malloc(sizeof(EventHandlerMap));
		if (!node)
		{
			return FALSE;
		}
		node->event = event;
		node->handler = handler;
		node->next = NULL;
		if (tail)
		{
			tail->next = node;
		}
		else
		{
			*map_list = node;
		}
	}
	return TRUE;
}

BOOL delete_all_event_handler(EventHandlerMap *map_list)
{
	EventHandlerMap *node, *next;

	node = map_list;
	while (node)
	{
		next = node->next;
		gui_free(node);
		node = next;
	}

	return TRUE;
}
