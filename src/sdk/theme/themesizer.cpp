/*
 * themesizer.cpp
 *
 *  Created on: 2019-5-31
 *      Author: Brant
 */

#include <wx/wx.h>
#include <wx/vector.h>
#include <wx/listimpl.cpp>
#include <theme/themesizer.h>


WX_DEFINE_EXPORTED_LIST(ThemeSizerItemList)


//-------------------------------------------------------------------------------------------------
// ThemeSizerItem
//-------------------------------------------------------------------------------------------------
ThemeSizerItem::ThemeSizerItem()
{
	Init();
}

ThemeSizerItem::ThemeSizerItem(ThemeItem *item, int proportion, int flag, int border)
{
	Init();
	m_Proportion = proportion;
	m_Flag = flag;
	m_Border = border;
	m_Kind = ThemeSizerItem_Normal;
	m_Item = item;
}

ThemeSizerItem::ThemeSizerItem(ThemeSizer *sizer, int proportion, int flag, int border)
{
	Init();
	m_Proportion = proportion;
	m_Flag = flag;
	m_Border = border;
	m_Kind = ThemeSizerItem_Sizer;
	m_Sizer = sizer;
}

ThemeSizerItem::~ThemeSizerItem()
{
}

void ThemeSizerItem::Init()
{
	m_MinSize = wxSize();
    m_Proportion = 0;
    m_Border = 0;
    m_Flag = 0;
    m_Kind = ThemeSizerItem_None;
}


wxSize ThemeSizerItem::CalcMin()
{
	if (IsSizer())
	{
		m_MinSize = m_Sizer->GetMinSize();
	}
	else if (IsNormalItem())
	{
//		m_MinSize = m_Item->GetMinSize();
	}

	return GetMinSizeWithBorder();
}

void ThemeSizerItem::RepositionChildren()
{
	if (IsSizer())
		m_Sizer->RepositionChildren();
}

//-------------------------------------------------------------------------------------------------
// ThemeSizerItem protected
//-------------------------------------------------------------------------------------------------
wxSize ThemeSizerItem::AddBorderToSize(const wxSize& size) const
{
    wxSize result = size;

	if (m_Flag & SizerItemFlag_LeftBorder)
		result.x += m_Border;
	if (m_Flag & SizerItemFlag_RightBorder)
		result.x += m_Border;

	if (m_Flag & SizerItemFlag_TopBorder)
		result.y += m_Border;
	if (m_Flag & SizerItemFlag_BottomBorder)
		result.y += m_Border;

    return result;
}

//-------------------------------------------------------------------------------------------------
// ThemeSizerItem private
//-------------------------------------------------------------------------------------------------
wxSize ThemeSizerItem::GetMinSizeWithBorder() const
{
	return AddBorderToSize(m_MinSize);
}


//-------------------------------------------------------------------------------------------------
// ThemeSizer
//-------------------------------------------------------------------------------------------------
wxSize ThemeSizer::GetMinSize()
{
	return CalcMin();
}

ThemeSizer::~ThemeSizer()
{

}

ThemeSizerItem* ThemeSizer::DoInsert(size_t index, ThemeSizerItem *item)
{
	m_Children.Insert(index, item);

    return item;
}


//-------------------------------------------------------------------------------------------------
// ThemeBoxSizer
//-------------------------------------------------------------------------------------------------
wxSize ThemeBoxSizer::CalcMin()
{
	m_TotalProportion = 0;
    wxSize minSize;

    // The minimal size for the sizer should be big enough to allocate its
    // element at least its minimal size but also, and this is the non trivial
    // part, to respect the children proportion. To satisfy the latter
    // condition we must find the greatest min-size-to-proportion ratio for all
    // elements with non-zero proportion.
    float maxMinSizeToProp = 0.;
    for (ThemeSizerItemList::iterator i = m_Children.begin(); i != m_Children.end(); ++i)
    {
    	ThemeSizerItem * const item = *i;

//        if ( !item->IsShown() )
//            continue;

        const wxSize sizeMinThis = item->CalcMin();
        if ( const int propThis = item->GetProportion() )
        {
            float minSizeToProp = GetSizeInMajorDir(sizeMinThis);
            minSizeToProp /= propThis;

            if ( minSizeToProp > maxMinSizeToProp )
                maxMinSizeToProp = minSizeToProp;

            m_TotalProportion += item->GetProportion();
        }
        else // fixed size item
        {
            // Just account for its size directly
            SizeInMajorDir(minSize) += GetSizeInMajorDir(sizeMinThis);
        }

        // In the transversal direction we just need to find the maximum.
		if (GetSizeInMinorDir(sizeMinThis) > GetSizeInMinorDir(minSize))
            SizeInMinorDir(minSize) = GetSizeInMinorDir(sizeMinThis);
    }

    // Using the max ratio ensures that the min size is big enough for all
    // items to have their min size and satisfy the proportions among them.
    SizeInMajorDir(minSize) += (int)(maxMinSizeToProp*m_TotalProportion);

    return minSize;
}

void ThemeBoxSizer::RepositionChildren()
{
    if (m_Children.empty())
        return;

    const wxCoord totalMinorSize = GetSizeInMinorDir(m_Size);
    const wxCoord totalMajorSize = GetSizeInMajorDir(m_Size);
    int delta = totalMajorSize - GetSizeInMajorDir(m_MinSize);

    if (delta < 0)
    	delta = 0;

//    wxPoint pt = m_Position;
    ThemeSizerItemList::iterator i;  // iterator in m_children list
    for (i = m_Children.begin(); i != m_Children.end(); i++)
    {
    	ThemeSizerItem * const item = *i;

    	if (item->GetFlag() & SizerItemFlag_Expand)
    		SizeInMinorDir(item->m_Size) = totalMinorSize;
    	else
    		SizeInMinorDir(item->m_Size) = GetSizeInMinorDir(item->GetMinSize());

    	if (item->GetProportion())
    		SizeInMajorDir(item->m_Size) = GetSizeInMajorDir(item->GetMinSize()) + delta * m_TotalProportion / item->GetProportion();
    	else
    		SizeInMajorDir(item->m_Size) = GetSizeInMajorDir(item->GetMinSize());

//    	if (item->m_Flag & SizerItemFlag_Center)
    	item->RepositionChildren();
    }
}
