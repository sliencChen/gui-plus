#include "app_global.h"
#include <wx/colordlg.h>
#include "editimagedlg.h"

//(*InternalHeaders(EditImageDialog)
#include "imagelistbox.h"
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/scrolwin.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/spinctrl.h>
#include <wx/srchctrl.h>
#include <wx/statbmp.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/string.h>
//*)

//(*IdInit(EditImageDialog)
const long EditImageDialog::ID_SEARCHCTRL1 = wxNewId();
const long EditImageDialog::IDC_ADD_BUTTON = wxNewId();
const long EditImageDialog::IDC_DELETE_BUTTON = wxNewId();
const long EditImageDialog::IDC_IMAGE_LIST = wxNewId();
const long EditImageDialog::IDC_PREVIEW_IMAGE = wxNewId();
const long EditImageDialog::IDC_PREVIEW_IMAGE_CONTAINER = wxNewId();
const long EditImageDialog::IDC_SCALE_DOWN_BUTTON = wxNewId();
const long EditImageDialog::IDC_SCALE_SLIDER = wxNewId();
const long EditImageDialog::IDC_SCALE_LABEL = wxNewId();
const long EditImageDialog::IDC_SCALE_UP_BUTTON = wxNewId();
const long EditImageDialog::IDC_SCALE_ORIGIN_BUTTON = wxNewId();
const long EditImageDialog::ID_STATICTEXT1 = wxNewId();
const long EditImageDialog::IDC_COLOR_MODE = wxNewId();
const long EditImageDialog::IDC_USE_KEY_COLOR = wxNewId();
const long EditImageDialog::IDC_SELECT_KEY_COLOR = wxNewId();
const long EditImageDialog::ID_STATICTEXT2 = wxNewId();
const long EditImageDialog::IDC_SLICE_MODE = wxNewId();
const long EditImageDialog::IDC_AUTO_SLICE_BUTTON = wxNewId();
const long EditImageDialog::ID_STATICTEXT3 = wxNewId();
const long EditImageDialog::IDC_SLICE_LEFT = wxNewId();
const long EditImageDialog::ID_STATICTEXT4 = wxNewId();
const long EditImageDialog::IDC_SLICE_RIGHT = wxNewId();
const long EditImageDialog::ID_STATICTEXT5 = wxNewId();
const long EditImageDialog::IDC_SLICE_TOP = wxNewId();
const long EditImageDialog::ID_STATICTEXT6 = wxNewId();
const long EditImageDialog::IDC_SLICE_BOTTOM = wxNewId();
const long EditImageDialog::ID_STATICLINE2 = wxNewId();
const long EditImageDialog::ID_STATICTEXT7 = wxNewId();
const long EditImageDialog::IDC_FILL_MODE = wxNewId();
const long EditImageDialog::ID_STATICTEXT8 = wxNewId();
const long EditImageDialog::IDC_FILL_WIDTH = wxNewId();
const long EditImageDialog::ID_STATICTEXT9 = wxNewId();
const long EditImageDialog::IDC_FILL_HEIGHT = wxNewId();
const long EditImageDialog::ID_STATICLINE1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(EditImageDialog,wxDialog)
	//(*EventTable(EditImageDialog)
	//*)

	EVT_SIZE(EditImageDialog::OnResize)

	EVT_SEARCH(ID_SEARCHCTRL1, EditImageDialog::OnSearch)
	EVT_SEARCH_CANCEL(ID_SEARCHCTRL1, EditImageDialog::OnSearchCancel)
	EVT_BUTTON(IDC_ADD_BUTTON, EditImageDialog::OnAdd)
	EVT_BUTTON(IDC_DELETE_BUTTON, EditImageDialog::OnDelete)
	EVT_LISTBOX(IDC_IMAGE_LIST, EditImageDialog::OnImageListSelectChanged)
	EVT_LISTBOX_DCLICK(IDC_IMAGE_LIST, EditImageDialog::OnImageItemRename)

	EVT_SLIDER(IDC_SCALE_SLIDER, EditImageDialog::OnScaleSliderValueChanged)
	EVT_SCROLL_CHANGED(EditImageDialog::OnScaleSliderValueChanged2)
	EVT_BUTTON(IDC_SCALE_DOWN_BUTTON, EditImageDialog::OnScaleDown)
	EVT_BUTTON(IDC_SCALE_UP_BUTTON, EditImageDialog::OnScaleUp)
	EVT_BUTTON(IDC_SCALE_ORIGIN_BUTTON, EditImageDialog::OnScaleOrigin)

	EVT_CHOICE(IDC_COLOR_MODE, EditImageDialog::OnColorModeSelectChanged)
	EVT_CHECKBOX(IDC_USE_KEY_COLOR, EditImageDialog::OnUseKeyColorChanged)
	EVT_BUTTON(IDC_SELECT_KEY_COLOR, EditImageDialog::OnSelectKeyColor)

	EVT_CHOICE(IDC_SLICE_MODE, EditImageDialog::OnSliceModeSelectionChanged)
	EVT_CHOICE(IDC_FILL_MODE, EditImageDialog::OnFillModeSelectionChanged)
	EVT_BUTTON(IDC_AUTO_SLICE_BUTTON, EditImageDialog::OnAutoSlice)

	EVT_SPINCTRL(IDC_SLICE_LEFT, EditImageDialog::OnSliceLeftValueChanged)
	EVT_SPINCTRL(IDC_SLICE_RIGHT, EditImageDialog::OnSliceRightValueChanged)
	EVT_SPINCTRL(IDC_SLICE_TOP, EditImageDialog::OnSliceTopValueChanged)
	EVT_SPINCTRL(IDC_SLICE_BOTTOM, EditImageDialog::OnSliceBottomValueChanged)
	EVT_SPINCTRL(IDC_FILL_WIDTH, EditImageDialog::OnFillWidthValueChanged)
	EVT_SPINCTRL(IDC_FILL_HEIGHT, EditImageDialog::OnFillHeightValueChanged)

END_EVENT_TABLE()


EditImageDialog::EditImageDialog(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(EditImageDialog)
	wxBoxSizer* BoxSizer10;
	wxBoxSizer* BoxSizer11;
	wxBoxSizer* BoxSizer12;
	wxBoxSizer* BoxSizer13;
	wxBoxSizer* BoxSizer14;
	wxBoxSizer* BoxSizer1;
	wxBoxSizer* BoxSizer2;
	wxBoxSizer* BoxSizer3;
	wxBoxSizer* BoxSizer4;
	wxBoxSizer* BoxSizer5;
	wxBoxSizer* BoxSizer6;
	wxBoxSizer* BoxSizer7;
	wxBoxSizer* BoxSizer8;
	wxBoxSizer* BoxSizer9;
	wxStaticBoxSizer* StaticBoxSizer1;
	wxStaticBoxSizer* StaticBoxSizer2;
	wxStaticBoxSizer* StaticBoxSizer3;
	wxStdDialogButtonSizer* StdDialogButtonSizer1;

	Create(parent, wxID_ANY, _T("编辑图片"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxMAXIMIZE_BOX|wxMINIMIZE_BOX, _T("wxID_ANY"));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	BoxSizer8 = new wxBoxSizer(wxVERTICAL);
	BoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
	m_SearchCtrl1 = new wxSearchCtrl(this, ID_SEARCHCTRL1, wxEmptyString, wxDefaultPosition, wxSize(212,27), wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_SEARCHCTRL1"));
	BoxSizer9->Add(m_SearchCtrl1, 1, wxRIGHT|wxEXPAND, 5);
	m_AddButton = new wxButton(this, IDC_ADD_BUTTON, _T("添加"), wxDefaultPosition, wxSize(50,27), 0, wxDefaultValidator, _T("IDC_ADD_BUTTON"));
	BoxSizer9->Add(m_AddButton, 0, wxEXPAND, 5);
	m_DeleteButton = new wxButton(this, IDC_DELETE_BUTTON, _T("删除"), wxDefaultPosition, wxSize(50,27), 0, wxDefaultValidator, _T("IDC_DELETE_BUTTON"));
	BoxSizer9->Add(m_DeleteButton, 0, wxEXPAND, 5);
	BoxSizer8->Add(BoxSizer9, 0, wxBOTTOM|wxEXPAND, 2);
	m_ImageList = new ImageListBox(this,IDC_IMAGE_LIST);
	BoxSizer8->Add(m_ImageList, 1, wxEXPAND, 5);
	BoxSizer2->Add(BoxSizer8, 0, wxALL|wxEXPAND, 5);
	BoxSizer3 = new wxBoxSizer(wxVERTICAL);
	m_PreviewImageContainer = new wxScrolledWindow(this, IDC_PREVIEW_IMAGE_CONTAINER, wxDefaultPosition, wxDefaultSize, wxVSCROLL|wxHSCROLL, _T("IDC_PREVIEW_IMAGE_CONTAINER"));
	m_PreviewImage = new wxStaticBitmap(m_PreviewImageContainer, IDC_PREVIEW_IMAGE, wxNullBitmap, wxPoint(0,0), wxSize(700,450), 0, _T("IDC_PREVIEW_IMAGE"));
	m_PreviewImage->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	BoxSizer3->Add(m_PreviewImageContainer, 1, wxALL|wxEXPAND, 5);
	BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
	BoxSizer6 = new wxBoxSizer(wxVERTICAL);
	StaticBoxSizer3 = new wxStaticBoxSizer(wxHORIZONTAL, this, _T("缩放"));
	m_ScaleDownButton = new wxButton(this, IDC_SCALE_DOWN_BUTTON, _T("-"), wxDefaultPosition, wxSize(24,24), 0, wxDefaultValidator, _T("IDC_SCALE_DOWN_BUTTON"));
	StaticBoxSizer3->Add(m_ScaleDownButton, 0, wxTOP|wxBOTTOM|wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_ScaleSlider = new wxSlider(this, IDC_SCALE_SLIDER, 100, 10, 800, wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS, wxDefaultValidator, _T("IDC_SCALE_SLIDER"));
	m_ScaleSlider->SetTickFreq(10);
	m_ScaleSlider->SetPageSize(50);
	StaticBoxSizer3->Add(m_ScaleSlider, 1, wxTOP|wxBOTTOM|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_ScaleLabel = new wxStaticText(this, IDC_SCALE_LABEL, _T("100%"), wxDefaultPosition, wxDefaultSize, 0, _T("IDC_SCALE_LABEL"));
	StaticBoxSizer3->Add(m_ScaleLabel, 0, wxTOP|wxBOTTOM|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_ScaleUpButton = new wxButton(this, IDC_SCALE_UP_BUTTON, _T("+"), wxDefaultPosition, wxSize(24,24), 0, wxDefaultValidator, _T("IDC_SCALE_UP_BUTTON"));
	StaticBoxSizer3->Add(m_ScaleUpButton, 0, wxTOP|wxBOTTOM|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_ScaleOriginButton = new wxButton(this, IDC_SCALE_ORIGIN_BUTTON, _T("100%"), wxDefaultPosition, wxSize(50,24), 0, wxDefaultValidator, _T("IDC_SCALE_ORIGIN_BUTTON"));
	StaticBoxSizer3->Add(m_ScaleOriginButton, 0, wxTOP|wxBOTTOM|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer6->Add(StaticBoxSizer3, 2, wxBOTTOM|wxEXPAND, 5);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, _T("颜色"));
	BoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _T("颜色格式:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	BoxSizer5->Add(StaticText1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_ColorModeChoice = new wxChoice(this, IDC_COLOR_MODE, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("IDC_COLOR_MODE"));
	m_ColorModeChoice->SetSelection( m_ColorModeChoice->Append(_T("与工程一致")) );
	m_ColorModeChoice->Append(_T("ARGB8888 (32位)"));
	m_ColorModeChoice->Append(_T("ARGB1555 (16位)"));
	BoxSizer5->Add(m_ColorModeChoice, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer1->Add(BoxSizer5, 1, wxTOP|wxEXPAND, 5);
	BoxSizer10 = new wxBoxSizer(wxHORIZONTAL);
	m_UseKeyColor = new wxCheckBox(this, IDC_USE_KEY_COLOR, _T("颜色过滤"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("IDC_USE_KEY_COLOR"));
	m_UseKeyColor->SetValue(false);
	BoxSizer10->Add(m_UseKeyColor, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_SelectKeyColorButton = new wxButton(this, IDC_SELECT_KEY_COLOR, _T("选择颜色"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("IDC_SELECT_KEY_COLOR"));
	BoxSizer10->Add(m_SelectKeyColorButton, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer1->Add(BoxSizer10, 1, wxBOTTOM|wxEXPAND, 5);
	BoxSizer6->Add(StaticBoxSizer1, 3, wxTOP|wxEXPAND, 5);
	BoxSizer4->Add(BoxSizer6, 1, wxEXPAND, 5);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxVERTICAL, this, _T("切片"));
	BoxSizer12 = new wxBoxSizer(wxHORIZONTAL);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _T("切片: "), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	BoxSizer12->Add(StaticText2, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_SliceModeChoice = new wxChoice(this, IDC_SLICE_MODE, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("IDC_SLICE_MODE"));
	m_SliceModeChoice->SetSelection( m_SliceModeChoice->Append(_T("无")) );
	m_SliceModeChoice->Append(_T("仅横向"));
	m_SliceModeChoice->Append(_T("仅纵向"));
	m_SliceModeChoice->Append(_T("横向和纵向"));
	BoxSizer12->Add(m_SliceModeChoice, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
	m_AutoSliceButton = new wxButton(this, IDC_AUTO_SLICE_BUTTON, _T("自动"), wxDefaultPosition, wxSize(80,-1), 0, wxDefaultValidator, _T("IDC_AUTO_SLICE_BUTTON"));
	BoxSizer12->Add(m_AutoSliceButton, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer2->Add(BoxSizer12, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 5);
	BoxSizer13 = new wxBoxSizer(wxHORIZONTAL);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _T("左: "), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	BoxSizer13->Add(StaticText3, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_SliceLeft = new wxSpinCtrl(this, IDC_SLICE_LEFT, _T("0"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 0, _T("IDC_SLICE_LEFT"));
	m_SliceLeft->SetValue(_T("0"));
	BoxSizer13->Add(m_SliceLeft, 1, wxALL|wxEXPAND, 5);
	StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _T("  右: "), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	BoxSizer13->Add(StaticText4, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_SliceRight = new wxSpinCtrl(this, IDC_SLICE_RIGHT, _T("0"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 0, _T("IDC_SLICE_RIGHT"));
	m_SliceRight->SetValue(_T("0"));
	BoxSizer13->Add(m_SliceRight, 1, wxALL|wxEXPAND, 5);
	StaticBoxSizer2->Add(BoxSizer13, 0, wxLEFT|wxRIGHT|wxEXPAND, 5);
	BoxSizer14 = new wxBoxSizer(wxHORIZONTAL);
	StaticText5 = new wxStaticText(this, ID_STATICTEXT5, _T("上: "), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
	BoxSizer14->Add(StaticText5, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_SliceTop = new wxSpinCtrl(this, IDC_SLICE_TOP, _T("0"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 0, _T("IDC_SLICE_TOP"));
	m_SliceTop->SetValue(_T("0"));
	BoxSizer14->Add(m_SliceTop, 1, wxALL|wxEXPAND, 5);
	StaticText6 = new wxStaticText(this, ID_STATICTEXT6, _T("  下: "), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6"));
	BoxSizer14->Add(StaticText6, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_SliceBottom = new wxSpinCtrl(this, IDC_SLICE_BOTTOM, _T("0"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 0, _T("IDC_SLICE_BOTTOM"));
	m_SliceBottom->SetValue(_T("0"));
	BoxSizer14->Add(m_SliceBottom, 1, wxALL|wxEXPAND, 5);
	StaticBoxSizer2->Add(BoxSizer14, 0, wxLEFT|wxRIGHT|wxEXPAND, 5);
	StaticLine2 = new wxStaticLine(this, ID_STATICLINE2, wxDefaultPosition, wxSize(10,-1), wxLI_HORIZONTAL, _T("ID_STATICLINE2"));
	StaticBoxSizer2->Add(StaticLine2, 0, wxALL|wxEXPAND, 5);
	BoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
	StaticText7 = new wxStaticText(this, ID_STATICTEXT7, _T("重复: "), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT7"));
	BoxSizer7->Add(StaticText7, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_FillModeChoice = new wxChoice(this, IDC_FILL_MODE, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("IDC_FILL_MODE"));
	m_FillModeChoice->SetSelection( m_FillModeChoice->Append(_T("平铺")) );
	m_FillModeChoice->Append(_T("拉伸"));
	BoxSizer7->Add(m_FillModeChoice, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer2->Add(BoxSizer7, 0, wxLEFT|wxRIGHT|wxEXPAND, 5);
	BoxSizer11 = new wxBoxSizer(wxHORIZONTAL);
	StaticText8 = new wxStaticText(this, ID_STATICTEXT8, _T("宽: "), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT8"));
	BoxSizer11->Add(StaticText8, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_FillWidth = new wxSpinCtrl(this, IDC_FILL_WIDTH, _T("0"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 0, _T("IDC_FILL_WIDTH"));
	m_FillWidth->SetValue(_T("0"));
	BoxSizer11->Add(m_FillWidth, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText9 = new wxStaticText(this, ID_STATICTEXT9, _T("  高: "), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT9"));
	BoxSizer11->Add(StaticText9, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_FillHeight = new wxSpinCtrl(this, IDC_FILL_HEIGHT, _T("0"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100, 0, _T("IDC_FILL_HEIGHT"));
	m_FillHeight->SetValue(_T("0"));
	BoxSizer11->Add(m_FillHeight, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer2->Add(BoxSizer11, 0, wxLEFT|wxRIGHT|wxEXPAND, 5);
	BoxSizer4->Add(StaticBoxSizer2, 1, wxLEFT|wxEXPAND, 5);
	BoxSizer3->Add(BoxSizer4, 0, wxALL|wxEXPAND, 5);
	BoxSizer2->Add(BoxSizer3, 1, wxTOP|wxEXPAND, 2);
	BoxSizer1->Add(BoxSizer2, 1, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 5);
	StaticLine1 = new wxStaticLine(this, ID_STATICLINE1, wxDefaultPosition, wxSize(10,-1), wxLI_HORIZONTAL, _T("ID_STATICLINE1"));
	BoxSizer1->Add(StaticLine1, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND, 5);
	StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_OK, _T("确定")));
	StdDialogButtonSizer1->Realize();
	BoxSizer1->Add(StdDialogButtonSizer1, 0, wxALL|wxALIGN_RIGHT, 5);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	Center();
	//*)

	m_ImageList->SetImages(&(Manager::Get()->GetActiveProject()->m_Images));
	m_SearchImageList = new ImageList();
	m_SearchCtrl1->Connect(wxEVT_COMMAND_TEXT_UPDATED, (wxObjectEventFunction)&EditImageDialog::OnSearch, nullptr, this);
	m_SearchCtrl1->Connect(wxEVT_CHAR, (wxObjectEventFunction)&EditImageDialog::OnSearchKeySelected, nullptr, this);

	if (m_ImageList->GetItemCount() > 0)
	{
		m_ImageList->SetSelection(0);
		UpdateCurrentImage(0);
	}
	else
	{
		ResizePreviewImage();
	}
}

EditImageDialog::~EditImageDialog()
{
	//(*Destroy(EditImageDialog)
	//*)
}


void EditImageDialog::OnResize(wxSizeEvent& event)
{
	this->Layout();
	ResizePreviewImage();
}


void EditImageDialog::OnSearchKeySelected(wxKeyEvent& event)
{
	unsigned int currentSelect;
	wxString currentName;
	if (m_ImageList->GetImages() == &(Manager::Get()->GetActiveProject()->m_Images))
	{
		event.Skip();
		return;
	}
	currentSelect = m_ImageList->GetSelection();

	if (event.GetKeyCode() == WXK_DOWN)
	{
		currentSelect++;
		if (currentSelect >= m_ImageList->GetItemCount())
		{
			currentSelect = 0;
		}
		m_ImageList->SetSelection(currentSelect);
		ShowImage(m_ImageList->GetImage(currentSelect));
	}
	else if (event.GetKeyCode() == WXK_UP)
	{
		currentSelect--;
		if (currentSelect < 0)
		{
			currentSelect = m_ImageList->GetItemCount() - 1;
		}
		m_ImageList->SetSelection(currentSelect);
		ShowImage(m_ImageList->GetImage(currentSelect));
	}
	else if (event.GetKeyCode() == WXK_RETURN)
	{
		currentName = m_ImageList->GetImage(currentSelect)->GetName();
		m_ImageList->SetImages(&(Manager::Get()->GetActiveProject()->m_Images));
		m_ImageList->SetSelection(m_ImageList->GetImages()->FindImage(currentName));
	    m_ImageList->RefreshSelected();
	    m_ImageList->RefreshAll();
	    m_SearchCtrl1->Clear();
	    m_ImageList->SetFocus();
	}
	else
	{
		event.Skip();
	}
}

void EditImageDialog::OnSearchCancel(wxCommandEvent& event)
{
	m_SearchCtrl1->ShowCancelButton(false);
    if (m_ImageList->GetImages() != &(Manager::Get()->GetActiveProject()->m_Images))
	{
    	m_ImageList->SetImages(&(Manager::Get()->GetActiveProject()->m_Images));
	}
}

void EditImageDialog::OnSearch(wxCommandEvent& event)
{
    wxString text = m_SearchCtrl1->GetValue();
    wxString propety;
    wxString tempStr;
    wxArrayString subStr;
    char *p;
    char *buf;
    bool hasFound = false;
	m_ImageList->SetImages(&(Manager::Get()->GetActiveProject()->m_Images));
	if (text.empty())
	{
		m_SearchCtrl1->ShowCancelButton(false);
	    m_ImageList->RefreshSelected();
	    m_ImageList->RefreshAll();
		return;
	}
	m_SearchCtrl1->ShowCancelButton(true);
	memset(m_SearchImageList, 0, sizeof(ImageList));

	for (size_t i = 0; i < m_ImageList->GetItemCount(); i++)
	{
		propety += m_ImageList->GetImage(i)->GetName();
	}
	propety.LowerCase();
	text.LowerCase();
	buf = (char *)text.c_str().AsChar();
	p = strtok(buf, " ");
	while(p != NULL)
	{
		subStr.Add(p);
	    p = strtok(NULL, " ");
	}
	wxArrayString::iterator it;
	for (size_t i = 0; i < m_ImageList->GetItemCount(); i++)
	{
		tempStr = m_ImageList->GetImage(i)->GetName();
		tempStr.LowerCase();
		for (it = subStr.begin(); it != subStr.end(); it++)
		{
    		if (tempStr.find(*it) == wxString::npos)
    		{
    			break;
    		}
		}
		if (it == subStr.end())
		{
			hasFound = true;
			if (m_SearchImageList->GetImage(m_ImageList->GetImage(i)->GetName()) == nullptr)
			{
    			m_SearchImageList->AddImage(m_ImageList->GetImage(i));
			}
		}
	}

	m_ImageList->SetImages(m_SearchImageList);
	if (hasFound)
	{
		m_ImageList->SetSelection(0);
		ShowImage(m_ImageList->GetImage(0));
	}

    m_ImageList->RefreshSelected();
    m_ImageList->RefreshAll();
}

void EditImageDialog::OnAdd(wxCommandEvent& event)
{
	wxFileDialog fileDialog(this, wxT("选择图片"), wxEmptyString, wxEmptyString,
			wxT("所有图片文件|*.bmp;*.jpg;*.jpeg;*.tif;*.tiff;*.png|位图文件(*.bmp)|*.bmp|JPEG(*.jpg;*.jpeg)|*.jpg;*.jpeg|PNG(*.png)|*.png|TIF(*.tif;*.tiff)|*.tif;*.tiff|所有文件|*.*"),
			wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_MULTIPLE);
	fileDialog.ShowModal();

    wxArrayString paths;
    wxArrayString filenames;
    fileDialog.GetPaths(paths);
    fileDialog.GetFilenames(filenames);

	Image *item;
	wxString addStr;
	int action;
	m_SearchCtrl1->Clear();
	m_ImageList->SetImages(&(Manager::Get()->GetActiveProject()->m_Images));
	wxMessageDialog askSave(this, addStr, wxT("添加图片"), wxYES_NO|wxNO_DEFAULT|wxICON_QUESTION);
	askSave.SetYesNoLabels(wxT("是"), wxT("否"));
    for (size_t i = 0; i < paths.GetCount(); i++)
    {
    	item = m_ImageList->GetImage(filenames.Item(i));
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
    			m_ImageList->DeleteImage(item);
			}
    	}
    	m_ImageList->AddImage(new Image(paths.Item(i), filenames.Item(i)));
    }
    m_ImageList->RefreshAll();
	Manager::Get()->GetActiveProject()->m_Images.ApplyToGUISystem();
	Manager::Get()->GetActiveProject()->MarkAsModified();
}


void EditImageDialog::OnDelete(wxCommandEvent& event)
{
	Image *selectDelete = nullptr;
	wxString name;
	ImageList *reall = &(Manager::Get()->GetActiveProject()->m_Images);
	selectDelete = m_ImageList->GetImage(m_ImageList->GetSelection());
	if (selectDelete)
	{
		name = selectDelete->GetName();
		m_ImageList->DeleteImage(selectDelete);
	    m_ImageList->RefreshAll();
	    if (m_ImageList->GetImages() != &(Manager::Get()->GetActiveProject()->m_Images))
	    {
	    	reall->DeleteImage(reall->GetImage(name));
	    }
		Manager::Get()->GetActiveProject()->m_Images.ApplyToGUISystem();
		Manager::Get()->GetActiveProject()->MarkAsModified();
	}
}


void EditImageDialog::OnImageListSelectChanged(wxCommandEvent& event)
{
	UpdateCurrentImage(event.GetSelection());
}

void EditImageDialog::OnImageItemRename(wxCommandEvent& event)
{
	Image *select = m_ImageList->GetImage(m_ImageList->GetSelection());
    wxTextEntryDialog dlg(this, wxT("请输入新名称:"), wxT("重命名图片"), select->GetName(), wxOK | wxCANCEL | wxCENTRE);
    wxString str;
	if (dlg.ShowModal() == wxID_OK)
	{
		if (nullptr == Manager::Get()->GetActiveProject()->m_Images.GetImage(dlg.GetValue()))
		{
			select->SetName( dlg.GetValue());
			Manager::Get()->GetActiveProject()->m_Images.ApplyToGUISystem();
			Manager::Get()->GetActiveProject()->MarkAsModified();
		}
		else
		{
			wxMessageDialog askSave(this, str, wxT("图片重命名"), wxOK);
			askSave.SetOKLabel("OK");
			str += dlg.GetValue();
			str += wxT(" 名称已存在！");
    		askSave.SetMessage(str);
    		askSave.ShowModal();
		}
	}
}


void EditImageDialog::OnScaleSliderValueChanged(wxCommandEvent& event)
{
	m_ScaleLabel->SetLabel(wxString::Format(wxT("%d%%"), m_ScaleSlider->GetValue()));
}

void EditImageDialog::OnScaleSliderValueChanged2(wxScrollEvent& event)
{
	ResizePreviewImage();
	ShowImage(m_CurrentImage);
}

void EditImageDialog::OnScaleDown(wxCommandEvent& event)
{
	if (m_ScaleSlider->GetValue() >= m_ScaleSlider->GetMin() + 10)
	{
		m_ScaleSlider->SetValue(m_ScaleSlider->GetValue() - 10);

		m_ScaleLabel->SetLabel(wxString::Format(wxT("%d%%"), m_ScaleSlider->GetValue()));
		ResizePreviewImage();
		ShowImage(m_CurrentImage);
	}
}

void EditImageDialog::OnScaleUp(wxCommandEvent& event)
{
	if (m_ScaleSlider->GetValue() <= m_ScaleSlider->GetMax() - 10)
	{
		m_ScaleSlider->SetValue(m_ScaleSlider->GetValue() + 10);

		m_ScaleLabel->SetLabel(wxString::Format(wxT("%d%%"), m_ScaleSlider->GetValue()));
		ResizePreviewImage();
		ShowImage(m_CurrentImage);
	}
}

void EditImageDialog::OnScaleOrigin(wxCommandEvent& event)
{
	if (m_ScaleSlider->GetValue() != 100)
	{
		m_ScaleSlider->SetValue(100);

		m_ScaleLabel->SetLabel(wxString::Format(wxT("%d%%"), m_ScaleSlider->GetValue()));
		ResizePreviewImage();
		ShowImage(m_CurrentImage);
	}
}


void EditImageDialog::OnColorModeSelectChanged(wxCommandEvent& event)
{
	if (m_CurrentImage == nullptr)
		return;

	switch (event.GetSelection())
	{
		case 1:
			m_CurrentImage->SetColorMode(ColorMode_ARGB8888);
			break;
		case 2:
			m_CurrentImage->SetColorMode(ColorMode_ARGB1555);
			break;
		default:
			m_CurrentImage->SetColorMode(ColorMode_Auto);
			break;
	}
	ShowImage(m_CurrentImage);
}

void EditImageDialog::OnUseKeyColorChanged(wxCommandEvent& event)
{
	if (m_CurrentImage == nullptr)
		return;

	m_SelectKeyColorButton->Enable(event.GetSelection());
	m_CurrentImage->SetUseKeyColor(event.GetSelection());
	ShowImage(m_CurrentImage);
}

void EditImageDialog::OnSelectKeyColor(wxCommandEvent& event)
{
	if (m_CurrentImage == nullptr)
		return;

	wxColourData colorData;
	colorData.SetColour(UIntToColor(m_CurrentImage->GetKeyColor()));

	wxColourDialog dlg(this, &colorData);
    if (dlg.ShowModal() == wxID_OK)
    {
        m_CurrentImage->SetKeyColor(ColorToUInt(dlg.GetColourData().GetColour()));
        ShowImage(m_CurrentImage);
    }
}


void EditImageDialog::OnSliceModeSelectionChanged(wxCommandEvent& event)
{
	if (m_CurrentImage == nullptr)
		return;

	UpdateSliceMode();
	m_CurrentImage->SetSliceMode(event.GetSelection());

	ShowImage(m_CurrentImage);
}

void EditImageDialog::OnFillModeSelectionChanged(wxCommandEvent& event)
{
	if (m_CurrentImage == nullptr)
		return;

	m_CurrentImage->SetSliceFillMode(event.GetSelection());
}

void EditImageDialog::OnAutoSlice(wxCommandEvent& event)
{
	if (m_CurrentImage == nullptr)
		return;

	m_CurrentImage->CalcSlicePosition();
	UpdateSlicePositionValue();

	ShowImage(m_CurrentImage);
}

void EditImageDialog::OnSliceLeftValueChanged(wxSpinEvent& event)
{
	if (m_CurrentImage == nullptr)
		return;

	wxLogMessage(wxString::Format("Slice Left Changed: %d", event.GetValue()));
	m_CurrentImage->SetSliceLeft(event.GetValue());

	m_SliceRight->SetRange(m_CurrentImage->GetSliceLeft(), m_CurrentImage->GetWidth() - 1);
	m_CurrentImage->SetSliceRight(m_SliceRight->GetValue());

	ShowImage(m_CurrentImage);
}

void EditImageDialog::OnSliceRightValueChanged(wxSpinEvent& event)
{
	if (m_CurrentImage == nullptr)
		return;

	wxLogMessage(wxString::Format("Slice Right Changed: %d", event.GetValue()));
	m_CurrentImage->SetSliceRight(event.GetValue());

	ShowImage(m_CurrentImage);
}

void EditImageDialog::OnSliceTopValueChanged(wxSpinEvent& event)
{
	if (m_CurrentImage == nullptr)
		return;

	wxLogMessage(wxString::Format("Slice Top Changed: %d", event.GetValue()));
	m_CurrentImage->SetSliceTop(event.GetValue());

	m_SliceBottom->SetRange(m_CurrentImage->GetSliceTop(), m_CurrentImage->GetHeight() - 1);
	m_CurrentImage->SetSliceBottom(m_SliceBottom->GetValue());

	ShowImage(m_CurrentImage);
}

void EditImageDialog::OnSliceBottomValueChanged(wxSpinEvent& event)
{
	if (m_CurrentImage == nullptr)
		return;

	wxLogMessage(wxString::Format("Slice Bottom Changed: %d", event.GetValue()));
	m_CurrentImage->SetSliceBottom(event.GetValue());

	ShowImage(m_CurrentImage);
}

void EditImageDialog::OnFillWidthValueChanged(wxSpinEvent& event)
{
	if (m_CurrentImage == nullptr)
		return;

	wxLogMessage(wxString::Format("Slice Fill Width Changed: %d", event.GetValue()));
	m_CurrentImage->SetSliceFillWidth(event.GetValue());

	ShowImage(m_CurrentImage);
}

void EditImageDialog::OnFillHeightValueChanged(wxSpinEvent& event)
{
	if (m_CurrentImage == nullptr)
		return;

	wxLogMessage(wxString::Format("Slice Fill Height Changed: %d", event.GetValue()));
	m_CurrentImage->SetSliceFillHeight(event.GetValue());

	ShowImage(m_CurrentImage);
}


void EditImageDialog::UpdateCurrentImage(int selection)
{
	m_CurrentImage = m_ImageList->GetImage(selection);

	ResizePreviewImage();
//	m_UseKeyColor->SetValue(m_CurrentImage->HaveKeyColor());
//	m_SelectKeyColorButton->Enable(m_CurrentImage->HaveKeyColor());
	switch (m_CurrentImage->GetColorMode())
	{
		case ColorMode_ARGB8888:
			m_ColorModeChoice->SetSelection(1);
			break;
		case ColorMode_ARGB1555:
			m_ColorModeChoice->SetSelection(2);
			break;
		default:
			m_ColorModeChoice->SetSelection(0);
			break;
	}

	m_SliceModeChoice->SetSelection(m_CurrentImage->GetSliceMode());
	m_FillModeChoice->SetSelection(m_CurrentImage->GetSliceFillMode());

	m_FillWidth->SetRange(1, m_CurrentImage->GetWidth() - 1);
	m_FillWidth->SetValue(m_CurrentImage->GetSliceFillWidth());

	m_FillHeight->SetRange(1, m_CurrentImage->GetHeight() - 1);
	m_FillHeight->SetValue(m_CurrentImage->GetSliceFillHeight());

	UpdateSlicePositionValue();
	UpdateSliceMode();

	ShowImage(m_CurrentImage);
}

void EditImageDialog::ResizePreviewImage(void)
{
	int imageWidth = 1, imageHeight = 1;

	if (m_PreviewImage == nullptr || m_PreviewImageContainer == nullptr)
		return;

	if (m_CurrentImage)
	{
		int scalePercent = m_ScaleSlider ? m_ScaleSlider->GetValue() : 100;

		imageWidth = wxMax(imageWidth, m_CurrentImage->GetWidth() * scalePercent / 100);
		imageHeight = wxMax(imageHeight, m_CurrentImage->GetHeight() * scalePercent / 100);
	}
	m_PreviewImage->SetSize(wxMax(m_PreviewImageContainer->GetSize().GetX(), imageWidth), wxMax(m_PreviewImageContainer->GetSize().GetY(), imageHeight));
	m_PreviewImageContainer->SetScrollbars(10, 10, m_PreviewImage->GetSize().x / 10, m_PreviewImage->GetSize().y / 10);
}

void EditImageDialog::UpdateSliceMode(void)
{
	int silceMode = m_SliceModeChoice->GetSelection();

	m_AutoSliceButton->Enable(silceMode);

	m_SliceLeft->Enable(silceMode & SliceMode_Horizontal);
	m_SliceRight->Enable(silceMode & SliceMode_Horizontal);

	m_SliceTop->Enable(silceMode & SliceMode_Vertical);
	m_SliceBottom->Enable(silceMode & SliceMode_Vertical);

	m_FillModeChoice->Enable(silceMode);
	m_FillWidth->Enable(silceMode & SliceMode_Horizontal);
	m_FillHeight->Enable(silceMode & SliceMode_Vertical);
}

void EditImageDialog::UpdateSlicePositionValue(void)
{
	if (m_CurrentImage == nullptr)
		return;

	m_SliceLeft->SetRange(0, m_CurrentImage->GetWidth() - 1);
	m_SliceLeft->SetValue(m_CurrentImage->GetSliceLeft());

	m_SliceRight->SetRange(m_CurrentImage->GetSliceLeft(), m_CurrentImage->GetWidth() - 1);
	m_SliceRight->SetValue(m_CurrentImage->GetSliceRight());

	m_SliceTop->SetRange(0, m_CurrentImage->GetHeight() - 1);
	m_SliceTop->SetValue(m_CurrentImage->GetSliceTop());

	m_SliceBottom->SetRange(m_CurrentImage->GetSliceTop(), m_CurrentImage->GetHeight() - 1);
	m_SliceBottom->SetValue(m_CurrentImage->GetSliceBottom());
}

void EditImageDialog::ShowImage(Image* image)
{
	if (!image || !image->IsOK())
		return;

	wxSize previewSize = m_PreviewImage->GetSize();
	unsigned int *pixels = new unsigned int[previewSize.GetWidth() * previewSize.GetHeight()];
	image->GetPreviewPixels(pixels, previewSize, m_PreviewImage->GetBackgroundColour(), m_ScaleSlider->GetValue());
	m_PreviewImage->SetBitmap(wxBitmap((const char *)pixels, previewSize.GetWidth(), previewSize.GetHeight(), 32));
	delete [] pixels;
}

void EditImageDialog::Onm_SearchComboBoxSelected(wxCommandEvent& event)
{

}
