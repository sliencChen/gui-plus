/*
 * page.h
 *
 *  Created on: 2018-7-6
 *      Author: Brant
 */

#ifndef _GUI_PAGE_H_
#define _GUI_PAGE_H_

#ifdef __cplusplus
extern "C"
{
#endif
#include <sizer/sizer.h>
#include <control/container.h>

//-------------------------------------------------------------------------------------------------
// define
//-------------------------------------------------------------------------------------------------
typedef UIWindow			UIPage;

#define PAGE_CLASS_NAME		CONTAINER_CLASS_NAME "/Page"


//-------------------------------------------------------------------------------------------------
// private data
//-------------------------------------------------------------------------------------------------

#define PAGE_DEFAULT_WIDTH  1280
#define PAGE_DEFAULT_HEIGHT  720

typedef struct _ui_page_data_
{
	/* inherits from container */
	UIContainerData container;

	/* special for page */
	UIString title;
	S16 logo_index;				/* set to -1 if no logo */
	U16 stop_channel	:1;		/* need stop channel in this page or not */
} UIPageData;


//-------------------------------------------------------------------------------------------------
// interface
//-------------------------------------------------------------------------------------------------
UIPage* create_page(const char *id);

/* set / get */
int set_page_title(UIPage *page, const char *title);
BOOL set_page_logo_index(UIPage *page, int logo_index);
int get_page_logo_index(UIPage *page);
BOOL set_page_stop_channel(UIPage *page, BOOL stop);
BOOL get_page_stop_channel(UIPage *page);


//-------------------------------------------------------------------------------------------------
// default event handler
//-------------------------------------------------------------------------------------------------
int page_on_exit_key(UIWindow *self, Event event, EventData event_data);
int page_will_appear(UIWindow *self, Event event, EventData event_data);
int page_will_disappear(UIWindow *self, Event event, EventData event_data);


//-------------------------------------------------------------------------------------------------
// page stack
//-------------------------------------------------------------------------------------------------
BOOL previous_page_is(const char *id);
int present_page(UIPage *page);
int dismiss_page(UIPage *page);
BOOL set_page_need_play_logo(UIPage *uipage, int logo_index);
BOOL set_page_need_stop_channel(UIPage *uipage, BOOL on_channel);
BOOL set_page_container_inset(UIPage *uipage, UIInset inset);

//-------------------------------------------------------------------------------------------------
// run loop
//-------------------------------------------------------------------------------------------------
int page_run_loop(void);	/* keep receiving event and send to sub window */


#ifdef __cplusplus
}
#endif

#endif /* _GUI_PAGE_H_ */
