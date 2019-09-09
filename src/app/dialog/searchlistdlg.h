#ifndef SEARCHLISTDLG_H
#define SEARCHLISTDLG_H

//(*Headers(SearchListDialog)
#include <wx/dialog.h>
class SearchListBox;
class wxBoxSizer;
class wxButton;
class wxSearchCtrl;
//*)

class SearchListDialog: public wxDialog
{
	public:

		SearchListDialog(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~SearchListDialog();

		//(*Declarations(SearchListDialog)
		SearchListBox* m_SearchList;
		wxButton* SearchButton;
		wxSearchCtrl* SearchCtrl1;
		//*)

	protected:

		//(*Identifiers(SearchListDialog)
		static const long ID_SEARCHCTRL1;
		static const long ID_SEARCH_BUTTON;
		static const long ID_SEARCH_LIST;
		//*)

	private:

		//(*Handlers(SearchListDialog)
		//*)

		void OnSearch(wxCommandEvent& event);
		DECLARE_EVENT_TABLE()
};

#endif
