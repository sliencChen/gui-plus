/*
 * imagelistbox.cpp
 *
 *  Created on: 2019-4-28
 *      Author: Brant
 */

#include "app_global.h"
#include "imagelistbox.h"


wxBEGIN_EVENT_TABLE(ImageListBox, wxVListBox)
//    EVT_PAINT(PageListWindow::OnPaint)
wxEND_EVENT_TABLE()


ImageListBox::ImageListBox(wxWindow *parent, wxWindowID id)
			: wxVListBox(parent, id, wxDefaultPosition, wxDefaultSize, wxLB_SINGLE | wxLB_NEEDED_SB | wxNO_FULL_REPAINT_ON_RESIZE, wxVListBoxNameStr)
{
	SetItemCount(0);
	wxLogMessage(wxT("in ImageListBox----"));
}


void ImageListBox::SetImages(ImageList* imageList)
{
//	m_ImageList = ImageList(imageList);
	m_ImageList = imageList;
	SetItemCount(m_ImageList->Count());
	wxLogMessage(wxT("in SetImages----"));
}


bool ImageListBox::AddImage(Image* image)
{
	if (m_ImageList == nullptr)
		return false;

	m_ImageList->AddImage(image);
//	{
		SetItemCount(m_ImageList->Count());
		wxLogMessage(wxT("in AddImage----"));
		return true;
//	}
//	return false;
}

bool ImageListBox::AddImage(void)
{
//	if (m_ColorList.AddColor())
//	{
//		SetItemCount(m_ColorList.Count());
//		SetSelection(m_ColorList.Count() - 1);
//		return true;
//	}
	return false;
}

Image* ImageListBox::GetImage(unsigned int index)
{
	if (m_ImageList == nullptr)
		return nullptr;

	return m_ImageList->GetImageAt(index);
}

Image* ImageListBox::GetImage(const wxString& name)
{
	if (m_ImageList == nullptr)
		return nullptr;

	Image *image = nullptr;
	for (size_t i = 0; i < m_ImageList->Count(); i++)
	{
		if (GetImage(i)->GetName() == name)
		{
			image = GetImage(i);
			break;
		}
	}
	return image;
}

void ImageListBox::SelectImage(unsigned int index)
{
	if (m_ImageList && index < m_ImageList->Count())
	{
		SetSelection(index);
	}
}

bool ImageListBox::UpdateImage(unsigned int index, Image image)
{
//	return m_ColorList.UpdateColor(index, color);
	return true;
}

bool ImageListBox::UpdateImageName(unsigned int index, const wxString& name)
{
//	return m_ColorList.UpdateColorName(index, name);
	return true;
}

//bool ImageListBox::UpdateColorValue(unsigned int index, unsigned int value)
//{
//	return m_ColorList.UpdateColorValue(index, value);
//}

void ImageListBox::DeleteImage(unsigned int index)
{
//	if (index >= m_ColorList.Count()) { return; }
//
//	m_ColorList.DeleteColor(index);
//	SetItemCount(m_ColorList.Count());
}


void ImageListBox::DeleteImage(Image *image)
{
	m_ImageList->DeleteImage(image);
	SetItemCount(m_ImageList->Count());
}


wxCoord ImageListBox::OnMeasureItem(size_t n) const
{
	return 60;
}

void ImageListBox::OnDrawItem(wxDC &dc, const wxRect &rect, size_t n) const
{
    wxLogMessage(wxT("in OnDrawItem----"));
	if (m_ImageList == nullptr)
		return;

	Image *image = m_ImageList->GetImageAt((unsigned int)n);
	wxFont font;
	wxString text;
	wxSize textSize;

	font = dc.GetFont();
	font.SetPointSize(10);
    dc.SetFont(font);
    dc.SetTextForeground(wxColour(0, 0, 0));

	text = image->GetName();
	textSize = dc.GetTextExtent(text);
	int nameTop = rect.y + (rect.height - textSize.GetHeight()) / 2 - 6;
	int nameBottom = nameTop + textSize.GetHeight();
	int nameMaxWidth = rect.GetWidth() - 100;
    while (!text.IsEmpty() && textSize.GetWidth() > nameMaxWidth)
    {
        text.RemoveLast();
        textSize = dc.GetTextExtent(text);
    }
	dc.DrawText(text, rect.x + 5, nameTop);


    font = dc.GetFont();
	font.SetPointSize(8);
    dc.SetFont(font);

	dc.SetTextForeground(wxColour(100, 100, 100));
    text = wxString::Format("%d x %d", image->GetWidth(), image->GetHeight());
    textSize = dc.GetTextExtent(text);
    dc.DrawText(text, rect.x + rect.width - textSize.GetWidth() - 5, nameTop);

    dc.SetTextForeground(wxColour(100, 100, 100));
    text = image->GetFileName();
    textSize = dc.GetTextExtent(text);
    dc.DrawText(text, rect.x + 5, nameBottom + (rect.GetBottom() - nameBottom - textSize.GetHeight())/ 2);


    /* separate line */
	dc.SetBrush(wxColour(245, 245, 245));
	dc.DrawRectangle(rect.x, rect.y + rect.height - 1, rect.width, 1);
}

