/*
 * rect.h
 *
 *  Created on: 2017-9-27
 *      Author: Brant
 */

#ifndef _OSD_RECT_H_
#define _OSD_RECT_H_

#ifdef __cplusplus
extern "C"
{
#endif


#define MIN(a,b)    ((a)>(b)?(b):(a))
#define MAX(a,b)    ((a)<(b)?(b):(a))


/* point */
UIPoint ui_point(int x, int y);
UIPoint offset_point(UIPoint point, int dx, int dy);
BOOL is_same_point(UIPoint a, UIPoint b);


//-------------------------------------------------------------------------------------------------
// size
//-------------------------------------------------------------------------------------------------
#define EMPTY_SIZE		ui_size(0, 0)

UISize ui_size(int width, int height);
BOOL is_empty_size(UISize size);
BOOL is_same_size(UISize a, UISize b);
UISize size_minus(UISize a, UISize b);


//-------------------------------------------------------------------------------------------------
// rect
//-------------------------------------------------------------------------------------------------
#define EMPTY_RECT		ui_rect(0, 0, 0, 0)

UIRect ui_rect(int x, int y, int width, int height);
UIRect ui_empty_rect(void);

int rect_left(UIRect rect);
void set_rect_left(UIRect *rect, int left);

int rect_top(UIRect rect);
void set_rect_top(UIRect *rect, int top);

int rect_right(UIRect rect);
void set_rect_right(UIRect *rect, int right);

int rect_bottom(UIRect rect);
void set_rect_bottom(UIRect *rect, int bottom);

BOOL is_empty_rect(UIRect rect);
void set_rect_empty(UIRect *rect);

BOOL intersect_rect(UIRect *result, UIRect rect1, UIRect rect2);
BOOL is_intersect(UIRect rect1, UIRect rect2);
UIRect offset_rect(UIRect rect, int dx, int dy);
UIRect inflate_rect(UIRect rect, int dx, int dy);

BOOL point_in_rect(UIPoint point, UIRect rect);
BOOL rect_in_rect(UIRect small1, UIRect big1);

UIPoint ui_center_point(UIRect container, UISize object);

char* rect_to_string(UIRect rect);


//-------------------------------------------------------------------------------------------------
// rect util
//-------------------------------------------------------------------------------------------------
enum
{
	UI_ALIGN_NOT		= 0x00,
	UI_ALIGN_CENTER_HOR = 0x01,
	UI_ALIGN_LEFT		= UI_ALIGN_NOT,
	UI_ALIGN_TOP		= UI_ALIGN_NOT,
	UI_ALIGN_RIGHT		= 0x02,
	UI_ALIGN_BOTTOM		= 0x04,
	UI_ALIGN_CENTER_VER = 0x08,

	UI_ALIGN_CENTER		= (UI_ALIGN_CENTER_HOR|UI_ALIGN_CENTER_VER),
};

UIRect rect_align(UISize object, UIRect container, int align_style);


//-------------------------------------------------------------------------------------------------
// inset
//-------------------------------------------------------------------------------------------------
UIInset ui_inset(int left, int top, int right, int bottom);
BOOL is_same_inset(UIInset a, UIInset b);
UIRect rect_intersect_inset(UIRect rect, UIInset inset);
UISize rect_intersect_inset_size(UIRect rect, UIInset inset);
UISize inset_delta(UIInset a, UIInset b);


#ifdef __cplusplus
}
#endif

#endif /* _OSD_RECT_H_ */
