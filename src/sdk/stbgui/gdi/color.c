/*
 * color.c
 *
 *  Created on: 2017-12-25
 *      Author: Brant
 */

#include <gdi/gui_interface.h>
#include <gdi/color.h>


U8 get_color_pixel_size(UIColorMode color_mode)
{
	return color_mode == COLOR_MODE_ARGB8888 ? 4 : 2;
}

UIColor argb_to_gray(UIColor color) //It's only for colorful, not white/black color
{
	U8 ucA, ucR, ucG, ucB;
	U8 ucGrayVal = 0;
	U32 uiRet = 0;

	ucA = (color & 0xff000000) >> 24;
	ucR = (color & 0x00ff0000) >> 16;
	ucG = (color & 0x0000ff00) >> 8;
	ucB = (color & 0x000000ff);

	ucGrayVal = (int)(ucR * 0.299) + (int)(ucG * 0.587) + (int)(ucB * 0.114);
	uiRet = (ucA << 24) | (ucGrayVal << 16) | (ucGrayVal << 8) | ucGrayVal;

	return uiRet;
}

BOOL is_trans_color(UIColorMode mode, UIColor color)
{
	if (color == CLEAR_COLOR)
		return FALSE;

	if (mode == COLOR_MODE_ARGB8888)
	{
		if ((color & 0xff000000) == 0xff000000)
			return FALSE;
	}
	else if (mode == COLOR_MODE_ARGB1555)
	{
		if ((color & 0x80000000) == 0x80000000)
			return FALSE;
	}
	return TRUE;
}

