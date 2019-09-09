/*
 * box_sizer.c
 *
 *  Created on: 2019-7-3
 *      Author: Brant
 */

#include <gui_window.h>
#include <sizer/box_sizer.h>


//-------------------------------------------------------------------------------------------------
// internal use
//-------------------------------------------------------------------------------------------------
static BOOL is_vertical(UIBoxSizer *box_sizer)
{
	UIBoxSizerData *bsd = (UIBoxSizerData *)box_sizer->private_data;

	return bsd->dir == UI_SIZER_DIR_VER;
}

static int get_size_in_major_dir(UIBoxSizer *box_sizer, UISize size)
{
	UIBoxSizerData *bsd = (UIBoxSizerData *)box_sizer->private_data;

	if (bsd->dir == UI_SIZER_DIR_HOR)
		return size.width;

	return size.height;
}

static int* size_in_major_dir(UIBoxSizer *box_sizer, UISize *size)
{
	UIBoxSizerData *bsd = (UIBoxSizerData *)box_sizer->private_data;

	if (bsd->dir == UI_SIZER_DIR_HOR)
		return &(size->width);

	return &(size->height);
}

static int get_size_in_minor_dir(UIBoxSizer *box_sizer, UISize size)
{
	UIBoxSizerData *bsd = (UIBoxSizerData *)box_sizer->private_data;

	if (bsd->dir == UI_SIZER_DIR_HOR)
		return size.height;

	return size.width;
}

static int* size_in_minor_dir(UIBoxSizer *box_sizer, UISize *size)
{
	UIBoxSizerData *bsd = (UIBoxSizerData *)box_sizer->private_data;

	if (bsd->dir == UI_SIZER_DIR_HOR)
		return &(size->height);

	return &(size->width);
}

static int* pos_in_major_dir(UIBoxSizer *box_sizer, UIPoint *point)
{
	UIBoxSizerData *bsd = (UIBoxSizerData *)box_sizer->private_data;

	if (bsd->dir == UI_SIZER_DIR_HOR)
		return &(point->x);

	return &(point->y);
}

static int* pos_in_minor_dir(UIBoxSizer *box_sizer, UIPoint *point)
{
	UIBoxSizerData *bsd = (UIBoxSizerData *)box_sizer->private_data;

	if (bsd->dir == UI_SIZER_DIR_HOR)
		return &(point->y);

	return &(point->x);
}

static UISize SizeFromMajorMinor(UIBoxSizer *box_sizer, int major, int minor)
{
	UIBoxSizerData *bsd = (UIBoxSizerData *)box_sizer->private_data;

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
static UISize calc_box_sizer_min_size(UIBoxSizer *box_sizer)
{
	UIBoxSizerData *bsd = (UIBoxSizerData *)box_sizer->private_data;
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

	node = box_sizer->items;
	while (node)
	{
		item = &(node->item);

		if (!sizer_item_shown(item))
        {
            node = node->next;
			continue;
        }

		min_size_this = sizer_item_calc_min_size(item);
		prop_this = item->proportion;
		if (prop_this)
		{
			min_size_to_prop = get_size_in_major_dir(box_sizer, min_size_this);
			min_size_to_prop /= prop_this;

			if (min_size_to_prop > max_min_size_to_prop)
				max_min_size_to_prop = min_size_to_prop;

			bsd->total_proportion += prop_this;
		}
		else // fixed size item
		{
			// Just account for its size directly
			*size_in_major_dir(box_sizer, &min_size) += get_size_in_major_dir(box_sizer, min_size_this);
		}

		// In the transversal direction we just need to find the maximum.
		if (get_size_in_minor_dir(box_sizer, min_size_this) > get_size_in_minor_dir(box_sizer, min_size))
			*size_in_minor_dir(box_sizer, &min_size) = get_size_in_minor_dir(box_sizer, min_size_this);

		node = node->next;
	}

	// Using the max ratio ensures that the min size is big enough for all
	// items to have their min size and satisfy the proportions among them.
	*size_in_major_dir(box_sizer, &min_size) += (int)(max_min_size_to_prop * bsd->total_proportion);

	return min_size;
}

static void box_sizer_reposition_children(UIBoxSizer *box_sizer, UISize min_size)
{
	UIBoxSizerData *bsd = (UIBoxSizerData *)box_sizer->private_data;

	if (box_sizer->items == NULL)
		return;

	int total_minor_size = get_size_in_minor_dir(box_sizer, box_sizer->size);
	int total_major_size = get_size_in_major_dir(box_sizer, box_sizer->size);

	// the amount of free space which we should redistribute among the
	// stretchable items (i.e. those with non zero proportion)
	int delta = total_major_size - get_size_in_major_dir(box_sizer, min_size);

	// declare loop variables used below:
	UISizerItemNode *node;					// iterator in m_children list
	unsigned int n = 0;                     // item index in majorSizes array

	// First, inform item about the available size in minor direction as this
	// can change their size in the major direction. Also compute the number of
	// visible items and sum of their min sizes in major direction.

	int min_major_size = 0;
	for (node = box_sizer->items; node != NULL; node = node->next)
	{
		UISizerItem *item = &(node->item);

		if (!sizer_item_shown(item))
			continue;

		UISize szMinPrev = get_sizer_item_min_size_with_border(item);
		inform_sizer_item_first_dir(item, bsd->dir/*^wxBOTH*/, total_minor_size, delta);
		UISize szMin = get_sizer_item_min_size_with_border(item);
		int deltaChange = get_size_in_major_dir(box_sizer, size_minus(szMin, szMinPrev));
		if (deltaChange)
		{
			// Since we passed available space along to the item, it should not
			// take too much, so delta should not become negative.
			delta -= deltaChange;
		}
		min_major_size += get_size_in_major_dir(box_sizer, get_sizer_item_min_size_with_border(item));
	}


	// space and sum of proportions for the remaining items, both may change
	// below
	int remaining = total_major_size;
	int total_proportion = bsd->total_proportion;

	// size of the (visible) items in major direction, -1 means "not fixed yet"
	int *major_sizes = (int *)gui_malloc(get_item_count(box_sizer) * sizeof(int));
	if (major_sizes == NULL)
		return;

	gui_memset(major_sizes, 0xff, get_item_count(box_sizer) * sizeof(int));


	// Check for the degenerated case when we don't have enough space for even
	// the min sizes of all the items: in this case we really can't do much
	// more than to allocate the min size to as many of fixed size items as
	// possible (on the assumption that variable size items such as text zones
	// or list boxes may use scrollbars to show their content even if their
	// size is less than min size but that fixed size items such as buttons
	// will suffer even more if we don't give them their min size)
	if (total_major_size < min_major_size)
	{
		// Second degenerated case pass: allocate min size to all fixed size
		// items.
		for (node = box_sizer->items; node != NULL; node = node->next, ++n)
		{
			UISizerItem *item = &(node->item);

			if (!sizer_item_shown(item))
				continue;

			// deal with fixed size items only during this pass
			if (item->proportion)
				continue;

			major_sizes[n] = GetMinOrRemainingSize(bsd->dir, item, &remaining);
		}


		// Third degenerated case pass: allocate min size to all the remaining,
		// i.e. non-fixed size, items.
		for (node = box_sizer->items; node != NULL; node = node->next, ++n)
		{
			UISizerItem *item = &(node->item);

			if (!sizer_item_shown(item))
				continue;

			// we've already dealt with fixed size items above
			if (!item->proportion)
				continue;

			major_sizes[n] = GetMinOrRemainingSize(bsd->dir, item, &remaining);
		}
	}
	else // we do have enough space to give at least min sizes to all items
	{
		// Second and maybe more passes in the non-degenerated case: deal with
		// fixed size items and items whose min size is greater than what we
		// would allocate to them taking their proportion into account. For
		// both of them, we will just use their min size, but for the latter we
		// also need to reexamine all the items as the items which fitted
		// before we adjusted their size upwards might not fit any more. This
		// does make for a quadratic algorithm but it's not obvious how to
		// avoid it and hopefully it's not a huge problem in practice as the
		// sizers don't have many items usually (and, of course, the algorithm
		// still reduces into a linear one if there is enough space for all the
		// min sizes).
		BOOL nonfixed_space_changed = FALSE;
		for (node = box_sizer->items, n = 0; ; node = node->next, ++n)
		{
			if (nonfixed_space_changed)
			{
				node = box_sizer->items;
				n = 0;
				nonfixed_space_changed = FALSE;
			}

			// check for the end of the loop only after the check above as
			// otherwise we wouldn't do another pass if the last child resulted
			// in non fixed space reduction
			if (node == NULL)
				break;

			UISizerItem *item = &(node->item);

			if (!sizer_item_shown(item))
				continue;

			// don't check the item which we had already dealt with during a
			// previous pass (this is more than an optimization, the code
			// wouldn't work correctly if we kept adjusting for the same item
			// over and over again)
			if (major_sizes[n] != -1)
				continue;

			int min_major = get_size_in_major_dir(box_sizer, get_sizer_item_min_size_with_border(item));

			// it doesn't make sense for min size to be negative but right now
			// it's possible to create e.g. a spacer with (-1, 10) as size and
			// people do it in their code apparently (see #11842) so ensure
			// that we don't use this -1 as real min size as it conflicts with
			// the meaning we use for it here and negative min sizes just don't
			// make sense anyhow (which is why it might be a better idea to
			// deal with them at wxSizerItem level in the future but for now
			// this is the minimal fix for the bug)
			if (min_major < 0)
				min_major = 0;

			const int prop_item = item->proportion;
			if (prop_item)
			{
				// is the desired size of this item big enough?
				if ( (remaining*prop_item)/total_proportion >= min_major )
				{
					// yes, it is, we'll determine the real size of this
					// item later, for now just leave it as wxDefaultCoord
					continue;
				}

				// the proportion of this item won't count, it has
				// effectively become fixed
				total_proportion -= prop_item;
			}

			// we can already allocate space for this item
			major_sizes[n] = min_major;

			// change the amount of the space remaining to the other items,
			// as this can result in not being able to satisfy their
			// proportions any more we will need to redo another loop
			// iteration
			remaining -= min_major;

			nonfixed_space_changed = TRUE;
		}

		// Similar to the previous loop, but dealing with items whose max size
		// is less than what we would allocate to them taking their proportion
		// into account.
		nonfixed_space_changed = FALSE;
		for (node = box_sizer->items, n = 0; ; node = node->next, ++n)
		{
			if (nonfixed_space_changed)
			{
				node = box_sizer->items;
				n = 0;
				nonfixed_space_changed = FALSE;
			}

			// check for the end of the loop only after the check above as
			// otherwise we wouldn't do another pass if the last child resulted
			// in non fixed space reduction
			if (node == NULL)
				break;

			UISizerItem *item = &(node->item);

			if (!sizer_item_shown(item))
				continue;

			// don't check the item which we had already dealt with during a
			// previous pass (this is more than an optimization, the code
			// wouldn't work correctly if we kept adjusting for the same item
			// over and over again)
			if (major_sizes[n] != -1)
				continue;

			int max_major = get_size_in_major_dir(box_sizer, get_sizer_item_max_size_with_border(item));

			// must be nonzero, fixed-size items were dealt with in the previous loop
			const int prop_item = item->proportion;

			// is the desired size of this item small enough?
			if (max_major < 0 || ((remaining * prop_item) / total_proportion) <= max_major)
			{
				// yes, it is, we'll determine the real size of this
				// item later, for now just leave it as wxDefaultCoord
				continue;
			}

			// the proportion of this item won't count, it has
			// effectively become fixed
			total_proportion -= prop_item;

			// we can already allocate space for this item
			major_sizes[n] = max_major;

			// change the amount of the space remaining to the other items,
			// as this can result in not being able to satisfy their
			// proportions any more we will need to redo another loop
			// iteration
			remaining -= max_major;

			nonfixed_space_changed = TRUE;
		}

		// Last by one pass: distribute the remaining space among the non-fixed
		// items whose size weren't fixed yet according to their proportions.
		for (node = box_sizer->items, n = 0; node != NULL; node = node->next, ++n)
		{
			UISizerItem *item = &(node->item);

			if (!sizer_item_shown(item))
				continue;

			if (major_sizes[n] == -1)
			{
				const int prop_item = item->proportion;
				major_sizes[n] = (remaining * prop_item) / total_proportion;

				remaining -= major_sizes[n];
				total_proportion -= prop_item;
			}
		}
	}


	// the position at which we put the next child
	UIPoint pt = box_sizer->position;


	// Final pass: finally do position the items correctly using their sizes as
	// determined above.
	for (node = box_sizer->items, n = 0; node != NULL; node = node->next, ++n)
	{
		UISizerItem *item = &(node->item);

		if (!sizer_item_shown(item))
			continue;

		const int major_size = major_sizes[n];

		UISize sizeThis = get_sizer_item_min_size_with_border(item);

		// apply the alignment in the minor direction
		UIPoint posChild = pt;

		int minorSize = get_size_in_minor_dir(box_sizer, sizeThis);
		const int flag = item->flags;
		if ((flag & (UI_SIZER_FLAG_EXPAND | UI_SIZER_FLAG_SHAPED)) || (minorSize > total_minor_size))
		{
			// occupy all the available space if wxEXPAND was given and also if
			// the item is too big to fit -- in this case we truncate it below
			// its minimal size which is bad but better than not showing parts
			// of the window at all
			minorSize = total_minor_size;

			// do not allow the size in the minor direction to grow beyond the max
			// size of the item in the minor direction
			const int maxMinorSize = get_size_in_minor_dir(box_sizer, get_sizer_item_max_size_with_border(item));
			if (maxMinorSize >= 0 && minorSize > maxMinorSize)
				minorSize = maxMinorSize;
		}

		if (flag & (is_vertical(box_sizer) ? UI_ALIGN_RIGHT : UI_ALIGN_BOTTOM))
		{
			*pos_in_minor_dir(box_sizer, &posChild) += total_minor_size - minorSize;
		}
		// NB: wxCENTRE is used here only for backwards compatibility,
		//     wxALIGN_CENTRE should be used in new code
		else if (flag & (UI_ALIGN_CENTER | (is_vertical(box_sizer) ? UI_ALIGN_CENTER_HOR : UI_ALIGN_CENTER_VER)))
		{
			*pos_in_minor_dir(box_sizer, &posChild) += (total_minor_size - minorSize) / 2;
		}


		// apply RTL adjustment for horizontal sizers:
//		if (!IsVertical() && m_containingWindow)
//		{
//			posChild.x = m_containingWindow->AdjustForLayoutDirection(posChild.x, major_size, m_size.x);
//		}

		// finally set size of this child and advance to the next one
		set_sizer_item_dimension(item, posChild, SizeFromMajorMinor(box_sizer, major_size, minorSize));

		*pos_in_major_dir(box_sizer, &pt) += major_size;
	}
}

static void box_sizer_on_destroy(UIBoxSizer *box_sizer)
{
	if (box_sizer && box_sizer->private_data)
		gui_free(box_sizer->private_data);
}


//-------------------------------------------------------------------------------------------------
// interface
//-------------------------------------------------------------------------------------------------
UIBoxSizer* create_box_sizer(int dir)
{
	UISizer *sizer;
	UIBoxSizerData *box_sizer_data;

	sizer = create_sizer();
	if (sizer == NULL)
		return NULL;

	box_sizer_data = (UIBoxSizerData *)gui_malloc(sizeof(UIBoxSizerData));
	if (box_sizer_data == NULL)
	{
		gui_free(sizer);
		return NULL;
	}

	box_sizer_data->dir = dir;
	box_sizer_data->total_proportion = 0;

	sizer->private_data = box_sizer_data;
	sizer->calc_min_size = calc_box_sizer_min_size;
	sizer->reposition_children = box_sizer_reposition_children;
	sizer->on_destroy = box_sizer_on_destroy;

	return sizer;
}
