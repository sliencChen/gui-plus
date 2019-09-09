/*
 * page_theme.c
 *
 *  Created on: 2018-8-8
 *      Author: Brant
 */

#include <gui_common.h>
#include <guicore/event.h>
#include <guicore/class.h>
#include <gdi/bitmap.h>
#include <guicore/window.h>
#include <guicore/winbase.h>
#include <control/page.h>
#include <theme/page_theme.h>


BOOL gear_setting_page(UIPage *self)
{
	UIPainter *painter = &(self->painter);
	UIRect rect = self->rect;
	DrawBmpParam bmp_param;

	#if 0
	if (!get_bmp_info_by_name(&(bmp_param.bmp_info), "MENUICONLIGHT30"))
	{
		return FALSE;
	}
	#endif
	bmp_param.mode = DRAW_BMP_MODE_NORMAL;

	draw_bitmap(painter, rect, &bmp_param);

	return TRUE;
}
