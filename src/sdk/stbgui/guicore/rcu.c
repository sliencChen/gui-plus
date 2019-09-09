/*
 * rcu.c
 *
 *  Created on: 2019-7-15
 *      Author: Brant
 */

#include <gdi/gui_interface.h>
#include <guicore/event.h>
#include <guicore/rcu.h>


typedef struct _rcu_vk_
{
	unsigned char main;
	unsigned char substitute;
} RcuVK;


static BOOL convert_hk_to_vk(int hk_code, RcuVK *vk)
{
	BOOL find = TRUE;

	gui_printf("key: 0x%04x\n", hk_code);

	if (hk_code == 0x26)
	{
		vk->main = GUI_EVT_VK_UP;
	}
	else if (hk_code == 0x28)
	{
		vk->main = GUI_EVT_VK_DOWN;
	}
	else if (hk_code == 0x25)
	{
		vk->main = GUI_EVT_VK_LEFT;
	}
	else if (hk_code == 0x27)
	{
		vk->main = GUI_EVT_VK_RIGHT;
	}
	else if (hk_code == 0x0D)
	{
		vk->main = GUI_EVT_VK_SELECT;
	}
	else if (hk_code == 0x1B)
	{
		vk->main = GUI_EVT_VK_EXIT;
	}
	else
	{
		find = FALSE;
	}

	return find;
}


int send_rcu_key(unsigned int hk_code, BOOL repeat_flag)
{
	RcuVK vk;
	RcuKeyEvtData key_data;
	U32 key_data_value;

	if (!convert_hk_to_vk(hk_code, &vk))
		return -1;

	key_data.substitute = vk.substitute;
	key_data.repeat_flag = repeat_flag;
	gui_memcpy(&key_data_value, &key_data, sizeof(U32));

	return send_window_event(NULL, vk.main, key_data_value);
}
