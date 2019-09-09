/*
 * event_list.h
 *
 *  Created on: 2019-7-15
 *      Author: Brant
 */

#ifndef GUI_EVENT_LIST_H_
#define GUI_EVENT_LIST_H_


enum
{
	GUI_EVT_IDLE = 0,

	/* rcu key */
	GUI_EVT_VK_UP,
	GUI_EVT_VK_DOWN,
	GUI_EVT_VK_LEFT,
	GUI_EVT_VK_RIGHT,

	GUI_EVT_VK_SELECT,
	GUI_EVT_VK_MENU,
	GUI_EVT_VK_EXIT,

	GUI_EVT_VK_RED,
	GUI_EVT_VK_GREEN,
	GUI_EVT_VK_YELLOW,
	GUI_EVT_VK_BLUE,
	GUI_EVT_VK_MAX,

	/* for system */


	/* for all window */
	GUI_EVT_DESTROY,

	/* for container */

	/* for page control */
	GUI_EVT_PAGE_WILL_APPEAR,
	GUI_EVT_PAGE_WILL_DISAPPEAR,
	GUI_EVT_EXIT_RUN_LOOP,

	/* for menu control */
	GUI_EVT_MENU_SLECTETION_CHANGED,
	
	GUI_EVT_NUM
};


#endif /* GUI_EVENT_LIST_H_ */
