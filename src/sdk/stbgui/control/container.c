/*
 * container.c
 *
 *  Created on: 2019-7-5
 *      Author: Brant
 */

#include <gui_window.h>
#include <control/container.h>


//-------------------------------------------------------------------------------------------------
// internal use
//-------------------------------------------------------------------------------------------------
static BOOL is_valid_container_window(UIContainer *container)
{
	if (container == NULL || container->private_data == NULL)
		return FALSE;

	if (!is_kind_of_window(container, CONTAINER_CLASS_NAME))
		return FALSE;

	return TRUE;
}

static void calculate_rect_bound(UIRect *bound, UIRect new_rect)
{
	if (rect_left(new_rect) < rect_left(*bound))
	{
		set_rect_left(bound, rect_left(new_rect));
	}
	if (rect_top(new_rect) < rect_top(*bound))
	{
		set_rect_top(bound, rect_top(new_rect));
	}
	if (rect_right(new_rect) > rect_right(*bound))
	{
		set_rect_right(bound, rect_right(new_rect));
	}
	if (rect_bottom(new_rect) > rect_bottom(*bound))
	{
		set_rect_bottom(bound, rect_bottom(new_rect));
	}
}

static UIRect get_sub_window_bound(UIContainer *container)
{
	UIWindow *child;
	UIRect rect = EMPTY_RECT;
	UIRect win_rect;

	child = container->sub_list;
	while (child)
	{
		win_rect.point = child->origin;
		win_rect.size = child->rect.size;
		calculate_rect_bound(&rect, win_rect);
		child = child->next;
	}
	child = container->control_list;
	while (child)
	{
		win_rect.point = child->origin;
		win_rect.size = child->rect.size;
		calculate_rect_bound(&rect, win_rect);
		child = child->next;
	}

	return rect;
}


//-------------------------------------------------------------------------------------------------
// window manager callback
//-------------------------------------------------------------------------------------------------
static BOOL container_on_destroy(UIContainer *self)
{
	UIContainerData *cd;

	if (self == NULL || self->private_data == NULL)
		return TRUE;

	cd = (UIContainerData *)self->private_data;
	delete_container_data(cd);

	gui_free(cd);

	return TRUE;
}


//-------------------------------------------------------------------------------------------------
// interface
//-------------------------------------------------------------------------------------------------
void default_container_data(UIContainerData *container_data)
{
	gui_memset(container_data, 0, sizeof(UIContainerData));

	container_data->auto_size = FALSE;
}

void delete_container_data(UIContainerData *container_data)
{
	if (container_data == NULL)
		return;

	if (container_data->sizer)
		destroy_sizer(container_data->sizer);
}


UIContainer* create_container(const char *id)
{
	UIWindow *container = create_sub_window(id, 0, 0, 200, 200);

	if (container)
	{
		UIContainerData *cd = (UIContainerData *)gui_malloc(sizeof(UIContainerData));

		if (!cd)
		{
			destroy_window(container);
			return NULL;
		}

		default_container_data(cd);
		container->private_data = cd;
		container->focus_stop = TRUE;
		container->class_name = CONTAINER_CLASS_NAME;
		container->on_destroy = container_on_destroy;
	}
	return container;
}

int set_container_inset(UIContainer *container, UIInset inset)
{
	UIContainerData *cd;

	if (!is_valid_container_window(container))
		return -1;

	cd = (UIContainerData *)container->private_data;
	if (is_same_inset(cd->inset, inset))
		return 0;

	cd->inset = inset;

	if (cd->auto_size)
		adjust_container_size(container);

	if (cd->sizer != NULL)
		container_layout_sub_window(container);
	else
		update_sub_window_position(container);

	return 0;
}

int set_container_sizer(UIContainer *container, UISizer *sizer)
{
	UIContainerData *cd;

	if (!is_valid_container_window(container))
		return -1;

	cd = (UIContainerData *)container->private_data;
	if (cd->sizer == sizer)		/* nothing to do */
		return 0;

	cd->sizer = sizer;
	if (cd->auto_size)		/* adjust_container_size will call container_layout_sub_window */
		adjust_container_size(container);
	else
		container_layout_sub_window(container);

	return 0;
}

int set_container_auto_size(UIContainer *container, BOOL auto_size)
{
	UIContainerData *cd;

	if (!is_valid_container_window(container))
		return -1;

	cd = (UIContainerData *)container->private_data;
	if (cd->auto_size == auto_size)
		return 0;

	cd->auto_size = auto_size;
	if (cd->auto_size)
		adjust_container_size(container);

	container_layout_sub_window(container);

	return 0;
}

int container_add_sub_window(UIContainer *container, UIWindow *sub_win)
{
	UIContainerData *cd;

	if (!is_valid_container_window(container))
		return -1;

	add_sub_window(container, sub_win);

	cd = (UIContainerData *)container->private_data;
	if (cd->auto_size)
		adjust_container_size(container);

	return 0;
}

int adjust_container_size(UIContainer *container)
{
	UIContainerData *cd;
	UIRect content_rect = EMPTY_RECT;
	UISize size;
	UIInset inset;

	if (!is_valid_container_window(container))
		return -1;

	cd = (UIContainerData *)container->private_data;
	if (!(cd->auto_size))
		return 0;

	inset = cd->inset;
	if (cd->sizer)
	{
		content_rect.point = ui_point(0, 0);
		content_rect.size = get_sizer_min_size(cd->sizer);
	}
	else
		content_rect = get_sub_window_bound(container);

	content_rect.size.width = MAX(container->min_size.width, content_rect.size.width);
	content_rect.size.height = MAX(container->min_size.width, content_rect.size.height);

	size.width = content_rect.point.x + content_rect.size.width + inset.left + inset.right;
	size.height = content_rect.point.y + content_rect.size.height + inset.top + inset.bottom;

	set_window_size(container, size);

	return 0;
}

int container_layout_sub_window(UIContainer *container)
{
	UIContainerData *cd;

	if (!is_valid_container_window(container))
		return -1;

	cd = (UIContainerData *)container->private_data;
	if (cd->sizer)
	{
		UIRect sizer_rect;

		sizer_rect.point = ui_point(0, 0);
		sizer_rect.size = rect_intersect_inset_size(container->rect, cd->inset);
		sizer_set_dimension(cd->sizer, sizer_rect);
	}

	return 0;
}
