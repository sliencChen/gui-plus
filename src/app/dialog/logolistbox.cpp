#include "app_global.h"
#include "logolistbox.h"

wxBEGIN_EVENT_TABLE(LogoListBox, wxVListBox)
//    EVT_PAINT(PageListWindow::OnPaint)
wxEND_EVENT_TABLE()


LogoListBox::LogoListBox(wxWindow *parent, wxWindowID id)
			: wxVListBox(parent, id, wxDefaultPosition, wxDefaultSize, wxLB_SINGLE | wxLB_NEEDED_SB | wxNO_FULL_REPAINT_ON_RESIZE, wxVListBoxNameStr)
{
	SetItemCount(0);
}


//-------------------------------------------------------------------------------------------------
// LogoList Function
//-------------------------------------------------------------------------------------------------
void LogoListBox::SetLogo(LogoList* logoList)
{
	m_LogoImage = logoList;
	SetItemCount(m_LogoImage->Count());
}


bool LogoListBox::AddLogo(Logo* logo)
{
	if (m_LogoImage == nullptr)
		return false;

	m_LogoImage->AddLogo(logo);
	
	SetItemCount(m_LogoImage->Count());
	return true;
}

bool LogoListBox::DeleteLogo(Logo *logo)
{

	if (m_LogoImage == nullptr || logo == nullptr)
		return false;

	m_LogoImage->DeleteLogo(logo);
	
	SetItemCount(m_LogoImage->Count());
	return true;
}

bool LogoListBox::UpLogo(Logo *logo)
{
    if (m_LogoImage == nullptr)
		return false;

	m_LogoImage->UpLogo(logo);
    SetItemCount(m_LogoImage->Count());
    return true;
}

bool LogoListBox::DownLogo(Logo *logo)
{
    if (m_LogoImage == nullptr)
		return false;

	m_LogoImage->DownLogo(logo);
    SetItemCount(m_LogoImage->Count());
    return true;
}

Logo* LogoListBox::GetLogo(unsigned int index)
{
	if (m_LogoImage == nullptr)
		return nullptr;

	return m_LogoImage->GetLogoAt(index);
}

Logo* LogoListBox::GetLogo(const wxString& logoName)
{
	if (m_LogoImage == nullptr)
		return nullptr;

	Logo *logo = nullptr;
	for (size_t i = 0; i < m_LogoImage->Count(); i++)
	{
		if (GetLogo(i)->GetName() == logoName)
		{
			logo = GetLogo(i);
			break;
		}
	}
	return logo;
}

wxCoord LogoListBox::OnMeasureItem(size_t n) const
{
	return 60;
}

void LogoListBox::OnDrawItem(wxDC &dc, const wxRect &rect, size_t n) const
{
	if (m_LogoImage == nullptr)
		return;

	Logo *image = m_LogoImage->GetLogoAt((unsigned int)n);
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

    dc.SetTextForeground(wxColour(100, 100, 100));
    text = image->GetFileName();
    textSize = dc.GetTextExtent(text);
    dc.DrawText(text, rect.x + 5, nameBottom + (rect.GetBottom() - nameBottom - textSize.GetHeight())/ 2);


    /* separate line */
	dc.SetBrush(wxColour(245, 245, 245));
	dc.DrawRectangle(rect.x, rect.y + rect.height - 1, rect.width, 1);
}

