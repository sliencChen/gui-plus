/*
 * themesizer.h
 *
 *  Created on: 2019-5-31
 *      Author: Brant
 */

#ifndef THEME_SIZER_H_
#define THEME_SIZER_H_

#include <wx/gdicmn.h>


//-------------------------------------------------------------------------------------------------
// ThemeSizerItem
//-------------------------------------------------------------------------------------------------
enum
{
	SizerItemFlag_Left = 0x0000,
	SizerItemFlag_Center = 0x0001,
	SizerItemFlag_Right = 0x0002,
	SizerItemFlag_Middle = 0x0004,
	SizerItemFlag_Bottom = 0x0008,
	SizerItemFlag_AlignMask = 0x000F,

	SizerItemFlag_Expand = 0x0010,
	SizerItemFlag_AttribMask = 0x00F0,

	SizerItemFlag_TopBorder = 0x1000,
	SizerItemFlag_RightBorder = 0x2000,
	SizerItemFlag_BottomBorder = 0x4000,
	SizerItemFlag_LeftBorder = 0x8000,
	SizerItemFlag_BorderMask = 0xF000,

	SizerItemFlag_Max
};

class ThemeItem;
class ThemeSizer;

class ThemeSizerItem
{
public:
	ThemeSizerItem(ThemeItem *item, int proportion = 0, int flag = 0, int border = 0);
	ThemeSizerItem(ThemeSizer *sizer, int proportion = 0, int flag = 0, int border = 0);

	ThemeSizerItem();
    virtual ~ThemeSizerItem();

    bool IsNormalItem() const { return m_Kind == ThemeSizerItem_Normal; }
    bool IsSizer() const { return m_Kind == ThemeSizerItem_Sizer; }

    int GetProportion() const { return m_Proportion; }
    int GetFlag() const { return m_Flag; }
    wxSize GetMinSize() { return m_MinSize; }

    virtual wxSize CalcMin();
    wxSize GetMinSize() const;

    void RepositionChildren();

    wxSize m_Size;
    wxPoint m_Position;

protected:
    enum
    {
    	ThemeSizerItem_None,
    	ThemeSizerItem_Normal,
    	ThemeSizerItem_Sizer,
    	ThemeSizerItem_Spacer,
    	ThemeSizerItem_Max
    } m_Kind;
    union
    {
    	ThemeItem *m_Item;
    	ThemeSizer *m_Sizer;
//        wxSizerSpacer *m_spacer;
    };

    void Init();

    wxSize AddBorderToSize(const wxSize& size) const;

private:
    wxSize GetMinSizeWithBorder() const;

    wxSize m_MinSize;
    int	m_Proportion;
    int	m_Border;
    int	m_Flag;
};


WX_DECLARE_EXPORTED_LIST(ThemeSizerItem, ThemeSizerItemList);

//-------------------------------------------------------------------------------------------------
// ThemeSizer
//-------------------------------------------------------------------------------------------------
class ThemeSizer
{
public:
	ThemeSizer();
    virtual ~ThemeSizer();

    ThemeSizerItem* Add(ThemeSizerItem *item);
    ThemeSizerItem* Add(ThemeItem *item, int proportion = 0, int flag = 0, int border = 0);
    ThemeSizerItem* Add(ThemeSizer *sizer, int proportion = 0, int flag = 0, int border = 0);

    ThemeSizerItem* Insert(size_t index, ThemeSizerItem *item);
    ThemeSizerItem* Insert(size_t index, ThemeItem *item, int proportion = 0, int flag = 0, int border = 0);
    ThemeSizerItem* Insert(size_t index, ThemeSizer *sizer, int proportion = 0, int flag = 0, int border = 0);

    wxSize GetMinSize();

    virtual wxSize CalcMin() = 0;
    virtual void RepositionChildren() = 0;

    size_t GetItemCount() const { return m_Children.GetCount(); }

protected:
    virtual ThemeSizerItem* DoInsert(size_t index, ThemeSizerItem *item);

    wxSize             m_Size;
    wxSize             m_MinSize;
    wxPoint            m_Position;
	ThemeSizerItemList m_Children;
};


//-------------------------------------------------------------------------------------------------
// ThemeBoxSizer
//-------------------------------------------------------------------------------------------------
enum
{
	ThemeBoxSizer_Horizontal,
	ThemeBoxSizer_Vertical,
};

class ThemeBoxSizer : public ThemeSizer
{
public:
	virtual wxSize CalcMin() wxOVERRIDE;
	virtual void RepositionChildren() wxOVERRIDE;

protected:
    int GetSizeInMajorDir(const wxSize& sz) const { return m_Orient == ThemeBoxSizer_Horizontal ? sz.x : sz.y; }
    int& SizeInMajorDir(wxSize& sz) { return m_Orient == ThemeBoxSizer_Horizontal ? sz.x : sz.y; }

    int GetSizeInMinorDir(const wxSize& sz) const { return m_Orient == ThemeBoxSizer_Horizontal ? sz.y : sz.x; }
    int& SizeInMinorDir(wxSize& sz) { return m_Orient == ThemeBoxSizer_Horizontal ? sz.y : sz.x; }

	int m_Orient;
	int m_TotalProportion;
};


//-------------------------------------------------------------------------------------------------
// Inline Methods
//-------------------------------------------------------------------------------------------------
inline ThemeSizerItem* ThemeSizer::Insert(size_t index, ThemeSizerItem *item)
{
    return DoInsert(index, item);
}

inline ThemeSizerItem* ThemeSizer::Insert(size_t index, ThemeItem *item, int proportion, int flag, int border)
{
	return Insert(index, new ThemeSizerItem(item, proportion, flag, border));
}

inline ThemeSizerItem* ThemeSizer::Insert(size_t index, ThemeSizer *sizer, int proportion, int flag, int border)
{
	return Insert(index, new ThemeSizerItem(sizer, proportion, flag, border));
}

inline ThemeSizerItem* ThemeSizer::Add(ThemeSizerItem *item)
{
	return Insert(m_Children.GetCount(), item);
}

inline ThemeSizerItem* ThemeSizer::Add(ThemeItem *item, int proportion, int flag, int border)
{
	return Add(new ThemeSizerItem(item, proportion, flag, border));
}

inline ThemeSizerItem* ThemeSizer::Add(ThemeSizer *sizer, int proportion, int flag, int border)
{
	return Add(new ThemeSizerItem(sizer, proportion, flag, border));
}


#endif /* THEME_SIZER_H_ */
