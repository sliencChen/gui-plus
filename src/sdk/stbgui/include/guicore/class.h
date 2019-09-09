/*
 * class.h
 *
 *  Created on: 2018-8-4
 *      Author: Brant
 */

#ifndef _GUI_CLASS_H_
#define _GUI_CLASS_H_

#ifdef __cplusplus
extern "C"
{
#endif


BOOL register_class_event_handler(const char *class_name, Event event, Handler handler);
EventHandlerMap* find_class_event_handler(const char *class_name, Event event);


#ifdef __cplusplus
}
#endif

#endif /* _GUI_CLASS_H_ */
