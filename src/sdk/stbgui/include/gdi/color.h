/*
 * color.h
 *
 *  Created on: 2017-12-25
 *      Author: Brant
 */

#ifndef _OSD_COLOR_H_
#define _OSD_COLOR_H_

#ifdef __cplusplus
extern "C"
{
#endif


typedef enum
{
	COLOR_MODE_ARGB8888,
	COLOR_MODE_RGB565,
	COLOR_MODE_ARGB1555,
	COLOR_MODE_RGB888,
	COLOR_MODE_NUM
} UIColorMode;


#define TRANS_COLOR				(0x00000000)
#define CLEAR_COLOR				(0x00ff00ff)

#define RED_COLOR				(0xffff0000)
#define DARK_RED_COLOR			(0xffAA0000)
#define GREEN_COLOR				(0xff00ff00)
#define DARK_GREEN_COLOR		(0xff00AA00)
#define YELLOW_COLOR			(0xffffff00)
#define GOLD_YELLOW_COLOR		(0xffff9f00)
#define BLUE_COLOR				(0xff0000ff)
#define BLACK_COLOR				(0xff000000)

#define WHITE_COLOR				(0xffffffff)
#define DEEP_GRAY_COLOR			(0xff3e3e3e)
#define LIGHT_GRAY_COLOR		(0xffb0b0b0)
#define NORMAL_GRAY_COLOR		(0xff808080)
#define ORANGE_COLOR			(0xffAA8800)
#define MARK_READ_COLOR			(0x7fFF9900)


U8 get_color_pixel_size(UIColorMode color_mode);
UIColor argb_to_gray(UIColor color);
BOOL is_trans_color(UIColorMode mode, UIColor color);


#ifdef __cplusplus
}
#endif

#endif /* _OSD_COLOR_H_ */
