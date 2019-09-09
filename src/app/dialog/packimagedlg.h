#ifndef PACKIMAGEDLG_H
#define PACKIMAGEDLG_H

//(*Headers(PackImageDialog)
#include <wx/dialog.h>
class wxBoxSizer;
class wxButton;
class wxCheckBox;
class wxGauge;
class wxStaticBoxSizer;
class wxStaticText;
//*)

enum
{
	WORKER_EVENT = wxID_HIGHEST+1,
	THREAD_EVENT_MAX
};

class PackImageDialog: public wxDialog
{
	public:

		PackImageDialog(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~PackImageDialog();

		//(*Declarations(PackImageDialog)
		wxButton* m_BrowseButton;
		wxButton* m_StartExportButton;
		wxCheckBox* m_ComprAllCheckBox;
		wxCheckBox* m_SingleComprCheckBox;
		wxGauge* m_ExportProgress;
		wxStaticText* StaticText1;
		wxStaticText* m_ExportPathLabel;
		//*)

		wxString m_ExportPath;
		bool m_ComprSingle = false;
		bool m_ComprAll = false;
		ImageList *m_ImageList;


	protected:

		//(*Identifiers(PackImageDialog)
		static const long ID_STATICTEXT1;
		static const long IDC_EXPORT_PATH_LABEL;
		static const long IDC_BROWSE_BUTTON;
		static const long IDC_COMPR_SINGLE;
		static const long IDC_COMPR_ALL;
		static const long IDC_EXPORT_PROGRESS;
		static const long IDC_START_EXPORT_BUTTON;
		//*)

	private:

		//(*Handlers(PackImageDialog)
		//*)
		void OnSelectExportPath(wxCommandEvent& event);
		void OnExport(wxCommandEvent& event);
		void OnWorkerEvent(wxThreadEvent& event);

		void UpdateExportStatus(const wxString &status);

		DECLARE_EVENT_TABLE()
};

#endif
