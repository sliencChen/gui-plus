#include "searchlistdlg.h"
#include "app_global.h"
#include <wx/wx.h>
//(*InternalHeaders(SearchListDialog)
#include "searchlistbox.h"
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/srchctrl.h>
#include <wx/string.h>
//*)

//(*IdInit(SearchListDialog)
const long SearchListDialog::ID_SEARCHCTRL1 = wxNewId();
const long SearchListDialog::ID_SEARCH_BUTTON = wxNewId();
const long SearchListDialog::ID_SEARCH_LIST = wxNewId();
//*)

BEGIN_EVENT_TABLE(SearchListDialog,wxDialog)
	//(*EventTable(SearchListDialog)
	//*)
	EVT_BUTTON(ID_SEARCH_BUTTON, SearchListDialog::OnSearch)
END_EVENT_TABLE()

SearchListDialog::SearchListDialog(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(SearchListDialog)
	wxBoxSizer* BoxSizer1;
	wxBoxSizer* BoxSizer2;
	wxBoxSizer* BoxSizer3;

	Create(parent, id, _T("查找"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
	SetClientSize(wxSize(500,500));
	Move(wxDefaultPosition);
	SetMinSize(wxSize(500,-1));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	SearchCtrl1 = new wxSearchCtrl(this, ID_SEARCHCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_SEARCHCTRL1"));
	BoxSizer2->Add(SearchCtrl1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SearchButton = new wxButton(this, ID_SEARCH_BUTTON, _T("Search"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_SEARCH_BUTTON"));
	BoxSizer2->Add(SearchButton, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(BoxSizer2, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
	m_SearchList = new SearchListBox(this,ID_SEARCH_LIST);
	BoxSizer3->Add(m_SearchList, 1, wxTOP|wxEXPAND, 5);
	BoxSizer1->Add(BoxSizer3, 1, wxALL|wxEXPAND, 5);
	SetSizer(BoxSizer1);
	SetSizer(BoxSizer1);
	Layout();
	//*)
}

SearchListDialog::~SearchListDialog()
{
	//(*Destroy(SearchListDialog)
	//*)
}

void SearchListDialog::OnSearch(wxCommandEvent& event)
{
    wxString text;

    text = SearchCtrl1->GetValue();
    if(0 == text.size())
    {
        return;
    }

     m_SearchList->ClearAllData();
     m_SearchList->SearchText(text);
     m_SearchList->ReloadAllData();
}
