/*
 * window.h
 *
 *  Created on: 2015-4-25
 *      Author: Brant
 */

#ifndef _GUI_WINDOW_H_
#define _GUI_WINDOW_H_

#ifdef __cplusplus
extern "C" {
#endif


typedef enum
{
	WINDOW_TYPE_MAIN,
	WINDOW_TYPE_SUB,
	WINDOW_TYPE_CONTROL,
	WINDOW_TYPE_SIZER,
	WINDOW_TYPE_NUM
} UIWindowType;

typedef struct _ui_window_
{
	char				id[GUI_WINDOW_ID_SIZE];
	U32					type: 2;			/* main, sub or control                            */
	U32					visible: 1;			/* visible state                                   */
	U32					enable: 1;			/* enable state                                    */
	U32					focus: 1;			/* focus state, only for window which accept key   */
	U32					focus_index: 8;		/* focus order                                     */
	U32					z_order: 16;		/* z order, low -> high values small -> big        */
	U32					has_trans: 1;		/* has transparent color(!= clear color) mark      */
	U32					clip_rgn_change: 1;	/* internal use for window manager                 */
	U32					focus_stop: 1;		/* can set focus or not                            */
	U32					reserved: 1;		/* for future use                                  */

	UIRect				rect;				/* position                                        */
	UIPoint				origin;				/* original point of window                        */
	UISize				min_size;			/* for layout                                      */
	UIColor				bkg_color;			/* background color                                */

	UIClipRegion		clip_region;		/* list of visible area                            */
	UIInvalidRegion		inv_region;			/* list of refresh area                            */
	UIPainter			painter;			/* used for draw                                   */

	void				*private_data;		/* private data for different type of window       */
	void				*theme_data;		/* save data for theme                             */
	const char			*class_name;		/* for windows which have same behaviors           */

	struct _ui_window_	*parent;			/* point to parent window                          */
	struct _ui_window_	*sub_list;			/* point to sub window list                        */
	struct _ui_window_	*control_list;		/* point to control list                           */

	struct _ui_window_	*next;				/* point to next window of same group              */
	struct _ui_window_	*prev;				/* point to previous window of same group          */

	EventHandlerMap		*handler_list;		/* handler list                                    */

	BOOL (*on_erase_bkg)(struct _ui_window_ *self);
	BOOL (*on_draw)(struct _ui_window_ *self);
	BOOL (*on_destroy)(struct _ui_window_ *self);
} UIWindow;


/*
 * create a main window
 */
UIWindow* create_main_window(const char *id, int x, int y, int width, int height);

/*
 * create a sub window
 */
UIWindow* create_sub_window(const char *id, int x, int y, int width, int height);

/*
 * create a control
 */
UIWindow* create_control(const char *id, int x, int y, int width, int height);

/*
 * delete all resource of window
 * note that: destroy window will also delete all its sub window and control
 */
BOOL destroy_window(UIWindow *window);

/*
 * add sub window to parent window
 */
BOOL add_sub_window(UIWindow *window, UIWindow *sub_win);

/*
 * detach window from its parent, not delete
 */
BOOL detach_window(UIWindow *window);


/*
 * search window in root window, including itself
 */
UIWindow* find_window_in(UIWindow *root_window, const char *id);

/*
 * check window class, window using sub class is also match super class
 */
BOOL is_kind_of_window(UIWindow *window, const char *class_name);

/*
 * minimum size of window, for sizer layout. if size <= 0, sizer will give window another size by its algorithm
 */
UISize get_window_min_size(UIWindow *window);

/*
 * only valid for container window, it is edge of window
 */
UIInset get_window_inset(UIWindow *window);


/*
 * set window background color
 */
BOOL set_window_color(UIWindow *window, UIColor color);

/*
 * set window position and size
 */
BOOL set_window_rect(UIWindow *window, UIRect rect);

/*
 * set window position
 */
BOOL move_window_to(UIWindow *window, UIPoint point);

/*
 * set window size
 */
BOOL set_window_size(UIWindow *window, UISize size);

/*
 * change position of window's all sub window, called when position or inset changed
 */
BOOL update_sub_window_position(UIWindow *window);

/*
 * set window show / hide
 */
BOOL set_window_visible(UIWindow *window, BOOL visible);

/*
 * set window to focus state
 */
BOOL set_window_focus(UIWindow *window);


/*
 * translate point in window's client area to screen position
 */
UIPoint client_to_screen(UIWindow *window, UIPoint point);

/*
 * translate screen position to window client position
 */
UIPoint screen_to_client(UIWindow *window, UIPoint point);


/*
 * add an event handler for window
 */
BOOL window_connect_handler(UIWindow *window, Event event, Handler handler);

/*
 * process event for specific window
 */
BOOL window_handle_event(UIWindow *window, Event event, EventData event_data);

/*
 * find which window in parameter "root_window" can handle this event
 */
BOOL window_dispatch_event(UIWindow *root_window, Event event, EventData event_data);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _GUI_WINDOW_H_ */
