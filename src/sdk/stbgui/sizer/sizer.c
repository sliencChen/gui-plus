/*
 * sizer.c
 *
 *  Created on: 2019-7-2
 *      Author: Brant
 */

#include <gui_window.h>
#include <sizer/sizer.h>


//-------------------------------------------------------------------------------------------------
// UISizerItem - internal use
//-------------------------------------------------------------------------------------------------
static UISize size_with_border(UISizerItem *item, UISize size)
{
	UISize result = size;

	// Notice that we shouldn't modify the unspecified component(s) of the
	// size, it's perfectly valid to have either min or max size specified in
	// one direction only and it shouldn't be applied in the other one then.

	if (result.width != -1)
	{
		if (item->flags & UI_SIZER_FLAG_BORDER_LEFT)
			result.width += item->border;
		if (item->flags & UI_SIZER_FLAG_BORDER_RIGHT)
			result.width += item->border;
	}

	if (result.height != -1)
	{
		if (item->flags & UI_SIZER_FLAG_BORDER_TOP)
			result.height += item->border;
		if (item->flags & UI_SIZER_FLAG_BORDER_BOTTOM)
			result.height += item->border;
	}

	return result;
}


//-------------------------------------------------------------------------------------------------
// UISizerItem - interface
//-------------------------------------------------------------------------------------------------
BOOL sizer_item_shown(UISizerItem *item)
{
	switch (item->type)
	{
		case UI_SIZER_ITEM_SIZER:
			return any_item_show_in_sizer(item->sizer);
		case UI_SIZER_ITEM_WINDOW:
			return win_is_drawable(item->window);
		default:
			break;
	}
	return FALSE;
}

UISize get_sizer_item_min_size_with_border(UISizerItem *item)
{
	return size_with_border(item, item->min_size_temp);
}

UISize get_sizer_item_max_size_with_border(UISizerItem *item)
{
	return size_with_border(item, ui_size(-1, -1));
}

UISize sizer_item_calc_min_size(UISizerItem *item)
{
	if (item->type == UI_SIZER_ITEM_WINDOW)
	{
		UISize win_min_size = get_window_min_size(item->window);

		item->min_size_temp.width = MAX(item->min_size.width, win_min_size.width);
		item->min_size_temp.height = MAX(item->min_size.height, win_min_size.height);
	}
	else if (item->type == UI_SIZER_ITEM_SIZER)
	{
		item->min_size_temp = calc_sizer_min_size(item->sizer);
	}

	return get_sizer_item_min_size_with_border(item);
}

BOOL inform_sizer_item_first_dir(UISizerItem *item, int direction, int size, int availableOtherDir)
{
	// The size that come here will be including borders. Child items should get it
	// without borders.

	if (size > 0)
	{
		if (direction == UI_SIZER_DIR_HOR)
		{
			if (item->flags & UI_SIZER_FLAG_BORDER_LEFT)
				size -= item->border;
			if (item->flags & UI_SIZER_FLAG_BORDER_RIGHT)
				size -= item->border;
		}
		else if (direction == UI_SIZER_DIR_VER)
		{
			if (item->flags & UI_SIZER_FLAG_BORDER_TOP)
				size -= item->border;
			if (item->flags & UI_SIZER_FLAG_BORDER_BOTTOM)
				size -= item->border;
		}
	}

	BOOL didUse = FALSE;
#if 0
	// Pass the information along to the held object
    if (IsSizer())
    {
        didUse = GetSizer()->InformFirstDirection(direction,size,availableOtherDir);
        if (didUse)
            m_minSize = GetSizer()->CalcMin();
    }
    else if (IsWindow())
    {
        didUse =  GetWindow()->InformFirstDirection(direction,size,availableOtherDir);
        if (didUse)
            m_minSize = m_window->GetEffectiveMinSize();

        // This information is useful for items with wxSHAPED flag, since
        // we can request an optimal min size for such an item. Even if
        // we overwrite the m_minSize member here, we can read it back from
        // the owned window (happens automatically).
        if( (m_flag & wxSHAPED) && (m_flag & wxEXPAND) && direction )
        {
            if( !wxIsNullDouble(m_ratio) )
            {
                wxCHECK_MSG( m_proportion==0, false, wxT("Shaped item, non-zero proportion in wxSizerItem::InformFirstDirection()") );
                if( direction==wxHORIZONTAL && !wxIsNullDouble(m_ratio) )
                {
                    // Clip size so that we don't take too much
                    if( availableOtherDir>=0 && int(size/m_ratio)-m_minSize.y>availableOtherDir )
                        size = int((availableOtherDir+m_minSize.y)*m_ratio);
                    m_minSize = wxSize(size,int(size/m_ratio));
                }
                else if( direction==wxVERTICAL )
                {
                    // Clip size so that we don't take too much
                    if( availableOtherDir>=0 && int(size*m_ratio)-m_minSize.x>availableOtherDir )
                        size = int((availableOtherDir+m_minSize.x)/m_ratio);
                    m_minSize = wxSize(int(size*m_ratio),size);
                }
                didUse = TRUE;
            }
        }
    }
#endif
	return didUse;
}

void set_sizer_item_dimension(UISizerItem *item, UIPoint point, UISize size)
{
#if 0
    if (item->flags & UI_SIZER_FLAG_SHAPED)
    {
        // adjust aspect ratio
        int rwidth = (int) (size.y * m_ratio);
        if (rwidth > size.x)
        {
            // fit horizontally
            int rheight = (int) (size.x / m_ratio);
            // add vertical space
            if (m_flag & wxALIGN_CENTER_VERTICAL)
                point.y += (size.y - rheight) / 2;
            else if (m_flag & wxALIGN_BOTTOM)
                point.y += (size.y - rheight);
            // use reduced dimensions
            size.y =rheight;
        }
        else if (rwidth < size.x)
        {
            // add horizontal space
            if (item->flags & wxALIGN_CENTER_HORIZONTAL)
                point.x += (size.x - rwidth) / 2;
            else if (m_flag & wxALIGN_RIGHT)
                point.x += (size.x - rwidth);
            size.x = rwidth;
        }
    }
#endif

	// This is what GetPosition() returns. Since we calculate
	// borders afterwards, GetPosition() will be the left/top
	// corner of the surrounding border.
//	m_pos = point;

	if (item->flags & UI_SIZER_FLAG_BORDER_LEFT)
	{
		point.x += item->border;
		size.width -= item->border;
	}
	if (item->flags & UI_SIZER_FLAG_BORDER_RIGHT)
	{
		size.width -= item->border;
	}
	if (item->flags & UI_SIZER_FLAG_BORDER_TOP)
	{
		point.y += item->border;
		size.height -= item->border;
	}
	if (item->flags & UI_SIZER_FLAG_BORDER_BOTTOM)
	{
		size.height -= item->border;
	}

	if (size.width < 0)
		size.width = 0;
	if (size.height < 0)
		size.height = 0;

//    m_rect = wxRect(point, size);

	switch (item->type)
	{
		case UI_SIZER_ITEM_WINDOW:
		{
			// Use wxSIZE_FORCE_EVENT here since a sizer item might
			// have changed alignment or some other property which would
			// not change the size of the window. In such a case, no
			// wxSizeEvent would normally be generated and thus the
			// control wouldn't get laid out correctly here.

			gui_printf("set window [%s] rect (%d, %d, %d, %d)\n", item->window->id, point.x, point.y, size.width, size.height);

			set_window_rect(item->window, ui_rect(point.x, point.y, size.width, size.height));
			break;
		}

		case UI_SIZER_ITEM_SIZER:
			sizer_set_dimension(item->sizer, ui_rect(point.x, point.y, size.width, size.height));
			break;

//		case Item_Spacer:
//			m_spacer->SetSize(size);
//			break;
//
//		case Item_Max:
		default:
			break;
	}
}


//-------------------------------------------------------------------------------------------------
// UISizerItemNode
//-------------------------------------------------------------------------------------------------
void destroy_sizer_item_node(UISizerItemNode *node)
{
	if (node == NULL)
		return;

	if (node->item.type == UI_SIZER_ITEM_SIZER)
		destroy_sizer(node->item.sizer);

	gui_free(node);
}


//-------------------------------------------------------------------------------------------------
// UISizer - internal use
//-------------------------------------------------------------------------------------------------
static int sizer_add_node(UISizer *sizer, UISizerItemNode *new_node)
{
	UISizerItemNode *node;

	if (sizer->items == NULL)
	{
		sizer->items = new_node;
		return 0;
	}

	node = sizer->items;
	while (node->next != NULL)
		node = node->next;

	node->next = new_node;

	return 0;
}


//-------------------------------------------------------------------------------------------------
// UISizer - interface
//-------------------------------------------------------------------------------------------------
UISizer* create_sizer(void)
{
	UISizer *sizer;

	sizer = (UISizer *)gui_malloc(sizeof(UISizer));
	if (sizer)
	{
		gui_memset(sizer, 0, sizeof(UISizer));
	}
	return sizer;
}

void destroy_sizer(UISizer *sizer)
{
	UISizerItemNode *node, *next;

	if (sizer == NULL)
		return;

	/* delete all item node first */
	node = sizer->items;
	while (node)
	{
		next = node->next;
		destroy_sizer_item_node(node);
		node = next;
	}

	/* delete private data */
	if (sizer->on_destroy)
		sizer->on_destroy(sizer);

	gui_free(sizer);
}

int sizer_add_window(UISizer *sizer, UIWindow *window, UISize min_size, int proportion, int border, U32 flags)
{
	UISizerItemNode *node;

	node = (UISizerItemNode *)gui_malloc(sizeof(UISizerItemNode));
	if (node == NULL)
		return -1;

	gui_memset(node, 0, sizeof(UISizerItemNode));
	node->item.type = UI_SIZER_ITEM_WINDOW;
	node->item.window = window;
	node->item.min_size = min_size;
	node->item.min_size_temp = min_size;
	node->item.proportion = proportion;
	node->item.border = border;
	node->item.flags = flags;
	node->next = NULL;

	return sizer_add_node(sizer, node);
}

int sizer_add_sub_sizer(UISizer *sizer, UISizer *sub_sizer, int proportion, int border, U32 flags)
{
	UISizerItemNode *node;

	node = (UISizerItemNode *)gui_malloc(sizeof(UISizerItemNode));
	if (node == NULL)
		return -1;

	gui_memset(node, 0, sizeof(UISizerItemNode));
	node->item.type = UI_SIZER_ITEM_SIZER;
	node->item.sizer = sub_sizer;
	node->item.proportion = proportion;
	node->item.border = border;
	node->item.flags = flags;

	return sizer_add_node(sizer, node);
}

int get_item_count(UISizer *sizer)
{
	UISizerItemNode *node;
	int count = 0;

	for (node = sizer->items; node != NULL; node = node->next)
	{
		count++;
	}
	return count;
}

BOOL any_item_show_in_sizer(UISizer *sizer)
{
	UISizerItemNode *node;

	for (node = sizer->items; node != NULL; node = node->next)
	{
		if (sizer_item_shown(&(node->item)))
			return TRUE;
	}
	return FALSE;
}

UISize calc_sizer_min_size(UISizer *self)
{
	if (self->calc_min_size)
		self->min_size = self->calc_min_size(self);
	else
		self->min_size = ui_size(1, 1);

	return self->min_size;
}

UISize get_sizer_min_size(UISizer *self)
{
	if (!is_empty_size(self->min_size))
		return self->min_size;

	return calc_sizer_min_size(self);
}

void sizer_set_dimension(UISizer *sizer, UIRect rect)
{
	sizer->position = rect.point;
	sizer->size = rect.size;

	sizer_layout(sizer);
}

void sizer_layout(UISizer *sizer)
{
	UISize min_size = calc_sizer_min_size(sizer);

	if (sizer->reposition_children)
		sizer->reposition_children(sizer, min_size);
}


//-------------------------------------------------------------------------------------------------
// UISizer - util
//-------------------------------------------------------------------------------------------------
/*
    Helper of RepositionChildren(): checks if there is enough remaining space
    for the min size of the given item and returns its min size or the entire
    remaining space depending on which one is greater.

    This function updates the remaining space parameter to account for the size
    effectively allocated to the item.
 */
int GetMinOrRemainingSize(int orient, UISizerItem *item, int *remaining_space)
{
	int size;

	if (*remaining_space > 0)
	{
		UISize sizeMin = get_sizer_item_min_size_with_border(item);
		size = orient == UI_SIZER_DIR_HOR ? sizeMin.width : sizeMin.height;

		if (size >= *remaining_space)
		{
			// truncate the item to fit in the remaining space, this is better
			// than showing it only partially in general, even if both choices
			// are bad -- but there is nothing else we can do
			size = *remaining_space;
		}

		*remaining_space -= size;
	}
	else // no remaining space
	{
		// no space at all left, no need to even query the item for its min
		// size as we can't give it to it anyhow
		size = 0;
	}

	return size;
}
