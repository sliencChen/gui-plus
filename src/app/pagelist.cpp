/*
 * pagelist.cpp
 *
 *  Created on: 2019-4-3
 *      Author: Brant
 */

#include "app_global.h"
#include "pagelist.h"


DEFINE_EVENT_TYPE(PAGELIST_EVT_COMMAND_DELETE)
wxIMPLEMENT_DYNAMIC_CLASS(PageListEvent, wxCommandEvent)

wxBEGIN_EVENT_TABLE(PageListWindow, wxVListBox)
	EVT_UPDATE_UI(IDC_PAGELIST_DELETE_EVENT, PageListWindow::OnUpdateUIRightDown)
	EVT_CONTEXT_MENU(PageListWindow::OnRightDownMenu)
	EVT_MENU(IDC_PAGELIST_DELETE_EVENT, PageListWindow::OnDeletePage)
//    EVT_PAINT(PageListWindow::OnPaint)
wxEND_EVENT_TABLE()


PageListWindow::PageListWindow(wxWindow *parent, wxWindowID id)
			  : wxVListBox(parent, id, wxDefaultPosition, wxDefaultSize, wxLB_SINGLE | wxLB_NEEDED_SB | wxNO_FULL_REPAINT_ON_RESIZE, wxVListBoxNameStr)
{
}

void PageListWindow::OnUpdateUIRightDown(wxUpdateUIEvent& event)
{
	if (Manager::Get()->GetActiveProject())
	{
		PageFile *pagefile = Manager::Get()->GetActiveProject()->GetActivePageFile();
		if (!pagefile)
		{
			event.Enable(false);
		}
	}
	else
	{
		event.Enable(false);
	}
}

void PageListWindow::OnRightDownMenu(wxContextMenuEvent& event)
{
    wxMenu menu;
    menu.Append(IDC_PAGELIST_DELETE_EVENT, "É¾³ý");
    PopupMenu(&menu, ScreenToClient(event.GetPosition()));
}

void PageListWindow::ReloadAllData(void)
{
	if (Manager::Get()->GetActiveProject())
		SetItemCount(Manager::Get()->GetActiveProject()->GetPageFileCount());
	else
		SetItemCount(0);

	RefreshAll();
}

void PageListWindow::ReloadCurrent(void)
{
	RefreshSelected();
}


void PageListWindow::SelectFirst(void)
{
	if (GetItemCount() > 0)
	{
		SetSelection(0);
	}
}

void PageListWindow::SelectLast(void)
{
	if (GetItemCount() > 0)
	{
		SetSelection(GetItemCount() - 1);
	}
}


wxCoord PageListWindow::OnMeasureItem(size_t n) const
{
	int width = GetSize().GetWidth();

	return width * 720 / 1280 + 30;
}

void PageListWindow::OnDrawItem(wxDC &dc, const wxRect &rect, size_t n) const
{
	if (!(Manager::Get()->GetActiveProject()))
		return;

	PageFile *pagefile = Manager::Get()->GetActiveProject()->GetPageFileAt((unsigned int)n);
	if (!pagefile) return;

	uiWindow *page = pagefile->GetRootWindow();
    if (!page)
    {
        dc.DrawBitmap(wxBITMAP_PNG(FILE_LOST_ICON), rect.x + 5, rect.y + 5, false);
        wxSize textSize;
        textSize = dc.GetTextExtent(pagefile->GetFileName());
        dc.DrawText(pagefile->GetFileName(), rect.x + 5 + (rect.width - 10 - textSize.GetWidth()) / 2, rect.y + rect.height - 30);
        return;
    }

	int x = 5, y = 5;
	int bmpWidth = rect.width - 10;
	int bmpHeight = rect.height - 10 - 30;
	unsigned int *renderBmp = new unsigned int[bmpWidth * bmpHeight];

	uiRender::GetBitmap(page, renderBmp, bmpWidth, bmpHeight);

	wxBitmap pageBitmap((const char *)renderBmp, bmpWidth, bmpHeight, 32);
	dc.DrawBitmap(pageBitmap, rect.x + x, rect.y + y, false);

	wxSize textSize;
	wxString name = page->Id();

	if (pagefile->GetModified())
		name += "*";

	textSize = dc.GetTextExtent(name);
	dc.DrawText(name, rect.x + x + (bmpWidth - textSize.GetWidth()) / 2, rect.y + rect.height - 30);

	delete [] renderBmp;
}


void PageListWindow::OnDeletePage(wxCommandEvent& event)
{
	PageListEvent canvasEvt(PAGELIST_EVT_COMMAND_DELETE, GetId());
	canvasEvt.SetEventObject(this);
	GetEventHandler()->ProcessEvent(canvasEvt);
}
