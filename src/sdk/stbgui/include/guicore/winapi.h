/*
 * winapi.h
 *
 *  Created on: 2018-8-8
 *      Author: Brant
 */

#ifndef _GUI_WINAPI_H_
#define _GUI_WINAPI_H_

#ifdef __cplusplus
extern "C" {
#endif


/*
 * add window to parent
 */
BOOL add_sub_window(char *parent, char *sub);

/*
 * detach from parent, not delete
 */
BOOL detach_window(char *id);

/*
 * set window show/hide status
 */
void set_window_visible(char *id, BOOL visible);

/*
 * get window show/hide status
 */
BOOL get_window_visible(char *id);

/*
 * set window background color
 */
void set_window_color(char *id, UIColor color);

/*
 * set window position, according to parent
 */
void set_window_rect(char *id, UIRect rect);

/*
 * set window size
 */
void set_window_size(char *id, UISize size);

/*
 * set window to focus state
 */
BOOL set_window_focus(char *id);

/*
 * delete window from memory
 */
void destroy_window(char *id);

/*
 * connect a event handler for window
 */
BOOL window_connect_handler(char *id, Event event, Handler handler);

/*
 * invoke event handler(if exists)
 */
//BOOL handle_window_event(char *id, Event event, EventData event_data);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _GUI_WINAPI_H_ */
