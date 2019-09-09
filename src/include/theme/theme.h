/*
 * theme.h
 *
 *  Created on: 2019-5-17
 *      Author: Brant
 */

#ifndef _THEME_H_
#define _THEME_H_

#include <wx/gdicmn.h>

//-------------------------------------------------------------------------------------------------
// Theme Items
//-------------------------------------------------------------------------------------------------
class ThemeItem
{
public:
	ThemeItem();

	wxSize size;

	ThemeItem* GetChild();
	ThemeItem* GetNext();


private:
	ThemeItem *m_Parent;
	ThemeItem *m_Child;
	ThemeItem *m_Next;
};


class ThemeItemLine : public ThemeItem
{
public:
	wxPoint m_Start;
	wxPoint m_End;
	int m_Width;
	unsigned int m_Color;
};

class ThemeItemRect : public ThemeItem
{
	wxRect m_Rect;
	unsigned int m_Color;
};

class ThemeItemText : public ThemeItem
{
	wxString m_Text;
	int m_FontSize;
	unsigned int m_Color;
};

class ThemeItemImage : public ThemeItem
{
	wxString m_Name;
	wxRect m_Rect;
};



//-------------------------------------------------------------------------------------------------
// uiTheme
//-------------------------------------------------------------------------------------------------
class uiTheme
{
public:
	wxString FullDisplayName() { return m_Name + wxT("(") + m_SubName + wxT(")"); }

public:
	wxString m_Name;
	wxString m_SubName;

	ThemeItem *rootItem;

	wxSize m_Size;
};



#endif /* _THEME_H_ */
