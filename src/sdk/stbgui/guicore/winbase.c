/*
 * winbase.c
 *
 *  Created on: 2015-6-16
 *      Author: Brant
 */

#include <gui_common.h>
#include <guicore/event.h>
#include <guicore/window.h>


//-------------------------------------------------------------------------------------------------
// window style
//-------------------------------------------------------------------------------------------------
BOOL win_is_focusable(const UIWindow *win)
{
	if (win == NULL) return FALSE;

	return win->focus_stop;
}

BOOL win_is_main(const UIWindow *win)
{
	if (win && win->type == WINDOW_TYPE_MAIN)
	{
		return TRUE;
	}
	return FALSE;
}

BOOL win_is_sub(const UIWindow *win)
{
	if (win && win->type == WINDOW_TYPE_SUB)
	{
		return TRUE;
	}
	return FALSE;
}

BOOL win_is_control(const UIWindow *win)
{
	if (win && win->type == WINDOW_TYPE_CONTROL)
	{
		return TRUE;
	}
	return FALSE;
}


//-------------------------------------------------------------------------------------------------
// window status
//-------------------------------------------------------------------------------------------------
BOOL win_is_visible(const UIWindow *win)
{
	if (win == NULL)
		return FALSE;

	return win->visible;
}

BOOL win_is_drawable(const UIWindow *win)
{
	const UIWindow *parent;

	if (!win)
	{
		return FALSE;
	}
	parent = win;
	while (parent)
	{
		if (!parent->visible)
		{
			return FALSE;
		}
		parent = parent->parent;
	}
	return TRUE;
}

BOOL win_has_trans(const UIWindow *win)
{
	if (!win) return FALSE;

	return win->has_trans;
}

static void set_cr_changed_mark(UIWindow *win)
{
	if (win) win->clip_rgn_change = TRUE;
}

static void clear_cr_changed_mark(UIWindow *win)
{
	if (win) win->clip_rgn_change = FALSE;
}


//-------------------------------------------------------------------------------------------------
// window region
//-------------------------------------------------------------------------------------------------
BOOL win_init_region(UIWindow *win)
{
	if (!win) return FALSE;

	/* clip region */
	init_clip_region(&(win->clip_region));
	win->clip_rgn_change = TRUE;

	/* invalidate region */
	init_invalid_region(&(win->inv_region));
	add_rect_invalid_region(&(win->inv_region), win->rect);

	/* painter */
	win->painter.canvas = get_main_canvas();
	win->painter.offset = ui_point(0, 0);
	win->painter.mode = UI_DRAW_MODE_COVER;
	init_clip_region(&(win->painter.clip_region));

	return TRUE;
}


static void win_empty_region_act(UIWindow *win)
{
	empty_clip_region(&(win->clip_region));
	empty_invalid_region(&(win->inv_region));
	empty_clip_region(&(win->painter.clip_region));
}

BOOL win_empty_region(UIWindow *win)
{
	UIWindow *sub, *control;

	if (win == NULL) return FALSE;

	sub = win->sub_list;
	while (sub)
	{
		control = sub->control_list;
		while (control)
		{
			win_empty_region_act(control);
			control = control->next;
		}
		win_empty_region_act(sub);
		sub = sub->next;
	}

	control = win->control_list;
	while (control)
	{
		win_empty_region_act(control);
		control = control->next;
	}

	win_empty_region_act(win);

	return TRUE;
}


//-------------------------------------------------------------------------------------------------
// calculate clip region
//-------------------------------------------------------------------------------------------------
static void calculate_clip_region_main(UIWindow *win)
{
	UIWindow *child;
	UIWindow *control;
	UIRect rect;

	/* initial clip region */
	empty_clip_region(&(win->clip_region));
	if (!win_is_visible(win))
	{
		return;
	}
	set_init_rect_clip_region(&(win->clip_region), win->rect);

	/* clipped by its sub window */
	child = win->sub_list;
	while (child)
	{
		if (win_is_visible(child))
		{
			intersect_rect(&rect, child->rect, win->rect);
			subtract_clip_region(&(win->clip_region), rect);
		}
		child = child->next;
	}
	/* clipped by its control */
	control = win->control_list;
	while (control)
	{
		if (win_is_visible(control))
		{
			intersect_rect(&rect, control->rect, win->rect);
			subtract_clip_region(&(win->clip_region), rect);
		}
		control = control->next;
	}
}

static void calculate_clip_region_sub(UIWindow *win)
{
	UIWindow *brother;
	UIWindow *control;
	UIRect rect;

	/* initial clip region */
	empty_clip_region(&(win->clip_region));
	if (!win->parent || !win_is_drawable(win))
	{
		return;
	}
	set_init_rect_clip_region(&(win->clip_region), win->rect);

	/* never draw out of parent */
	intersect_clip_region(&(win->clip_region), win->parent->rect);

	/* clipped by brother which Z order is bigger than its */
	brother = win->parent->sub_list;
	while (brother && brother != win)
	{
		if (win_is_drawable(brother))
		{
			subtract_clip_region(&(win->clip_region), brother->rect);
		}
		brother = brother->next;
	}
	/* clipped by control */
	control = win->control_list;
	while (control)
	{
		if (win_is_drawable(control))
		{
			intersect_rect(&rect, control->rect, win->rect);
			subtract_clip_region(&(win->clip_region), rect);
		}
		control = control->next;
	}
}

static void calculate_clip_region_control(UIWindow *win)
{
	UIWindow *child;
	UIWindow *control;
	UIWindow *parent;
	UIWindow *uncle;

	empty_clip_region(&(win->clip_region));
	if (!win->parent || !win_is_drawable(win))
	{
		return;
	}
	set_init_rect_clip_region(&(win->clip_region), win->rect);

	/* never draw out of parent */
	intersect_clip_region(&(win->clip_region), win->parent->rect);
	/* clipped by all sub window */
	if (win_is_main(win->parent))
	{
		child = win->parent->sub_list;
		while (child)
		{
			if (win_is_drawable(child))
			{
				subtract_clip_region(&(win->clip_region), child->rect);
			}
			child = child->next;
		}
	}
	if (win_is_sub(win->parent))
	{
		parent = win->parent;
		uncle = win->parent->parent->sub_list;
		while (uncle && uncle != parent)
		{
			if (win_is_drawable(uncle))
			{
				subtract_clip_region(&(win->clip_region), uncle->rect);
			}
			uncle = uncle->next;
		}
	}
	/* clipped by brother which Z order is bigger than its */
	control = win->parent->control_list;
	while (control && control != win)
	{
		if (win_is_drawable(control))
		{
			subtract_clip_region(&(win->clip_region), control->rect);
		}
		control = control->next;
	}
}

static void calculate_clip_region(UIWindow *win)
{
	if (!win || !win->clip_rgn_change)
	{
		return;
	}
	switch (win->type)
	{
		case WINDOW_TYPE_MAIN:
			calculate_clip_region_main(win);
			break;
		case WINDOW_TYPE_SUB:
			calculate_clip_region_sub(win);
			break;
		case WINDOW_TYPE_CONTROL:
			calculate_clip_region_control(win);
			break;
	}
	CLIP_LOG("==== Clip Region(%x): ====\n", win);
#if ENABLE_DEBUG_CLIP_RGN
	print_clip_region(&win->clip_region);
#endif

	clear_cr_changed_mark(win);
}

static void calculate_sub_win_clip_region(UIWindow *win)
{
	UIWindow *child;
	UIWindow *control;

	calculate_clip_region(win);
	child = win->sub_list;
	while (child)
	{
		calculate_clip_region(child);
		control = child->control_list;
		while (control)
		{
			calculate_clip_region(control);
			control = control->next;
		}
		child = child->next;
	}
	control = win->control_list;
	while (control)
	{
		calculate_clip_region(control);
		control = control->next;
	}
}


//-------------------------------------------------------------------------------------------------
// mark clip region change
//-------------------------------------------------------------------------------------------------
static void mark_clip_region_change_main(UIWindow *win)
{
	UIWindow *sub;
	UIWindow *control;

	/* all sub window need change */
	sub = win->sub_list;
	while (sub)
	{
		/* control in sub window need change */
		control = sub->control_list;
		while (control)
		{
			set_cr_changed_mark(control);
			control = control->next;
		}
		set_cr_changed_mark(sub);
		sub = sub->next;
	}

	/* control in main window need change */
	control = win->control_list;
	while (control)
	{
		set_cr_changed_mark(control);
		control = control->next;
	}

	/* self change */
	set_cr_changed_mark(win);
}

static void mark_clip_region_change_sub(UIWindow *win)
{
	UIWindow *brother;
	UIWindow *control;

	/* sub change will make parent change */
	set_cr_changed_mark(win->parent);

	/* make brother which Z order less than its change */
	brother = win->next;
	while (brother)
	{
		set_cr_changed_mark(brother);
		/* make control which in Z order less than its brother change */
		if (win_is_sub(brother))
		{
			control = brother->control_list;
			while (control)
			{
				set_cr_changed_mark(control);
				control = control->next;
			}
		}
		brother = brother->next;
	}

	/* make control contained in parent change */
	if (win->parent)
	{
		control = win->parent->control_list;
		while (control)
		{
			set_cr_changed_mark(control);
			control = control->next;
		}
	}

	/* self change */
	set_cr_changed_mark(win);

	/* self control change */
	control = win->control_list;
	while (control)
	{
		set_cr_changed_mark(control);
		control = control->next;
	}
}

static void mark_clip_region_change_control(UIWindow *win)
{
	UIWindow *brother;

	/* control change will make parent change */
	set_cr_changed_mark(win->parent);

	/* make brother which Z order less than its change */
	brother = win->next;
	while (brother)
	{
		set_cr_changed_mark(brother);
		brother = brother->next;
	}

	/* self change */
	set_cr_changed_mark(win);
}

void mark_clip_region_change(UIWindow *win)
{
	if (!win)
	{
		return;
	}
	switch (win->type)
	{
		case WINDOW_TYPE_MAIN:
			mark_clip_region_change_main(win);
			break;
		case WINDOW_TYPE_SUB:
			mark_clip_region_change_sub(win);
			break;
		case WINDOW_TYPE_CONTROL:
			mark_clip_region_change_control(win);
			break;
	}
}


//-------------------------------------------------------------------------------------------------
// draw window
//-------------------------------------------------------------------------------------------------
static void draw_window_ex(UIWindow *win, UIClipRegion *clip_region);
static void scr_invalidate_clip_region(UIWindow *win, UIClipRegion *clip_region)
{
	UIClipRegion cr;
	UIRect win_rect = win->rect;
	UIWindow *parent;

	parent = win->parent;
	while (parent)
	{
		intersect_rect(&win_rect, parent->rect, win_rect);
		parent = parent->parent;
	}
	init_clip_region(&cr);
	copy_clip_region(&cr, clip_region);
	intersect_clip_region(&cr, win_rect);
	draw_window_ex(win, &cr);
	/* if already refreshed current area, don't pass to lower level window */
	subtract_by_clip_region(clip_region, &cr);
	empty_clip_region(&cr);
}

/* refresh covered window by painter of win, from high level to low */
static void refresh_covered_window(UIWindow *win)
{
	UIClipRegion clip_region;
	UIWindow *brother;
	UIWindow *control;

	init_clip_region(&clip_region);
	copy_clip_region(&clip_region, &(win->painter.clip_region));

	/* 1.it's brother which Z order is small than its */
	brother = win->next;
	while (brother)
	{
		/* controls of this window's brother should be repainted */
		if (win_is_sub(brother))
		{
			control = brother->control_list;
			while (control)
			{
				if (win_is_drawable(control) && is_intersect(clip_region.bound, control->rect))
				{
					scr_invalidate_clip_region(control, &clip_region);
				}
				control = control->next;
			}
		}
		/* brother */
		if (win_is_drawable(brother) && is_intersect(clip_region.bound, brother->rect))
		{
			scr_invalidate_clip_region(brother, &clip_region);
		}
		brother = brother->next;
	}

	/* 2.parent window */
	if (win_is_drawable(win->parent) && is_intersect(clip_region.bound, win->parent->rect))
	{
		scr_invalidate_clip_region(win->parent, &clip_region);
	}

	/* 3.if this is a sub window, we should repaint the controls of its parent */
	if (win_is_sub(win))
	{
		control = win->parent->control_list;
		while (control)
		{
			if (win_is_drawable(brother) && is_intersect(clip_region.bound, control->rect))
			{
				scr_invalidate_clip_region(control, &clip_region);
			}
			control = control->next;
		}
	}

	empty_clip_region(&clip_region);
}

static void refresh_background(UIWindow *win)
{
	if (win == NULL)
	{
		return;
	}
	if (win && win->has_trans)
	{
		refresh_covered_window(win);
	}
	if (win->on_erase_bkg)
	{
		win->on_erase_bkg(win);
	}
}

static BOOL window_will_draw(UIWindow *win, UIClipRegion *clip_region)
{
	/* use other object's clip region */
	if (clip_region)
	{
		copy_clip_region(&(win->painter.clip_region), clip_region);
	}
	else
	{
		if (is_empty_invalid_region(&(win->inv_region)))
		{
			return FALSE;
		}
		if (!is_intersect(win->clip_region.bound, win->inv_region.bound))
		{
			return FALSE;
		}
		/* calculate new clip region according to invalid region */
		if (!is_empty_clip_region(&(win->painter.clip_region)))
		{
			empty_clip_region(&(win->painter.clip_region));
		}
		clip_region_intersect_inv_region(&(win->painter.clip_region), &(win->clip_region), &(win->inv_region));
	}

	win->painter.offset = win->rect.point;
	/* do erase background now */
	refresh_background(win);

	return TRUE;
}

/*
 * if clip_region != NULL, it means high level window has trans, need use its clip region refresh background.
 * can't delete invalid region in this situation, high level window need it to draw itself.
 */
static BOOL window_did_draw(UIWindow *win, UIClipRegion *clip_region)
{
	/* clear painter clip region */
	if (!is_empty_clip_region(&(win->painter.clip_region)))
	{
		empty_clip_region(&(win->painter.clip_region));
	}
	/* clear invalidate area */
	if (!clip_region)
	{
		empty_invalid_region(&(win->inv_region));
	}

	return TRUE;
}

static void draw_window_ex(UIWindow *win, UIClipRegion *clip_region)
{
	BOOL need_draw;

	need_draw = window_will_draw(win, clip_region);
	if (need_draw && win->on_draw != NULL)
	{
		win->on_draw(win);
	}
	window_did_draw(win, clip_region);
}

static void draw_window(UIWindow *win)
{
	draw_window_ex(win, NULL);
}


//-------------------------------------------------------------------------------------------------
// position transform
//-------------------------------------------------------------------------------------------------
UIRect client_rect_to_win_rect(UIWindow *win, UIRect client_rect)
{
	if (win == NULL) return client_rect;

	return offset_rect(client_rect, win->rect.point.x, win->rect.point.y);
}


//-------------------------------------------------------------------------------------------------
// invalidate window
//-------------------------------------------------------------------------------------------------
static BOOL set_window_invalid_region(UIWindow *window, UIInvalidRegion *inv_region)
{
	UIInvalidRect *node;
	UIRect result;

	if (!window || !inv_region)
		return FALSE;

	if (!is_intersect(window->rect, inv_region->bound))
		return TRUE;

	node = inv_region->head;
	while (node)
	{
		if (intersect_rect(&result, window->rect, node->rect/*inv_region->bound*/))
		{
			INV_LOG("Inv(%x): %s\n", win, rect_to_string(result));
			add_rect_invalid_region(&window->inv_region, result);
		}
		node = node->next;
	}

	return TRUE;
}

static BOOL invalidate_low_level_window(UIWindow *window, UIInvalidRegion *inv_region);
/* invalidate window which lower than its, from high level to low */
static BOOL invalidate_low_level_window_sub(UIWindow *win, UIInvalidRegion *inv_region)
{
	UIWindow *brother, *control;

	/* brother which Z order less than its */
	brother = win->next;
	while (brother)
	{
		if (win_is_visible(brother))
		{
			if (win_is_sub(brother))
			{
				control = brother->control_list;
				while (control)
				{
					if (win_is_visible(control))
					{
						invalidate_low_level_window(control, inv_region);
					}
					control = control->next;
				}
			}
			invalidate_low_level_window(brother, inv_region);
		}
		brother = brother->next;
	}
	/* parent's control */
	if (win->parent)
	{
		control = win->parent->control_list;
		while (control)
		{
			if (win_is_visible(control))
			{
				invalidate_low_level_window(control, inv_region);
			}
			control = control->next;
		}
	}
	/* parent */
	if (win_is_visible(win->parent))
	{
		invalidate_low_level_window(win->parent, inv_region);
	}

	return TRUE;
}

static BOOL invalidate_low_level_window_control(UIWindow *win, UIInvalidRegion *inv_region)
{
	UIWindow *brother;

	/* brother which Z order less than its */
	brother = win->next;
	while (brother)
	{
		if (win_is_visible(brother))
		{
			invalidate_low_level_window(brother, inv_region);
		}
		brother = brother->next;
	}
	/* parent */
	if (win_is_visible(win->parent))
	{
		invalidate_low_level_window(win->parent, inv_region);
	}

	return TRUE;
}

static BOOL invalidate_low_level_window(UIWindow *window, UIInvalidRegion *inv_region)
{
	if (window == NULL || inv_region == NULL)
		return FALSE;

	if (is_empty_invalid_region(inv_region))
		return TRUE;

	set_window_invalid_region(window, inv_region);

	if (win_is_visible(window))
		subtract_invalid_region(inv_region, window->rect);

	if (is_empty_invalid_region(inv_region))
		return TRUE;

	/* have something need set to lower level window */
	switch (window->type)
	{
		case WINDOW_TYPE_SUB:
			return invalidate_low_level_window_sub(window, inv_region);
		case WINDOW_TYPE_CONTROL:
			return invalidate_low_level_window_control(window, inv_region);
		default:
			/* main window is lowest level window, do nothing */
			break;
	}
	return TRUE;
}

static BOOL invalidate_high_level_window_main(UIWindow *win, UIInvalidRegion *inv_region, BOOL force)
{
	UIWindow *child, *control;

	/* invalidate its control */
	control = win->control_list;
	while (control)
	{
		if (win_is_visible(control))
		{
			if (force || win_has_trans(control))
			{
				set_window_invalid_region(control, inv_region);
			}
			else
			{
				subtract_invalid_region(inv_region, control->rect);
			}
		}
		control = control->next;
	}

	/* invalidate its sub window */
	child = win->sub_list;
	while (child)
	{
		if (win_is_visible(child))
		{
			if (force || win_has_trans(child))
			{
				set_window_invalid_region(child, inv_region);
			}
			else
			{
				subtract_invalid_region(inv_region, child->rect);
			}
			/* controls on sub window */
			control = child->control_list;
			while (control)
			{
				if (win_is_drawable(control))
				{
					if (force || win_has_trans(control))
					{
						set_window_invalid_region(control, inv_region);
					}
					else
					{
						subtract_invalid_region(inv_region, control->rect);
					}
				}
				control = control->next;
			}
		}
		child = child->next;
	}

	return TRUE;
}

/* force == TRUE means it from hide status to show, need invalidate all child in it */
static BOOL invalidate_high_level_window_sub(UIWindow *win, UIInvalidRegion *inv_region, BOOL force)
{
	UIWindow *brother, *control;

	/* invalidate control */
	control = win->control_list;
	while (control)
	{
		if (win_is_visible(control))
		{
			if (force || win_has_trans(control))
			{
				set_window_invalid_region(control, inv_region);
			}
			else
			{
				subtract_invalid_region(inv_region, control->rect);
			}
		}
		control = control->next;
	}

	/* invalidate brother which Z order is bigger than its */
	if (win->parent)
	{
		brother = win->parent->sub_list;
		while (brother && brother != win)
		{
			if (win_is_visible(brother))
			{
				if (win_has_trans(brother))
				{
					set_window_invalid_region(brother, inv_region);
				}
				else
				{
					subtract_invalid_region(inv_region, brother->rect);
				}
				if (win_is_sub(brother))
				{
					control = brother->control_list;
					while (control)
					{
						if (win_is_visible(control))
						{
							if (win_has_trans(control))
							{
								set_window_invalid_region(control, inv_region);
							}
							else
							{
								subtract_invalid_region(inv_region, control->rect);
							}
						}
						control = control->next;
					}
				}
			}
			brother = brother->next;
		}
	}

	return TRUE;
}

static BOOL invalidate_high_level_window_control(UIWindow *win, UIInvalidRegion *inv_region, BOOL force)
{
	UIWindow *brother;
	UIWindow *parent;
	UIWindow *uncle;
	UIWindow *control;

	/* invalidate all sub window */
	if (win_is_main(win->parent))
	{
		if (win->parent)
		{
			brother = win->parent->sub_list;
			while (brother)
			{
				if (win_is_visible(brother))
				{
					if (win_has_trans(brother))
					{
						set_window_invalid_region(brother, inv_region);
					}
					else
					{
						subtract_invalid_region(inv_region, brother->rect);
					}
					/* invalidate control on sub window */
					control = brother->control_list;
					while (control)
					{
						if (win_is_visible(control))
						{
							if (win_has_trans(control))
							{
								set_window_invalid_region(control, inv_region);
							}
							else
							{
								subtract_invalid_region(inv_region, control->rect);
							}
						}
						control = control->next;
					}
				}
				brother = brother->next;
			}
		}
	}
	else if (win_is_sub(win->parent))
	{
		parent = win->parent;
		if (parent && parent->parent)
		{
			uncle = parent->parent->sub_list;
			while (uncle && uncle != parent)
			{
				if (win_is_visible(uncle))
				{
					if (win_has_trans(uncle))
					{
						set_window_invalid_region(uncle, inv_region);
					}
					else
					{
						subtract_invalid_region(inv_region, uncle->rect);
					}
					/* invalidate control on uncle window */
					control = uncle->control_list;
					while (control)
					{
						if (win_is_visible(control))
						{
							if (win_has_trans(control))
							{
								set_window_invalid_region(control, inv_region);
							}
							else
							{
								subtract_invalid_region(inv_region, control->rect);
							}
						}
						control = control->next;
					}

				}
				uncle = uncle->next;
			}
		}
	}
	/* invalidate brother which Z order is bigger than its */
	if (win->parent)
	{
		brother = win->parent->control_list;
		while (brother && brother != win)
		{
			if (win_is_visible(brother))
			{
				if (win_has_trans(brother))
				{
					set_window_invalid_region(brother, inv_region);
				}
				else
				{
					subtract_invalid_region(inv_region, brother->rect);
				}
			}
			brother = brother->next;
		}
	}

	return TRUE;
}

static BOOL invalidate_high_level_window(UIWindow *win, UIInvalidRegion *inv_region, BOOL force)
{
	if (win == NULL || inv_region == NULL)
		return FALSE;

	switch (win->type)
	{
		case WINDOW_TYPE_MAIN:
			return invalidate_high_level_window_main(win, inv_region, force);
		case WINDOW_TYPE_SUB:
			return invalidate_high_level_window_sub(win, inv_region, force);
		case WINDOW_TYPE_CONTROL:
			return invalidate_high_level_window_control(win, inv_region, force);
		default:
			break;
	}

	return TRUE;
}

BOOL invalidate_window_with_region(UIWindow *window, UIInvalidRegion *region, BOOL force)
{
	UIInvalidRegion save_inv_rgn;

	if (window == NULL || !(force || win_is_drawable(window)))
		return FALSE;

	init_invalid_region(&save_inv_rgn);
	copy_invalid_region(&save_inv_rgn, region);
	/* invalidate low level window when invalid region out of win's region */
	invalidate_low_level_window(window, region);
	/* invalidate high level window if it has trans */
	invalidate_high_level_window(window, &save_inv_rgn, force);
	empty_invalid_region(&save_inv_rgn);

	return TRUE;
}

BOOL invalidate_window_rect(UIWindow *window, UIRect rect, BOOL force)
{
	UIInvalidRegion inv_region;

	if (window == NULL || !(force || win_is_drawable(window)))
		return FALSE;

	init_invalid_region(&inv_region);
	add_rect_invalid_region(&inv_region, rect);
	invalidate_window_with_region(window, &inv_region, force);
	empty_invalid_region(&inv_region);

	return TRUE;
}

BOOL invalidate_window_ex(UIWindow *window, BOOL force)
{
	if (window == NULL)
		return FALSE;

	return invalidate_window_rect(window, window->rect, force);
}

BOOL invalidate_window(UIWindow *window)
{
	return invalidate_window_ex(window, FALSE);
}


//-------------------------------------------------------------------------------------------------
// flush window
//-------------------------------------------------------------------------------------------------
/* draw all window which has invalid region, from high level to low */
static void flush_main_window(UIWindow *win)
{
	UIWindow *child;
	UIWindow *control;

	calculate_sub_win_clip_region(win);

	child = win->sub_list;
	while (child)
	{
		control = child->control_list;
		while (control)
		{
			draw_window(control);
			control = control->next;
		}
		draw_window(child);
		child = child->next;
	}
	control = win->control_list;
	while (control)
	{
		draw_window(control);
		control = control->next;
	}
	draw_window(win);
}

/* draw to screen, include it's sub window and control */
void flush_window(UIWindow *win)
{
	if (win->type != WINDOW_TYPE_MAIN)
	{
		GUI_WARN("warning: you are flushing a sub window or control.\n");
		return;
	}

	flush_main_window(win);
//	BitBlitOSD();
//	flush_osd();

#if ENABLE_DEBUG_CLIP_RGN
	print_clip_region_use();
#endif
#if ENABLE_DEBUG_INV_RGN
	print_invalid_region_use();
#endif
}
