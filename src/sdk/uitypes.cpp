/*
 * uitypes.cpp
 *
 *  Created on: 2019-4-16
 *      Author: Brant
 */

#include "sdk_common.h"
#include "uitypes.h"

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(ColorArray);


bool uiColor::operator!=(const uiColor& rhs)
{
	return (m_Value != rhs.m_Value) || (GetName() != rhs.GetName());
}


ColorList::ColorList(void)
{
	m_Colors.Empty();
}

ColorList::ColorList(const ColorList& anotherList)
{
	m_Colors = anotherList.m_Colors;
}

ColorList::~ColorList(void)
{
	m_Colors.Clear();
}


bool ColorList::AddColor(void)
{
	for (unsigned int number = 1; number < INT_MAX; number++)
	{
		wxString name = wxString::Format("New Color %d", number);
		bool find = false;

		for (unsigned int i = 0; i < m_Colors.Count(); i++)
		{
			if (m_Colors.Item(i).GetName() == name)
			{
				find = true;
				break;
			}
		}
		if (!find)
		{
			AddColor(uiColor(0xff000000, name));
			return true;
		}
	}
	return false;
}

bool ColorList::AddColor(uiColor color)
{
	for (unsigned int i = 0; i < m_Colors.Count(); i++)
	{
		if (color.GetName() == m_Colors.Item(i).GetName())
		{
			return false;
		}
	}
	m_Colors.Add(color);

	return true;
}

uiColor ColorList::GetColor(wxString name) const
{
	for (size_t i; i < m_Colors.Count(); i++)
	{
		if (m_Colors.Item(i).GetName() == name)
		{
			return m_Colors.Item(i);
		}
	}
	return uiColor(0, "");
}

int ColorList::FindColor(wxString name) const
{
	for (size_t i = 0; i < m_Colors.Count(); i++)
	{
		if (m_Colors.Item(i).GetName() == name)
		{	wxLogMessage("\nfindcolor %s at %d\n",name,i);
			return i;
		}
	}
	return -1;
}

uiColor ColorList::GetColor(unsigned int index) const
{
	if (index < m_Colors.Count())
	{
		return m_Colors.Item(index);
	}
	return uiColor(0, "");
}

bool ColorList::UpdateColor(unsigned int index, uiColor color)
{
	if (index >= m_Colors.Count()) { return false; }

	for (unsigned int i = 0; i < m_Colors.Count(); i++)
	{
		if (i != index && color.GetName() == m_Colors.Item(i).GetName())
		{
			return false;
		}
	}

	m_Colors.RemoveAt(index);
	m_Colors.Insert(color, index);

	return true;
}

bool ColorList::UpdateColorName(unsigned int index, const wxString& name)
{
	if (index >= m_Colors.Count()) { return false; }

	for (unsigned int i = 0; i < m_Colors.Count(); i++)
	{
		if (i != index && name == m_Colors.Item(i).GetName())
		{
			return false;
		}
	}

	uiColor color = m_Colors.Item(index);
	color.SetName(name);
	m_Colors.RemoveAt(index);
	m_Colors.Insert(color, index);

	return true;
}

bool ColorList::UpdateColorValue(unsigned int index, unsigned int value)
{
	if (index >= m_Colors.Count()) { return false; }

	uiColor color = m_Colors.Item(index);
	color.SetValue(value);
	m_Colors.RemoveAt(index);
	m_Colors.Insert(color, index);

	return true;
}

bool ColorList::DeleteColor(unsigned int index)
{
	if (index >= m_Colors.Count()) { return false; }

	m_Colors.RemoveAt(index);

	return true;
}

bool ColorList::DeleteColor(wxString name)
{
	bool IsDelete = false;
	for (size_t i = 0; i < m_Colors.Count(); i++)
	{
		if (m_Colors.Item(i).GetName() == name)
		{
			m_Colors.RemoveAt(i);
			IsDelete = true;
			break;
		}
	}
	return IsDelete;
}

void ColorList::Empty(void)
{
	m_Colors.Empty();
}


bool ColorList::operator!=(const ColorList& rhs)
{
	if (Count() != rhs.Count()) return true;

	for (unsigned int i = 0; i < Count(); i++)
	{
		if (GetColor(i) != rhs.GetColor(i)) return true;
	}

	return false;
}
