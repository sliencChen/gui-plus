#ifndef EDITLOGOSDLG_H
#define EDITLOGOSDLG_H

//(*Headers(EditLogosDialog)
#include <wx/dialog.h>
class LogoListBox;
class wxBoxSizer;
class wxButton;
class wxStaticBitmap;
class wxStdDialogButtonSizer;
//*)
#include "manager.h"
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/statbmp.h>
#include <wx/checkbox.h>
#include <wx/choice.h>

class EditLogosDialog: public wxDialog
{
	public:

		EditLogosDialog(wxWindow* parent);
		virtual ~EditLogosDialog();

		//(*Declarations(EditLogosDialog)
		LogoListBox* m_LogoList;
		wxButton* m_AddLogoButton;
		wxButton* m_DeleteLogoButton;
		wxButton* m_SelectLogoDownButton;
		wxButton* m_SelectLogoUpButton;
		wxStaticBitmap* m_PreviewLogo;
		//*)

	protected:

		//(*Identifiers(EditLogosDialog)
		static const long IDC_SELECT_ADD_LOGO;
		static const long IDC_SELECT_DELETE_LOGO;
		static const long IDC_SELECT_LOGO_UP;
		static const long IDC_SELECT_LOGO_DOWN;
		static const long IDC_LOGO_LIST;
		static const long IDC_LOGO_IMAGE;
		//*)

	private:

		//(*Handlers(EditLogosDialog)
		//*)
		void OnAddLogo(wxCommandEvent& event);
        void OnLogoListSelectChanged(wxCommandEvent& event);
        void OnDeleteLogo(wxCommandEvent& event);
        void OnSelectLogoUp(wxCommandEvent& event);
        void OnSelectLogoDown(wxCommandEvent& event);

        void ShowLogo(Logo* logo);

		Logo *m_CurrentLogo = nullptr;

		DECLARE_EVENT_TABLE()
};

#endif
