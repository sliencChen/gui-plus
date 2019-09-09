#include "app_global.h"
#include "packimagedlg.h"
#include "packimageworker.h"

//(*InternalHeaders(PackImageDialog)
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/gauge.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/string.h>
//*)

//(*IdInit(PackImageDialog)
const long PackImageDialog::ID_STATICTEXT1 = wxNewId();
const long PackImageDialog::IDC_EXPORT_PATH_LABEL = wxNewId();
const long PackImageDialog::IDC_BROWSE_BUTTON = wxNewId();
const long PackImageDialog::IDC_COMPR_SINGLE = wxNewId();
const long PackImageDialog::IDC_COMPR_ALL = wxNewId();
const long PackImageDialog::IDC_EXPORT_PROGRESS = wxNewId();
const long PackImageDialog::IDC_START_EXPORT_BUTTON = wxNewId();
//*)

BEGIN_EVENT_TABLE(PackImageDialog,wxDialog)
	//(*EventTable(PackImageDialog)
	//*)
	EVT_BUTTON(IDC_BROWSE_BUTTON, PackImageDialog::OnSelectExportPath)
	EVT_BUTTON(IDC_START_EXPORT_BUTTON, PackImageDialog::OnExport)

	EVT_THREAD(WORKER_EVENT, PackImageDialog::OnWorkerEvent)
END_EVENT_TABLE()

PackImageDialog::PackImageDialog(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(PackImageDialog)
	wxBoxSizer* BoxSizer1;
	wxBoxSizer* BoxSizer2;
	wxBoxSizer* BoxSizer3;
	wxStaticBoxSizer* StaticBoxSizer1;
	wxStaticBoxSizer* StaticBoxSizer2;

	Create(parent, id, _T("图片打包"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxMINIMIZE_BOX, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _T("导出到:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	BoxSizer2->Add(StaticText1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_ExportPathLabel = new wxStaticText(this, IDC_EXPORT_PATH_LABEL, _T("请选择导出位置"), wxDefaultPosition, wxSize(500,-1), wxDOUBLE_BORDER, _T("IDC_EXPORT_PATH_LABEL"));
	m_ExportPathLabel->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
	BoxSizer2->Add(m_ExportPathLabel, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
	m_BrowseButton = new wxButton(this, IDC_BROWSE_BUTTON, _T("浏览..."), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("IDC_BROWSE_BUTTON"));
	BoxSizer2->Add(m_BrowseButton, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(BoxSizer2, 0, wxALL|wxEXPAND, 5);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, this, _T("选项"));
	m_SingleComprCheckBox = new wxCheckBox(this, IDC_COMPR_SINGLE, _T("每张图片单独压缩"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("IDC_COMPR_SINGLE"));
	m_SingleComprCheckBox->SetValue(false);
	StaticBoxSizer1->Add(m_SingleComprCheckBox, 1, wxALL|wxEXPAND, 5);
	m_ComprAllCheckBox = new wxCheckBox(this, IDC_COMPR_ALL, _T("所有图片整体压缩"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("IDC_COMPR_ALL"));
	m_ComprAllCheckBox->SetValue(true);
	StaticBoxSizer1->Add(m_ComprAllCheckBox, 1, wxALL|wxEXPAND, 5);
	BoxSizer1->Add(StaticBoxSizer1, 0, wxALL|wxEXPAND, 5);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxVERTICAL, this, _T("进度"));
	BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
	m_ExportProgress = new wxGauge(this, IDC_EXPORT_PROGRESS, 100, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("IDC_EXPORT_PROGRESS"));
	BoxSizer3->Add(m_ExportProgress, 1, wxALL|wxEXPAND, 5);
	m_StartExportButton = new wxButton(this, IDC_START_EXPORT_BUTTON, _T("开始"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("IDC_START_EXPORT_BUTTON"));
	BoxSizer3->Add(m_StartExportButton, 0, wxALL|wxEXPAND, 5);
	StaticBoxSizer2->Add(BoxSizer3, 0, wxEXPAND, 5);
	BoxSizer1->Add(StaticBoxSizer2, 1, wxALL|wxEXPAND, 5);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	Center();
	//*)

	if (Manager::Get()->GetActiveProject())
		m_ImageList = &(Manager::Get()->GetActiveProject()->m_Images);
	else
		m_ImageList = nullptr;
}

PackImageDialog::~PackImageDialog()
{
	//(*Destroy(PackImageDialog)
	//*)
}


void PackImageDialog::OnSelectExportPath(wxCommandEvent& event)
{
	wxFileDialog saveFileDlg(this, wxT("保存"), "", "", "Designer Image Package (*.bin)|*.bin|All File (*.*)|*.*", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

	if (saveFileDlg.ShowModal() == wxID_CANCEL)
		return;

	m_ExportPath = saveFileDlg.GetPath();
    m_ExportPathLabel->SetLabel(m_ExportPath);
}

void PackImageDialog::OnExport(wxCommandEvent& event)
{
	if (m_ImageList == nullptr || m_ImageList->Count() <= 0)
	{
		wxMessageBox(wxT("没有可以打包的图片，图片列表为空！"), wxT("Designer"));
		return;
	}
	if (m_ExportPath.Length() <= 0)
	{
		wxMessageBox(wxT("请先指定导出路径！"), wxT("Designer"));
		return;
	}

	m_ComprSingle = m_SingleComprCheckBox->GetValue();
	m_ComprAll = m_ComprAllCheckBox->GetValue();

	PackImageWorkerThread *thread = new PackImageWorkerThread(this);
    if (thread->Create() != wxTHREAD_NO_ERROR)
    {
        wxLogError(wxT("创建线程失败！"));
        return;
    }
    thread->Run();
}

void PackImageDialog::OnWorkerEvent(wxThreadEvent& event)
{
	m_ExportProgress->SetValue(event.GetInt());
}

