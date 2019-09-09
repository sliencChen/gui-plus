/*
 * colorlistbox.cpp
 *
 *  Created on: 2019-4-15
 *      Author: Brant
 */

#include "app_global.h"
#include "colorlistbox.h"


wxBEGIN_EVENT_TABLE(ColorListBox, wxVListBox)
//    EVT_PAINT(PageListWindow::OnPaint)
wxEND_EVENT_TABLE()


ColorListBox::ColorListBox(wxWindow *parent, wxWindowID id)
			  : wxVListBox(parent, id, wxDefaultPosition, wxDefaultSize, wxLB_SINGLE | wxLB_NEEDED_SB | wxNO_FULL_REPAINT_ON_RESIZE, wxVListBoxNameStr)
{
	SetItemCount(0);
}


void ColorListBox::SetColors(const ColorList& colorList)
{
	m_ColorList = ColorList(colorList);
	SetItemCount(m_ColorList.Count());
}


bool ColorListBox::AddColor(uiColor color)
{
	if (m_ColorList.AddColor(color))
	{
		SetItemCount(m_ColorList.Count());
		return true;
	}
	return false;
}

bool ColorListBox::AddColor(void)
{
	if (m_ColorList.AddColor())
	{
		SetItemCount(m_ColorList.Count());
		SetSelection(m_ColorList.Count() - 1);
		return true;
	}
	return false;
}

uiColor ColorListBox::GetColor(unsigned int index)
{
	return m_ColorList.GetColor(index);
}

void ColorListBox::SelectColor(unsigned int index)
{
	if (index < m_ColorList.Count())
	{
		SetSelection(index);
	}
}

bool ColorListBox::UpdateColor(unsigned int index, uiColor color)
{
	return m_ColorList.UpdateColor(index, color);
}

bool ColorListBox::UpdateColorName(unsigned int index, const wxString& name)
{
	return m_ColorList.UpdateColorName(index, name);
}

bool ColorListBox::UpdateColorValue(unsigned int index, unsigned int value)
{
	return m_ColorList.UpdateColorValue(index, value);
}

void ColorListBox::DeleteColor(unsigned int index)
{
	if (index >= m_ColorList.Count()) { return; }

	m_ColorList.DeleteColor(index);
	SetItemCount(m_ColorList.Count());
}


wxCoord ColorListBox::OnMeasureItem(size_t n) const
{
	return 60;
}

void ColorListBox::OnDrawItem(wxDC &dc, const wxRect &rect, size_t n) const
{
	uiColor color = m_ColorList.GetColor((unsigned int)n);
	wxColour wxcolor((unsigned char)((color.m_Value & 0xff0000) >> 16), (unsigned char)((color.m_Value & 0xff00) >> 8),
			(unsigned char)(color.m_Value & 0xff), (unsigned char)((color.m_Value & 0xff000000) >> 24));

	dc.SetBrush(wxcolor);
	dc.DrawRectangle(rect.x + 10, rect.y + 10, 40, 40);

	wxString name = color.GetName();
	wxSize textSize;
	textSize = dc.GetTextExtent(color.GetName());
	dc.DrawText(name, rect.x + 60, rect.y + (rect.height - textSize.GetHeight()) / 2);

	dc.SetBrush(wxColour(245, 245, 245));
	dc.DrawRectangle(rect.x, rect.y + rect.height - 1, rect.width, 1);
}
