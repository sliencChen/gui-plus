/*
 * event.h
 *
 *  Created on: 2015-8-17
 *      Author: Brant
 */

#ifndef _GUI_EVENT_H_
#define _GUI_EVENT_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "event_list.h"


typedef U32 Event;
typedef U32 EventData;

struct _ui_window_;
typedef int (*Handler)(struct _ui_window_ *window, Event event, EventData event_data);


typedef struct _event_handler_map_
{
	Event event;
	Handler handler;
	struct _event_handler_map_ *next;
} EventHandlerMap;


BOOL send_window_event(struct _ui_window_ *window, Event event, EventData event_data);
BOOL receive_window_event(struct _ui_window_ **window, Event *event, EventData *event_data, U32 timeout);

EventHandlerMap* find_event_handler_map_node(EventHandlerMap *map_list, Event event);
BOOL add_event_handler_into_list(EventHandlerMap **map_list, Event event, Handler handler);
BOOL delete_all_event_handler(EventHandlerMap *map_list);


#ifdef __cplusplus
}
#endif

#endif /* _GUI_EVENT_H_ */
