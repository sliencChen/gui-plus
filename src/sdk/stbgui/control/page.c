/*
 * page.c
 *
 *  Created on: 2018-7-6
 *      Author: Brant
 */

#include <gui_window.h>
#include <control/page.h>


//-------------------------------------------------------------------------------------------------
// local variable
//-------------------------------------------------------------------------------------------------
static UIPage* page_stack[PAGE_STACK_MAX_NUM];
static int page_stack_num = 0;


//-------------------------------------------------------------------------------------------------
// internal use
//-------------------------------------------------------------------------------------------------
UIPageData* new_page_private_data(void)
{
	UIPageData *page_data;

	page_data = (UIPageData *)gui_malloc(sizeof(UIPageData));
	if (page_data == NULL)
	{
		return NULL;
	}

	/* default parameter */
	gui_memset(page_data, 0, sizeof(UIPageData));

	default_container_data(&(page_data->container));

	page_data->logo_index = -1;
	page_data->stop_channel = TRUE;

	return page_data;
}


//-------------------------------------------------------------------------------------------------
// class event
//-------------------------------------------------------------------------------------------------
int page_on_exit_key(UIWindow *self, Event event, EventData event_data)
{
	if (page_stack_num <= 1)
		return -1;

	dismiss_page(self);

	return 0;
}

int page_will_appear(UIWindow *self, Event event, EventData event_data)
{
	int logo_index = -1;

	mark_clip_region_change(self);
	invalidate_window_ex(self, TRUE);
//	set_window_focus(self);

	logo_index = get_page_logo_index(self);
	if (logo_index >= 0)
	{
//		gui_play_logo(logo_index);
	}
	else if (get_page_stop_channel(self))
	{
//		gui_stop_channel();
	}

	return 0;
}

int page_will_disappear(UIWindow *self, Event event, EventData event_data)
{
	return 0;
}


BOOL register_page_event(void)
{
	BOOL result = TRUE;

	result &= register_class_event_handler(PAGE_CLASS_NAME, GUI_EVT_VK_EXIT,				page_on_exit_key);
	result &= register_class_event_handler(PAGE_CLASS_NAME, GUI_EVT_PAGE_WILL_APPEAR,		page_will_appear);
	result &= register_class_event_handler(PAGE_CLASS_NAME, GUI_EVT_PAGE_WILL_DISAPPEAR,	page_will_disappear);

	return result;
}

static BOOL register_page_class(void)
{
	static BOOL registered = FALSE;
	BOOL res;

	if (registered)
		return TRUE;

	res = register_page_event();
	registered = TRUE;

	return res;
}


//-------------------------------------------------------------------------------------------------
// window manager callback
//-------------------------------------------------------------------------------------------------
static BOOL page_on_destroy(UIPage *self)
{
	UIPageData *page_data;

	if (self == NULL || self->private_data == NULL)
		return TRUE;

	page_data = (UIPageData *)self->private_data;
	if (page_data->container.sizer)
		destroy_sizer(page_data->container.sizer);

	gui_free(page_data);

	return TRUE;
}


//-------------------------------------------------------------------------------------------------
// interface
//-------------------------------------------------------------------------------------------------
UIPage* create_page(const char *id)
{
	UIPage *page;

	register_page_class();

	page = create_main_window(id, 0, 0, main_canvas_width(), main_canvas_height());
	if (page)
	{
		UIPageData *page_data = new_page_private_data();

		if (page_data == NULL)
		{
			destroy_window(page);
			return NULL;
		}

		page->private_data = page_data;
		page->focus_stop = TRUE;
		page->class_name = PAGE_CLASS_NAME;
		page->on_destroy = page_on_destroy;
	}

	return page;
}

int set_page_title(UIPage *page, const char *title)
{
	UIPageData *page_data;

	if (!page || !page->private_data)
		return -1;

	page_data = (UIPageData *)page->private_data;
	set_string_text(&(page_data->title), title);

	return 0;
}

BOOL set_page_logo_index(UIPage *page, int logo_index)
{
	UIPageData *page_data;

	if (!page || !page->private_data)
		return FALSE;

	page_data = (UIPageData *)page->private_data;
	page_data->logo_index = logo_index;

	return TRUE;
}

int get_page_logo_index(UIPage *page)
{
	UIPageData *page_data;

	if (!page || !page->private_data)
		return FALSE;

	page_data = (UIPageData *)page->private_data;

	return page_data->logo_index;
}

BOOL set_page_stop_channel(UIPage *page, BOOL stop)
{
	UIPageData *page_data;

	if (!page || !page->private_data)
		return FALSE;

	page_data = (UIPageData *)page->private_data;
	page_data->stop_channel = stop;

	return TRUE;
}

BOOL get_page_stop_channel(UIPage *uipage)
{
	UIPageData *page_data;

	if (!uipage || !uipage->private_data)
		return FALSE;

	page_data = (UIPageData *)uipage->private_data;

	return page_data->stop_channel;
}


//-------------------------------------------------------------------------------------------------
// page stack - internal
//-------------------------------------------------------------------------------------------------
static BOOL push_page_to_stack(UIPage *page)
{
	if (page == NULL)
		return FALSE;

	if (page_stack_num >= PAGE_STACK_MAX_NUM)
		return FALSE;

	page_stack[page_stack_num] = page;
	page_stack_num++;

	return TRUE;
}

static UIPage *get_top_page(void)
{
	if (page_stack_num < 1)
		return NULL;

	return page_stack[page_stack_num - 1];
}

static UIPage *get_prev_page(void)
{
	if (page_stack_num < 2)
		return NULL;

	return page_stack[page_stack_num - 2];
}

static BOOL pop_page_from_stack(UIPage *page)
{
	/* can only pop top page */
	if (page == NULL || page != get_top_page())
		return FALSE;

	if (page_stack_num <= 1)
	{
		GUI_WARN("Warning: You are removing the last page now!\n");
	}
	page_stack_num--;

	return TRUE;
}


//-------------------------------------------------------------------------------------------------
// page stack - interface
//-------------------------------------------------------------------------------------------------
BOOL previous_page_is(const char *id)
{
	UIPage *prev_top;

	prev_top = get_prev_page();
	if (prev_top == NULL)
		return FALSE;

	return gui_strcmp(prev_top->id, id) == 0;
}

int present_page(UIPage *page)
{
	UIPage *prev_top_page;

	prev_top_page = get_top_page();

	if (!push_page_to_stack(page))
		return -1;

	/* previous page should be hidden, empty its region for save memory */
	win_empty_region(prev_top_page);
	send_window_event(prev_top_page, GUI_EVT_PAGE_WILL_DISAPPEAR, 0);

	set_window_focus(page);
	send_window_event(page, GUI_EVT_PAGE_WILL_APPEAR, 0);

	return 0;
}


int dismiss_page(UIPage *page)
{
	UIPage *prev_page;

	/* can only dismiss top page */
	if (get_top_page() != page)
		return -1;

	prev_page = get_prev_page();

	send_window_event(page, GUI_EVT_PAGE_WILL_DISAPPEAR, 0);
	send_window_event(page, GUI_EVT_DESTROY, 0);
	send_window_event(prev_page, GUI_EVT_PAGE_WILL_APPEAR, 0);

	return 0;
}


//-------------------------------------------------------------------------------------------------
// run loop
//-------------------------------------------------------------------------------------------------
int page_run_loop(void)
{
	UIWindow *window;
	Event event;
	EventData data;
	UIPage *cur_page = get_top_page();

	if (cur_page == NULL)
		return -1;

	while (1)
	{
		flush_window(cur_page);

		if (receive_window_event(&window, &event, &data, GUI_MSG_WAIT_FOREVER))
		{
			if (window != NULL)
				window_handle_event(window, event, data);
			else
				window_dispatch_event(cur_page, event, data);

			/* exit run loop when current page was destroyed */
			if ((window == NULL || window == cur_page) && event == GUI_EVT_DESTROY)
			{
				pop_page_from_stack(cur_page);
				break;
			}
		}
	}
	return 0;
}

BOOL set_page_need_stop_channel(UIPage *uipage, BOOL on_channel)
{
    UIPageData *page_data;

	if (!uipage || !uipage->private_data)
	{
		return FALSE;
	}
	page_data = (UIPageData *)uipage->private_data;
	page_data->stop_channel = on_channel;
	return TRUE;
}

BOOL set_page_need_play_logo(UIPage *uipage, int logo_index)
{
    UIPageData *page_data;

    if (!uipage || !uipage->private_data)
	{
		return FALSE;
	}
	page_data = (UIPageData *)uipage->private_data;
	page_data->logo_index = logo_index;
	return TRUE;
}

BOOL set_page_container_inset(UIPage *uipage, UIInset inset)
{
	if (!uipage)
		return FALSE;

	set_container_inset(uipage, inset);

	return TRUE;
}

//-------------------------------------------------------------------------------------------------
// typical theme
//-------------------------------------------------------------------------------------------------

