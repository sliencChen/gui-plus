#include "app_global.h"
#include "editlogosdlg.h"

//(*InternalHeaders(EditLogosDialog)
#include "logolistbox.h"
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/statbmp.h>
#include <wx/string.h>
//*)

//(*IdInit(EditLogosDialog)
const long EditLogosDialog::IDC_SELECT_ADD_LOGO = wxNewId();
const long EditLogosDialog::IDC_SELECT_DELETE_LOGO = wxNewId();
const long EditLogosDialog::IDC_SELECT_LOGO_UP = wxNewId();
const long EditLogosDialog::IDC_SELECT_LOGO_DOWN = wxNewId();
const long EditLogosDialog::IDC_LOGO_LIST = wxNewId();
const long EditLogosDialog::IDC_LOGO_IMAGE = wxNewId();
//*)

BEGIN_EVENT_TABLE(EditLogosDialog,wxDialog)
	//(*EventTable(EditLogosDialog)
	//*)
    EVT_BUTTON(IDC_SELECT_ADD_LOGO, EditLogosDialog::OnAddLogo)
    EVT_BUTTON(IDC_SELECT_DELETE_LOGO, EditLogosDialog::OnDeleteLogo)
    EVT_BUTTON(IDC_SELECT_LOGO_UP, EditLogosDialog::OnSelectLogoUp)
    EVT_BUTTON(IDC_SELECT_LOGO_DOWN, EditLogosDialog::OnSelectLogoDown)

    EVT_LISTBOX(IDC_LOGO_LIST, EditLogosDialog::OnLogoListSelectChanged)

 //   EVT_CHOICE(IDC_LOGO_COLOR_MODE, EditLogoDialog::OnColorModeSelectChanged)
 //   EVT_CHECKBOX(IDC_LOGO_USE_KEY_COLOR, EditLogoDialog::OnUseKeyColorChanged)
END_EVENT_TABLE()

EditLogosDialog::EditLogosDialog(wxWindow* parent)
{
	//(*Initialize(EditLogosDialog)
	wxBoxSizer* BoxSizer1;
	wxBoxSizer* BoxSizer2;
	wxBoxSizer* BoxSizer3;
	wxBoxSizer* BoxSizer4;
	wxBoxSizer* BoxSizer5;
	wxStdDialogButtonSizer* StdDialogButtonSizer1;

	Create(parent, wxID_ANY, _T("编辑Logo"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxMAXIMIZE_BOX|wxMINIMIZE_BOX, _T("wxID_ANY"));
	SetClientSize(wxSize(900,600));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	BoxSizer3 = new wxBoxSizer(wxVERTICAL);
	BoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
	m_AddLogoButton = new wxButton(this, IDC_SELECT_ADD_LOGO, _T("添加"), wxDefaultPosition, wxSize(70,-1), 0, wxDefaultValidator, _T("IDC_SELECT_ADD_LOGO"));
	BoxSizer5->Add(m_AddLogoButton, 0, wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_DeleteLogoButton = new wxButton(this, IDC_SELECT_DELETE_LOGO, _T("删除"), wxDefaultPosition, wxSize(70,-1), 0, wxDefaultValidator, _T("IDC_SELECT_DELETE_LOGO"));
	BoxSizer5->Add(m_DeleteLogoButton, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_SelectLogoUpButton = new wxButton(this, IDC_SELECT_LOGO_UP, _T("上移"), wxDefaultPosition, wxSize(70,-1), 0, wxDefaultValidator, _T("IDC_SELECT_LOGO_UP"));
	BoxSizer5->Add(m_SelectLogoUpButton, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_SelectLogoDownButton = new wxButton(this, IDC_SELECT_LOGO_DOWN, _T("下移"), wxDefaultPosition, wxSize(70,-1), 0, wxDefaultValidator, _T("IDC_SELECT_LOGO_DOWN"));
	BoxSizer5->Add(m_SelectLogoDownButton, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer3->Add(BoxSizer5, 0, wxEXPAND, 5);
	m_LogoList = new LogoListBox(this,IDC_LOGO_LIST);
	BoxSizer3->Add(m_LogoList, 1, wxTOP|wxEXPAND, 5);
	BoxSizer2->Add(BoxSizer3, 0, wxALL|wxEXPAND, 5);
	BoxSizer4 = new wxBoxSizer(wxVERTICAL);
	m_PreviewLogo = new wxStaticBitmap(this, IDC_LOGO_IMAGE, wxNullBitmap, wxDefaultPosition, wxSize(500,450), wxSIMPLE_BORDER, _T("IDC_LOGO_IMAGE"));
	BoxSizer4->Add(m_PreviewLogo, 1, wxEXPAND, 5);
	BoxSizer2->Add(BoxSizer4, 2, wxALL|wxEXPAND, 5);
	BoxSizer1->Add(BoxSizer2, 1, wxALL|wxEXPAND, 5);
	StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_OK, _T("确定")));
	StdDialogButtonSizer1->Realize();
	BoxSizer1->Add(StdDialogButtonSizer1, 0, wxALL|wxALIGN_RIGHT, 5);
	SetSizer(BoxSizer1);
	SetSizer(BoxSizer1);
	Layout();
	Center();
	//*)
	m_LogoList->SetLogo(&(Manager::Get()->GetActiveProject()->m_LogoImage));
}

EditLogosDialog::~EditLogosDialog()
{
	//(*Destroy(EditLogosDialog)
	//*)
}


//-------------------------------------------------------------------------------------------------
// Edit logo Function
//-------------------------------------------------------------------------------------------------
void EditLogosDialog::OnAddLogo(wxCommandEvent& event)
{
    wxFileDialog fileDialog(this, wxT("选择图片"), wxEmptyString, wxEmptyString,
                            wxT("所有图片文件|*.bmp;*.jpg;*.jpeg;*.tif;*.tiff;*.png|位图文件(*.bmp)|*.bmp|JPEG(*.jpg;*.jpeg)|*.jpg;*.jpeg|PNG(*.png)|*.png|TIF(*.tif;*.tiff)|*.tif;*.tiff|所有文件|*.*"),
                            wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_MULTIPLE);
    fileDialog.ShowModal();

    wxArrayString paths;
    wxArrayString filenames;
    fileDialog.GetPaths(paths);
    fileDialog.GetFilenames(filenames);

	Logo *item;
	wxString addStr;
	int action;
	wxMessageDialog askSave(this, addStr, wxT("添加Logo"), wxYES_NO|wxNO_DEFAULT|wxICON_QUESTION);
	askSave.SetYesNoLabels(wxT("是"), wxT("否"));

    for (size_t i = 0; i < paths.GetCount(); i++)
    {
    	item = m_LogoList->GetLogo(filenames.Item(i));
		if (nullptr != item)
		{
			addStr = filenames.Item(i);
			addStr += wxT("已存在，是否覆盖它？");
			askSave.SetMessage(addStr);
			action = askSave.ShowModal();
			if (wxID_NO == action)
			{
				continue;
			}
			else if (wxID_YES == action)
			{
				m_LogoList->DeleteLogo(item);
			}
		}
        m_LogoList->AddLogo(new Logo(paths.Item(i), filenames.Item(i)));
    }
    m_LogoList->RefreshAll();
}

void EditLogosDialog::OnLogoListSelectChanged(wxCommandEvent& event)
{
    m_CurrentLogo = m_LogoList->GetLogo(event.GetSelection());
    ShowLogo(m_CurrentLogo);
    m_LogoList->Refresh();
}

void EditLogosDialog::OnDeleteLogo(wxCommandEvent& event)
{
	Logo *selectDelete = nullptr;
	int nextSelect = 0;
	selectDelete = m_LogoList->GetLogo(m_LogoList->GetSelection());
	if (selectDelete)
	{
	    m_LogoList->DeleteLogo(selectDelete);
	    m_LogoList->RefreshAll();
	    nextSelect = m_LogoList->GetSelection() + 1;
	    if (nextSelect >= m_LogoList->GetItemCount())
	    {
	    	nextSelect = 0;
	    }
	    ShowLogo(m_LogoList->GetLogo(nextSelect));
		Manager::Get()->GetActiveProject()->m_LogoImage.ApplyToGUISystem();
		Manager::Get()->GetActiveProject()->MarkAsModified();
	}
}

void EditLogosDialog::OnSelectLogoUp(wxCommandEvent& event)
{
	int upSelect = 0;
    m_CurrentLogo = m_LogoList->GetLogo(m_LogoList->GetSelection());
    m_LogoList->UpLogo(m_CurrentLogo);
    upSelect = m_LogoList->GetSelection() - 1;
    if (upSelect < 0)
    {
    	upSelect = m_LogoList->GetItemCount() - 1;
    }
    m_LogoList->SetSelection(upSelect);
    m_LogoList->RefreshAll();
}

void EditLogosDialog::OnSelectLogoDown(wxCommandEvent& event)
{
	int downSelect = 0;
    m_CurrentLogo = m_LogoList->GetLogo(m_LogoList->GetSelection());
    m_LogoList->DownLogo(m_CurrentLogo);
    downSelect = m_LogoList->GetSelection() + 1;
    if (downSelect >= m_LogoList->GetItemCount())
    {
    	downSelect = 0;
    }
    m_LogoList->SetSelection(downSelect);
    m_LogoList->RefreshAll();
}

void EditLogosDialog::ShowLogo(Logo* logo)
{
    if (!logo || !logo->IsOK())
        return;

	wxSize previewSize = m_PreviewLogo->GetSize();
	unsigned int *pixels = new unsigned int[previewSize.GetWidth() * previewSize.GetHeight()];
	logo->GetPreviewPixels(pixels, previewSize, m_PreviewLogo->GetBackgroundColour());
	m_PreviewLogo->SetBitmap(wxBitmap((const char *)pixels, previewSize.GetWidth(), previewSize.GetHeight(), 32));
	delete [] pixels;
}


