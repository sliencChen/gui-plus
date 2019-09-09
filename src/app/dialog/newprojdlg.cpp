#include "app_global.h"
#include "newprojdlg.h"

//(*InternalHeaders(NewProjectDialog)
#include <wx/button.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textctrl.h>
//*)

//(*IdInit(NewProjectDialog)
const long NewProjectDialog::ID_STATICTEXT1 = wxNewId();
const long NewProjectDialog::ID_TEXTCTRL1 = wxNewId();
const long NewProjectDialog::ID_STATICTEXT2 = wxNewId();
const long NewProjectDialog::IDC_NEW_PROJ_DLG_WIDTH = wxNewId();
const long NewProjectDialog::ID_STATICTEXT3 = wxNewId();
const long NewProjectDialog::IDC_NEW_PROJ_DLG_HEIGHT = wxNewId();
const long NewProjectDialog::ID_STATICTEXT4 = wxNewId();
const long NewProjectDialog::IDC_NEW_PROJ_DLG_COLOR_MODE = wxNewId();
//*)

BEGIN_EVENT_TABLE(NewProjectDialog,wxDialog)
	//(*EventTable(NewProjectDialog)
	//*)

	EVT_BUTTON(wxID_OK, NewProjectDialog::OnOK)

END_EVENT_TABLE()

NewProjectDialog::NewProjectDialog(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(NewProjectDialog)
	wxBoxSizer* BoxSizer2;
	wxBoxSizer* sizerProjName;
	wxBoxSizer* sizerProperty1;
	wxBoxSizer* topSizer;
	wxStaticBoxSizer* sizerProjectProperty;
	wxStdDialogButtonSizer* StdDialogButtonSizer1;

	Create(parent, wxID_ANY, _T("新建工程"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	topSizer = new wxBoxSizer(wxVERTICAL);
	sizerProjName = new wxBoxSizer(wxHORIZONTAL);
	staticProjectName = new wxStaticText(this, ID_STATICTEXT1, _T("工程名称:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	sizerProjName->Add(staticProjectName, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	textProjectName = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	textProjectName->SetMaxLength(32);
	sizerProjName->Add(textProjectName, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	topSizer->Add(sizerProjName, 0, wxALL|wxEXPAND, 5);
	sizerProjectProperty = new wxStaticBoxSizer(wxVERTICAL, this, _T("工程属性"));
	sizerProperty1 = new wxBoxSizer(wxHORIZONTAL);
	staticProjectWidth = new wxStaticText(this, ID_STATICTEXT2, _T("宽度:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	sizerProperty1->Add(staticProjectWidth, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	textProjectWidth = new wxTextCtrl(this, IDC_NEW_PROJ_DLG_WIDTH, _T("1280"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("IDC_NEW_PROJ_DLG_WIDTH"));
	textProjectWidth->SetMaxLength(10);
	sizerProperty1->Add(textProjectWidth, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	sizerProperty1->Add(10,-1,0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT3, _T("高度:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	sizerProperty1->Add(StaticText2, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	textProjectHeight = new wxTextCtrl(this, IDC_NEW_PROJ_DLG_HEIGHT, _T("720"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("IDC_NEW_PROJ_DLG_HEIGHT"));
	textProjectHeight->SetMaxLength(10);
	sizerProperty1->Add(textProjectHeight, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	sizerProjectProperty->Add(sizerProperty1, 1, wxALL|wxEXPAND, 5);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT4, _T("颜色格式:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	BoxSizer2->Add(StaticText3, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	choiceProjectColor = new wxChoice(this, IDC_NEW_PROJ_DLG_COLOR_MODE, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("IDC_NEW_PROJ_DLG_COLOR_MODE"));
	choiceProjectColor->SetSelection( choiceProjectColor->Append(_T("ARGB8888 (32位)")) );
	choiceProjectColor->Append(_T("ARGB1555 (16位)"));
	choiceProjectColor->Append(_T("RGB565 (16位)"));
	BoxSizer2->Add(choiceProjectColor, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	sizerProjectProperty->Add(BoxSizer2, 1, wxALL|wxEXPAND, 5);
	topSizer->Add(sizerProjectProperty, 1, wxALL|wxEXPAND, 5);
	StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_OK, _T("确定")));
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_CANCEL, _T("取消")));
	StdDialogButtonSizer1->Realize();
	topSizer->Add(StdDialogButtonSizer1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(topSizer);
	topSizer->Fit(this);
	topSizer->SetSizeHints(this);
	Center();
	//*)
}

NewProjectDialog::~NewProjectDialog()
{
	//(*Destroy(NewProjectDialog)
	//*)
}

void NewProjectDialog::OnOK(wxCommandEvent& event)
{
    m_ProjectName = textProjectName->GetValue().Trim();
	if (m_ProjectName.IsEmpty())
	{
	    wxMessageBox(wxT("请输入工程名称"), wxT(APP_NAME), wxOK | wxICON_ERROR, this);
	    return;
	}

	unsigned long width, height;
    if (!textProjectWidth->GetValue().ToULong(&width) || width <= 0 || width > 65535
        || !textProjectHeight->GetValue().ToULong(&height) || height <= 0 || height > 65535)
    {
        wxMessageBox(wxT("请输入 1~65535 之间的数值"), wxT(APP_NAME), wxOK | wxICON_ERROR, this);
        return;
    }
    m_CanvasWidth = (int)width;
    m_CanvasHeight = (int)height;

    m_ColorMode = choiceProjectColor->GetSelection();

    EndModal(wxID_OK);
}
