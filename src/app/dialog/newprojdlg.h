#ifndef NEWPROJDLG_H
#define NEWPROJDLG_H

//(*Headers(NewProjectDialog)
#include <wx/dialog.h>
class wxBoxSizer;
class wxChoice;
class wxStaticBoxSizer;
class wxStaticText;
class wxStdDialogButtonSizer;
class wxTextCtrl;
//*)

class NewProjectDialog: public wxDialog
{
	public:

		NewProjectDialog(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~NewProjectDialog();

		//(*Declarations(NewProjectDialog)
		wxChoice* choiceProjectColor;
		wxStaticText* StaticText2;
		wxStaticText* StaticText3;
		wxStaticText* staticProjectName;
		wxStaticText* staticProjectWidth;
		wxTextCtrl* textProjectHeight;
		wxTextCtrl* textProjectName;
		wxTextCtrl* textProjectWidth;
		//*)

		wxString m_ProjectName;
		int m_CanvasWidth;
		int m_CanvasHeight;
		int m_ColorMode;

	protected:

		//(*Identifiers(NewProjectDialog)
		static const long ID_STATICTEXT1;
		static const long ID_TEXTCTRL1;
		static const long ID_STATICTEXT2;
		static const long IDC_NEW_PROJ_DLG_WIDTH;
		static const long ID_STATICTEXT3;
		static const long IDC_NEW_PROJ_DLG_HEIGHT;
		static const long ID_STATICTEXT4;
		static const long IDC_NEW_PROJ_DLG_COLOR_MODE;
		//*)

	private:

		//(*Handlers(NewProjectDialog)
		//*)
		
		void OnOK(wxCommandEvent& event);

		DECLARE_EVENT_TABLE()
};

#endif
