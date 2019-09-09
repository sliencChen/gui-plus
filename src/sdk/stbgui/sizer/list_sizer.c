/*
 * list_sizer.c
 *
 *  Created on: 2019-7-3
 *      Author: Brant
 */

#include <gui_window.h>
#include <sizer/list_sizer.h>


//-------------------------------------------------------------------------------------------------
// internal use
//-------------------------------------------------------------------------------------------------
static BOOL is_vertical(UIListSizer *list_sizer)
{
	UIListSizerData *bsd = (UIListSizerData *)list_sizer->private_data;

	return bsd->dir == UI_SIZER_DIR_VER;
}

static int get_size_in_major_dir(UIListSizer *list_sizer, UISize size)
{
	UIListSizerData *bsd = (UIListSizerData *)list_sizer->private_data;

	if (bsd->dir == UI_SIZER_DIR_HOR)
		return size.width;

	return size.height;
}

static int* size_in_major_dir(UIListSizer *list_sizer, UISize *size)
{
	UIListSizerData *bsd = (UIListSizerData *)list_sizer->private_data;

	if (bsd->dir == UI_SIZER_DIR_HOR)
		return &(size->width);

	return &(size->height);
}

static int get_size_in_minor_dir(UIListSizer *list_sizer, UISize size)
{
	UIListSizerData *bsd = (UIListSizerData *)list_sizer->private_data;

	if (bsd->dir == UI_SIZER_DIR_HOR)
		return size.height;

	return size.width;
}

static int* size_in_minor_dir(UIListSizer *list_sizer, UISize *size)
{
	UIListSizerData *bsd = (UIListSizerData *)list_sizer->private_data;

	if (bsd->dir == UI_SIZER_DIR_HOR)
		return &(size->height);

	return &(size->width);
}

static int* pos_in_major_dir(UIListSizer *list_sizer, UIPoint *point)
{
	UIListSizerData *bsd = (UIListSizerData *)list_sizer->private_data;

	if (bsd->dir == UI_SIZER_DIR_HOR)
		return &(point->x);

	return &(point->y);
}

static int* pos_in_minor_dir(UIListSizer *list_sizer, UIPoint *point)
{
	UIListSizerData *bsd = (UIListSizerData *)list_sizer->private_data;

	if (bsd->dir == UI_SIZER_DIR_HOR)
		return &(point->y);

	return &(point->x);
}

static UISize SizeFromMajorMinor(UIListSizer *list_sizer, int major, int minor)
{
	UIListSizerData *bsd = (UIListSizerData *)list_sizer->private_data;

	if (bsd->dir == UI_SIZER_DIR_HOR)
	{
		return ui_size(major, minor);
	}
	else // UI_SIZER_DIR_VER
	{
		return ui_size(minor, major);
	}
}


//-------------------------------------------------------------------------------------------------
// sizer call back
//-------------------------------------------------------------------------------------------------
static UISize calc_list_sizer_min_size(UIListSizer *list_sizer)
{
	UIListSizerData *bsd = (UIListSizerData *)list_sizer->private_data;
	UISize min_size = {0, 0};
	UISize min_size_this;
	UISizerItemNode *node;
	UISizerItem *item;
	int prop_this;
	float min_size_to_prop, max_min_size_to_prop;

	// The minimal size for the sizer should be big enough to allocate its
	// element at least its minimal size but also, and this is the non trivial
	// part, to respect the children proportion. To satisfy the latter
	// condition we must find the greatest min-size-to-proportion ratio for all
	// elements with non-zero proportion.
	bsd->total_proportion = 0;
	max_min_size_to_prop = 0;

	node = list_sizer->items;
	while (node)
	{
		item = &(node->item);
		if (!sizer_item_shown(item))
        {
            node = node->next;
            continue;
        }
		min_size_this = sizer_item_calc_min_size(item);

        prop_this = 1;//item->proportion;
		if (prop_this)
		{
			min_size_to_prop = get_size_in_major_dir(list_sizer, min_size_this);
			min_size_to_prop /= prop_this;

			if (min_size_to_prop > max_min_size_to_prop)
				max_min_size_to_prop = min_size_to_prop;

			bsd->total_proportion += prop_this;
		}
		else // fixed size item
		{
			// Just account for its size directly
			*size_in_major_dir(list_sizer, &min_size) += get_size_in_major_dir(list_sizer, min_size_this);
		}

		// In the transversal direction we just need to find the maximum.
		if (get_size_in_minor_dir(list_sizer, min_size_this) > get_size_in_minor_dir(list_sizer, min_size))
			*size_in_minor_dir(list_sizer, &min_size) = get_size_in_minor_dir(list_sizer, min_size_this);

		node = node->next;
	}

	// Using the max ratio ensures that the min size is big enough for all
	// items to have their min size and satisfy the proportions among them.
	*size_in_major_dir(list_sizer, &min_size) += (int)(max_min_size_to_prop * bsd->total_proportion);

	return min_size;
}

static void list_sizer_reposition_children(UIListSizer *list_sizer, UISize min_size)
{
	UIListSizerData *bsd = (UIListSizerData *)list_sizer->private_data;
	int item_major_step;

	if (list_sizer->items == NULL)
		return;

//    int item_count = get_item_count(list_sizer);
//    int item_width = list_sizer->size.height / item_count;
    int total_minor_size = get_size_in_minor_dir(list_sizer, list_sizer->size);
    UISizerItemNode *node;					// iterator in m_children list
	unsigned int n = 0;                     // item index in majorSizes array

	for (node = list_sizer->items; node != NULL; node = node->next)
	{
		UISizerItem *item = &(node->item);

		if (!sizer_item_shown(item))
			continue;

        n++;
	}

	if (bsd->dir == UI_SIZER_DIR_VER)
    {
        item_major_step = list_sizer->size.height / n;
    }
    else
    {
        item_major_step = list_sizer->size.width / n;
    }

	// the position at which we put the next child
	UIPoint pt = list_sizer->position;
	for (node = list_sizer->items, n = 0; node != NULL; node = node->next, ++n)
	{
		UISizerItem *item = &(node->item);

		if (!sizer_item_shown(item))
			continue;

		const int major_size = item_major_step;

		UISize sizeThis = get_sizer_item_min_size_with_border(item);

		// apply the alignment in the minor direction
		UIPoint posChild = pt;

		int minorSize = get_size_in_minor_dir(list_sizer, sizeThis);
		const int flag = item->flags;

		if (flag & (is_vertical(list_sizer) ? UI_ALIGN_RIGHT : UI_ALIGN_BOTTOM))
		{
			*pos_in_minor_dir(list_sizer, &posChild) += total_minor_size - minorSize;
		}
		// NB: wxCENTRE is used here only for backwards compatibility,
		//     wxALIGN_CENTRE should be used in new code
		else if (flag & (UI_ALIGN_CENTER | (is_vertical(list_sizer) ? UI_ALIGN_CENTER_HOR : UI_ALIGN_CENTER_VER)))
		{
			*pos_in_minor_dir(list_sizer, &posChild) += (total_minor_size - minorSize) / 2;
		}


		// apply RTL adjustment for horizontal sizers:
//		if (!IsVertical() && m_containingWindow)
//		{
//			posChild.x = m_containingWindow->AdjustForLayoutDirection(posChild.x, major_size, m_size.x);
//		}
		// finally set size of this child and advance to the next one
		set_sizer_item_dimension(item, posChild, SizeFromMajorMinor(list_sizer, major_size, minorSize));

		*pos_in_major_dir(list_sizer, &pt) += major_size;
	}
}

static void list_sizer_on_destroy(UIListSizer *list_sizer)
{
	if (list_sizer && list_sizer->private_data)
		gui_free(list_sizer->private_data);
}


//-------------------------------------------------------------------------------------------------
// interface
//-------------------------------------------------------------------------------------------------
UIListSizer* create_list_sizer(BOOL dir)
{
	UISizer *sizer;
	UIListSizerData *list_sizer_data;

	sizer = create_sizer();
	if (sizer == NULL)
		return NULL;

	list_sizer_data = (UIListSizerData *)gui_malloc(sizeof(UIListSizerData));
	if (list_sizer_data == NULL)
	{
		gui_free(sizer);
		return NULL;
	}

	list_sizer_data->dir = dir;
	list_sizer_data->total_proportion = 0;

	sizer->private_data = list_sizer_data;
	sizer->calc_min_size = calc_list_sizer_min_size;
	sizer->reposition_children = list_sizer_reposition_children;
	sizer->on_destroy = list_sizer_on_destroy;

	return sizer;
}
