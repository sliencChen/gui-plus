/*
 * gui_main.c
 *
 *  Created on: 2017-12-29
 *      Author: Brant
 */

#include <gui_common.h>
#include <guicore/event.h>
#include <guicore/window.h>


int gui_init(void)
{
	init_invalid_region_heap();
	init_clip_region_heap();

//	init_main_canvas(get_osd_addr(), ui_size(get_osd_width(), get_osd_height()), COLOR_MODE_ARGB8888);

	return 0;
}
