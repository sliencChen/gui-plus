/*
 * rect.c
 *
 *  Created on: 2017-9-27
 *      Author: Brant
 */

#include <gdi/gui_interface.h>
#include <gdi/rect.h>


/* point */
UIPoint ui_point(int x, int y)
{
	UIPoint point = {x, y};

	return point;
}

UIPoint offset_point(UIPoint point, int dx, int dy)
{
	UIPoint pt = {point.x + dx, point.y + dy};

	return pt;
}

BOOL is_same_point(UIPoint a, UIPoint b)
{
	return a.x == b.x && a.y == b.y;
}


//-------------------------------------------------------------------------------------------------
// size
//-------------------------------------------------------------------------------------------------
UISize ui_size(int width, int height)
{
	UISize size = {width, height};

	return size;
}

BOOL is_empty_size(UISize size)
{
	return size.width <= 0 || size.height <= 0;
}

BOOL is_same_size(UISize a, UISize b)
{
	return a.width == b.width && a.height == b.height;
}

UISize size_minus(UISize a, UISize b)
{
	return ui_size(a.width - b.width, a.height - b.height);
}


//-------------------------------------------------------------------------------------------------
// rect
//-------------------------------------------------------------------------------------------------
UIRect ui_rect(int x, int y, int width, int height)
{
	UIRect rect = {{x, y}, {width, height}};

	return rect;
}

UIRect ui_empty_rect(void)
{
	return ui_rect(0, 0, 0, 0);
}

int rect_left(UIRect rect)
{
	return rect.point.x;
}

void set_rect_left(UIRect *rect, int left)
{
	int right = rect->point.x + rect->size.width;

	rect->point.x = left;
	rect->size.width = right - left;
}

int rect_top(UIRect rect)
{
	return rect.point.y;
}

void set_rect_top(UIRect *rect, int top)
{
	int bottom = rect->point.y + rect->size.height;

	rect->point.y = top;
	rect->size.height = bottom - top;
}

int rect_right(UIRect rect)
{
	return rect.point.x + rect.size.width;
}

void set_rect_right(UIRect *rect, int right)
{
	rect->size.width = right - rect->point.x;
}

int rect_bottom(UIRect rect)
{
	return rect.point.y + rect.size.height;
}

void set_rect_bottom(UIRect *rect, int bottom)
{
	rect->size.height = bottom - rect->point.y;
}

BOOL is_empty_rect(UIRect rect)
{
	return rect.size.width <= 0 || rect.size.height <= 0;
}

void set_rect_empty(UIRect *rect)
{
	memset(rect, 0, sizeof(UIRect));
}

BOOL intersect_rect(UIRect *result, UIRect rect1, UIRect rect2)
{
	if (!result)
	{
		return FALSE;
	}
	set_rect_left(result, MAX(rect_left(rect1), rect_left(rect2)));
	set_rect_top(result, MAX(rect_top(rect1), rect_top(rect2)));
	set_rect_right(result, MIN(rect_right(rect1), rect_right(rect2)));
	set_rect_bottom(result, MIN(rect_bottom(rect1), rect_bottom(rect2)));

	return !is_empty_rect(*result);
}

BOOL is_intersect(UIRect rect1, UIRect rect2)
{
    int left, top, right, bottom;

	left   = MAX(rect_left(rect1), rect_left(rect2));
	top    = MAX(rect_top(rect1), rect_top(rect2));
	right  = MIN(rect_right(rect1), rect_right(rect2));
	bottom = MIN(rect_bottom(rect1), rect_bottom(rect2));

    return ((left <= right) && (top <= bottom));
}

UIRect offset_rect(UIRect rect, int dx, int dy)
{
	UIRect rt = {{rect.point.x + dx, rect.point.y + dy}, {rect.size.width, rect.size.height}};

	return rt;
}

UIRect inflate_rect(UIRect rect, int dx, int dy)
{
	UIRect rt = {{rect.point.x - dx, rect.point.y - dy}, {rect.size.width + dx + dx, rect.size.height + dy + dy}};

	return rt;
}

BOOL point_in_rect(UIPoint point, UIRect rect)
{
	return (point.x >= rect.point.x && point.x <= rect.point.x + rect.size.width)
			&& (point.y >= rect.point.y && point.y <= rect.point.y + rect.size.height);
}

BOOL rect_in_rect(UIRect small1, UIRect big1)
{
	if (big1.point.x > small1.point.x || big1.point.y > small1.point.y)
	{
		return FALSE;
	}
	if (big1.point.x + big1.size.width < small1.point.x + small1.size.width || big1.point.y + big1.size.height < small1.point.y + small1.size.height)
	{
		return FALSE;
	}
	return TRUE;
}

UIPoint ui_center_point(UIRect container, UISize object)
{
	UIPoint point;

	point.x = container.point.x + (container.size.width - object.width) / 2;
	point.y = container.point.y + (container.size.height - object.height) / 2;

	return point;
}

char* rect_to_string(UIRect rect)
{
	static char str[64];

	gui_snprintf(str, sizeof(str), "[(%4d,%4d) - (%4d,%4d)]", rect.point.x, rect.point.y, rect.size.width, rect.size.height);
	return str;
}


//-------------------------------------------------------------------------------------------------
// rect util
//-------------------------------------------------------------------------------------------------
UIRect rect_align(UISize object, UIRect container, int align_style)
{
	UIPoint point = container.point;

	if (align_style & UI_ALIGN_CENTER_HOR)
		point.x = container.point.x + (container.size.width - object.width) / 2;
	else if (align_style & UI_ALIGN_RIGHT)
		point.x = rect_right(container) - object.width;

	if (align_style & UI_ALIGN_CENTER_VER)
		point.y = container.point.y + (container.size.height - object.height) / 2;
	else if (align_style & UI_ALIGN_BOTTOM)
		point.y = rect_bottom(container) - object.height;

	return ui_rect(point.x, point.y, object.width, object.height);
}


//-------------------------------------------------------------------------------------------------
// inset
//-------------------------------------------------------------------------------------------------
UIInset ui_inset(int left, int top, int right, int bottom)
{
	UIInset inset = {left, top, right, bottom};

	return inset;
}

BOOL is_same_inset(UIInset a, UIInset b)
{
	return a.left == b.left && a.top == b.top && a.right == b.right && a.bottom == b.bottom;
}

UIRect rect_intersect_inset(UIRect rect, UIInset inset)
{
	return ui_rect(inset.left, inset.top, rect.size.width - inset.left - inset.right, rect.size.height - inset.top - inset.bottom);
}

UISize rect_intersect_inset_size(UIRect rect, UIInset inset)
{
	return ui_size(rect.size.width - inset.left - inset.right, rect.size.height - inset.top - inset.bottom);
}

UISize inset_delta(UIInset a, UIInset b)
{
	return ui_size((b.left + b.right) - (a.left + a.right), (b.top + b.bottom) - (a.top + a.bottom));
}
