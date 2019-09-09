/*
 * pagelist.cpp
 *
 *  Created on: 2019-4-3
 *      Author: Brant
 */

#include "app_global.h"
#include "pagefile.h"
#include "pagelist.h"
#include "searchlistbox.h"
#include "window/propname.h"
#include "property.h"


wxBEGIN_EVENT_TABLE(SearchListBox, wxVListBox)
//    EVT_PAINT(SearchListBox::OnPaint)
wxEND_EVENT_TABLE()


SearchListBox::SearchListBox(wxWindow *parent, wxWindowID id)
			  : wxVListBox(parent, id, wxDefaultPosition, wxDefaultSize, wxLB_SINGLE | wxLB_NEEDED_SB | wxNO_FULL_REPAINT_ON_RESIZE, wxVListBoxNameStr)
{
    SetItemCount(0);
}


void SearchListBox::ReloadAllData(void)
{
	RefreshAll();
}

void SearchListBox::ClearAllData(void)
{
    SearchResultItems.clear();
}

wxCoord SearchListBox::OnMeasureItem(size_t n) const
{
    return 60;
}

void SearchListBox::AddResult(SearchResultItem *resultitem)
{
	SearchResultItems.push_back(resultitem);
	SetItemCount(GetResultItemCount());
}

void SearchListBox::SetActivePage(PageFile* pagefile)
{
    if(pagefile)
        m_ActivePage = pagefile;

	if (m_ActivePage && m_ActivePage->GetRootWindow())
	{
		m_ActiveWindow = m_ActiveRootWindow = m_ActivePage->GetRootWindow();
	}
}

//unsigned int SearchListBox::GetResultItemCount(void)
//{
//    return SearchResultItems.size();
//}

//SearchResultItem* SearchListBox::GetResultItemAt(unsigned int index)
//{
//	if (index < GetResultItemCount())
//	{
//		return SearchResultItems.at(index);
//	}
//	return nullptr;
//}

void SearchListBox::SearchText(wxString text)
{
	wxString propety;
	int count = 0;
	unsigned int index;

    Project *proj = Manager::Get()->GetActiveProject();

    //wxLogMessage("page num = %d",proj->GetPageFileCount());
    for (index = 0;index<proj->GetPageFileCount();index++)
    {
        SetActivePage(proj->GetPageFileAt(index));
        uiWindow *window = m_ActiveRootWindow->m_Childs;
        while (window)
        {
//            wxLogMessage("PROP_CLASS = %s,PROP_NAME = %s,PROP_VAR_NAME = %s,PROP_TEXT = %s,PROP_IMAGE_NAME = %s",
//                     window->GetStrPropValue(PROP_CLASS),window->GetStrPropValue(PROP_NAME),window->GetStrPropValue(PROP_VAR_NAME),window->GetStrPropValue(PROP_TEXT),window->GetStrPropValue(PROP_IMAGE_NAME));
            propety = dsC2U(window->GetStrPropValue(PROP_CLASS)) + dsC2U(window->GetStrPropValue(PROP_NAME));// + dsC2U(window->GetStrPropValue(PROP_TEXT)) + dsC2U(window->GetStrPropValue(PROP_IMAGE_NAME));
            //wxLogMessage("propety =%s,text = %s",propety,text);
            if(propety.find(text) != wxString::npos)
            {
                count++;
//                wxLogMessage("count = %d",count);

                SearchResultItem *resultItem = new SearchResultItem();
                resultItem->pagefile = m_ActivePage;
                resultItem->window = window;
                resultItem->propety = dsC2U(window->GetStrPropValue(PROP_CLASS));
                AddResult(resultItem);
            }
            window = window->m_Next;
        }
    }
}

void SearchListBox::OnDrawItem(wxDC &dc, const wxRect &rect, size_t n) const
{
    if(0 == GetResultItemCount())
        return;
    SearchResultItem *ResultItem;
    wxFont font;
	wxString text;
	wxSize textSize;

	font = dc.GetFont();
	font.SetPointSize(10);
    dc.SetFont(font);
    dc.SetTextForeground(wxColour(0, 0, 0));

    ResultItem = GetResultItemAt((unsigned int)n);//SearchResultItems.at((unsigned int)n);//
    text = ResultItem->pagefile->GetFileName() + ' ' + dsC2U(ResultItem->window->GetStrPropValue(PROP_CLASS)) + ' ' + ResultItem->window->GetName();
    textSize = dc.GetTextExtent(text);
	int nameTop = rect.y + (rect.height - textSize.GetHeight()) / 2 - 6;
	int nameMaxWidth = rect.GetWidth() - 100;
    while (!text.IsEmpty() && textSize.GetWidth() > nameMaxWidth)
    {
        text.RemoveLast();
        textSize = dc.GetTextExtent(text);
    }
	dc.DrawText(text, rect.x + 5, nameTop);
}
