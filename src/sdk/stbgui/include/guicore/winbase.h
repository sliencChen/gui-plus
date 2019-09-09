/*
 * winbase.h
 *
 *  Created on: 2015-6-17
 *      Author: Brant
 */

#ifndef _GUI_WINBASE_H_
#define _GUI_WINBASE_H_

#ifdef __cplusplus
extern "C" {
#endif


BOOL win_is_visible(const UIWindow *win);
BOOL win_is_drawable(const UIWindow *win);
BOOL win_is_focusable(const UIWindow *win);
BOOL win_is_main(const UIWindow *win);
BOOL win_is_sub(const UIWindow *win);
BOOL win_is_control(const UIWindow *win);

BOOL win_init_region(UIWindow *win);
BOOL win_empty_region(UIWindow *win);
void mark_clip_region_change(UIWindow *win);

UIRect client_rect_to_win_rect(UIWindow *win, UIRect client_rect);

BOOL invalidate_window_with_region(UIWindow *win, UIInvalidRegion *region, BOOL force);
BOOL invalidate_window_rect(UIWindow *win, UIRect rect);
BOOL invalidate_window_ex(UIWindow *win, BOOL force);
BOOL invalidate_window(UIWindow *win);

void flush_window(UIWindow *win);


#ifdef __cplusplus
}
#endif

#endif /* _GUI_WINBASE_H_ */
