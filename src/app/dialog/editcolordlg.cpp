#include "app_global.h"
#include "editcolordlg.h"

//(*InternalHeaders(EditColorDialog)
#include "colorlistbox.h"
#include <wx/button.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/srchctrl.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textctrl.h>
//*)

//(*IdInit(EditColorDialog)
const long EditColorDialog::ID_SEARCHCTRL1 = wxNewId();
const long EditColorDialog::IDC_COLOR_NEW = wxNewId();
const long EditColorDialog::IDC_COLOR_DEL = wxNewId();
const long EditColorDialog::IDC_COLOR_LIST = wxNewId();
const long EditColorDialog::IDC_SHOW_COLOR_STATIC = wxNewId();
const long EditColorDialog::IDC_COLOR_VALUE_HEX = wxNewId();
const long EditColorDialog::ID_BUTTON1 = wxNewId();
const long EditColorDialog::ID_STATICLINE2 = wxNewId();
const long EditColorDialog::ID_STATICTEXT2 = wxNewId();
const long EditColorDialog::IDC_COLOR_NAME = wxNewId();
const long EditColorDialog::ID_STATICTEXT3 = wxNewId();
const long EditColorDialog::IDC_COLOR_SLIDER_R = wxNewId();
const long EditColorDialog::IDC_COLOR_R = wxNewId();
const long EditColorDialog::ID_STATICTEXT4 = wxNewId();
const long EditColorDialog::IDC_COLOR_SLIDER_G = wxNewId();
const long EditColorDialog::IDC_COLOR_G = wxNewId();
const long EditColorDialog::ID_STATICTEXT5 = wxNewId();
const long EditColorDialog::IDC_COLOR_SLIDER_B = wxNewId();
const long EditColorDialog::IDC_COLOR_B = wxNewId();
const long EditColorDialog::ID_STATICTEXT6 = wxNewId();
const long EditColorDialog::IDC_COLOR_SLIDER_A = wxNewId();
const long EditColorDialog::IDC_COLOR_A = wxNewId();
const long EditColorDialog::ID_STATICLINE1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(EditColorDialog,wxDialog)
	//(*EventTable(EditColorDialog)
	//*)

    EVT_LISTBOX(IDC_COLOR_LIST, EditColorDialog::OnColorListSelected)

    EVT_COMMAND_SCROLL(IDC_COLOR_SLIDER_A, EditColorDialog::OnSliderValueChanged)
    EVT_COMMAND_SCROLL(IDC_COLOR_SLIDER_R, EditColorDialog::OnSliderValueChanged)
    EVT_COMMAND_SCROLL(IDC_COLOR_SLIDER_G, EditColorDialog::OnSliderValueChanged)
    EVT_COMMAND_SCROLL(IDC_COLOR_SLIDER_B, EditColorDialog::OnSliderValueChanged)

    EVT_TEXT_ENTER(IDC_COLOR_NAME, EditColorDialog::OnColorNameChangeDone)
    EVT_BUTTON(IDC_COLOR_NEW, EditColorDialog::OnNewColor)
    EVT_BUTTON(IDC_COLOR_DEL, EditColorDialog::OnDeleteColor)
	EVT_SEARCH_CANCEL(ID_SEARCHCTRL1, EditColorDialog::OnSearchCancel)
	EVT_SEARCH(ID_SEARCHCTRL1, EditColorDialog::OnSearch)
    EVT_BUTTON(wxID_OK, EditColorDialog::OnOK)
    EVT_BUTTON(wxID_APPLY, EditColorDialog::OnApply)

END_EVENT_TABLE()


EditColorDialog::EditColorDialog(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(EditColorDialog)
	wxBoxSizer* BoxSizer10;
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
	wxStdDialogButtonSizer* StdDialogButtonSizer1;

	Create(parent, wxID_ANY, _T("编辑颜色"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxMAXIMIZE_BOX|wxMINIMIZE_BOX, _T("wxID_ANY"));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	BoxSizer3 = new wxBoxSizer(wxVERTICAL);
	BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
	m_SearchCtrl1 = new wxSearchCtrl(this, ID_SEARCHCTRL1, wxEmptyString, wxDefaultPosition, wxSize(201,27), wxTE_PROCESS_ENTER, wxDefaultValidator, _T("ID_SEARCHCTRL1"));
	BoxSizer4->Add(m_SearchCtrl1, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_NewColorButton = new wxButton(this, IDC_COLOR_NEW, _T("新建"), wxDefaultPosition, wxSize(50,27), 0, wxDefaultValidator, _T("IDC_COLOR_NEW"));
	BoxSizer4->Add(m_NewColorButton, 0, wxEXPAND, 5);
	m_DeleteColorButton = new wxButton(this, IDC_COLOR_DEL, _T("删除"), wxDefaultPosition, wxSize(50,27), 0, wxDefaultValidator, _T("IDC_COLOR_DEL"));
	BoxSizer4->Add(m_DeleteColorButton, 0, wxEXPAND, 5);
	BoxSizer3->Add(BoxSizer4, 0, wxBOTTOM|wxEXPAND, 2);
	m_ColorListBox = new ColorListBox(this, IDC_COLOR_LIST);
	BoxSizer3->Add(m_ColorListBox, 1, wxEXPAND, 5);
	BoxSizer2->Add(BoxSizer3, 1, wxALL|wxEXPAND, 5);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, _T("颜色"));
	m_ShowColorStatic = new wxStaticText(this, IDC_SHOW_COLOR_STATIC, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDOUBLE_BORDER, _T("IDC_SHOW_COLOR_STATIC"));
	m_ShowColorStatic->SetMinSize(wxSize(250,150));
	m_ShowColorStatic->SetMaxSize(wxSize(250,150));
	m_ShowColorStatic->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	StaticBoxSizer1->Add(m_ShowColorStatic, 1, wxALL, 5);
	BoxSizer10 = new wxBoxSizer(wxHORIZONTAL);
	m_ColorHexValueStatic = new wxStaticText(this, IDC_COLOR_VALUE_HEX, _T("0xFFFFFFFF"), wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE|wxALIGN_CENTRE, _T("IDC_COLOR_VALUE_HEX"));
	BoxSizer10->Add(m_ColorHexValueStatic, 1, wxALL|wxEXPAND, 5);
	m_ViewBackgroundColor = new wxButton(this, ID_BUTTON1, wxEmptyString, wxDefaultPosition, wxSize(30,30), wxSIMPLE_BORDER|wxTAB_TRAVERSAL, wxDefaultValidator, _T("ID_BUTTON1"));
	m_ViewBackgroundColor->SetMaxSize(wxSize(30,0));
	m_ViewBackgroundColor->SetBackgroundColour(wxColour(255,255,255));
	BoxSizer10->Add(m_ViewBackgroundColor, 0, wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer1->Add(BoxSizer10, 0, wxEXPAND, 5);
	StaticLine2 = new wxStaticLine(this, ID_STATICLINE2, wxDefaultPosition, wxSize(10,-1), wxLI_HORIZONTAL, _T("ID_STATICLINE2"));
	StaticBoxSizer1->Add(StaticLine2, 0, wxALL|wxEXPAND, 5);
	BoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _T("名称:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	BoxSizer5->Add(StaticText2, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_ColorNameText = new wxTextCtrl(this, IDC_COLOR_NAME, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER, wxDefaultValidator, _T("IDC_COLOR_NAME"));
	m_ColorNameText->SetMaxLength(64);
	BoxSizer5->Add(m_ColorNameText, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer1->Add(BoxSizer5, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);
	BoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _T("R:"), wxDefaultPosition, wxSize(14,-1), 0, _T("ID_STATICTEXT3"));
	BoxSizer6->Add(StaticText3, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_ColorRSlider = new wxSlider(this, IDC_COLOR_SLIDER_R, 0, 0, 255, wxDefaultPosition, wxSize(150,-1), 0, wxDefaultValidator, _T("IDC_COLOR_SLIDER_R"));
	m_ColorRSlider->SetLineSize(1);
	m_ColorRSlider->SetThumbLength(1);
	m_ColorRSlider->SetTick(1);
	BoxSizer6->Add(m_ColorRSlider, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_ColorRText = new wxTextCtrl(this, IDC_COLOR_R, wxEmptyString, wxDefaultPosition, wxSize(45,-1), 0, wxDefaultValidator, _T("IDC_COLOR_R"));
	BoxSizer6->Add(m_ColorRText, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer1->Add(BoxSizer6, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);
	BoxSizer7 = new wxBoxSizer(wxHORIZONTAL);
	StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _T("G:"), wxDefaultPosition, wxSize(14,-1), 0, _T("ID_STATICTEXT4"));
	BoxSizer7->Add(StaticText4, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_ColorGSlider = new wxSlider(this, IDC_COLOR_SLIDER_G, 0, 0, 255, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("IDC_COLOR_SLIDER_G"));
	BoxSizer7->Add(m_ColorGSlider, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_ColorGText = new wxTextCtrl(this, IDC_COLOR_G, wxEmptyString, wxDefaultPosition, wxSize(45,-1), 0, wxDefaultValidator, _T("IDC_COLOR_G"));
	BoxSizer7->Add(m_ColorGText, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer1->Add(BoxSizer7, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);
	BoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
	StaticText5 = new wxStaticText(this, ID_STATICTEXT5, _T("B:"), wxDefaultPosition, wxSize(14,-1), 0, _T("ID_STATICTEXT5"));
	BoxSizer8->Add(StaticText5, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_ColorBSlider = new wxSlider(this, IDC_COLOR_SLIDER_B, 0, 0, 255, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("IDC_COLOR_SLIDER_B"));
	BoxSizer8->Add(m_ColorBSlider, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_ColorBText = new wxTextCtrl(this, IDC_COLOR_B, wxEmptyString, wxDefaultPosition, wxSize(45,-1), 0, wxDefaultValidator, _T("IDC_COLOR_B"));
	BoxSizer8->Add(m_ColorBText, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer1->Add(BoxSizer8, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);
	BoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
	StaticText6 = new wxStaticText(this, ID_STATICTEXT6, _T("A:"), wxDefaultPosition, wxSize(14,-1), 0, _T("ID_STATICTEXT6"));
	BoxSizer9->Add(StaticText6, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_ColorASlider = new wxSlider(this, IDC_COLOR_SLIDER_A, 0, 0, 255, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("IDC_COLOR_SLIDER_A"));
	BoxSizer9->Add(m_ColorASlider, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_ColorAText = new wxTextCtrl(this, IDC_COLOR_A, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("IDC_COLOR_A"));
	m_ColorAText->SetMinSize(wxSize(45,-1));
	BoxSizer9->Add(m_ColorAText, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer1->Add(BoxSizer9, 0, wxTOP|wxBOTTOM|wxEXPAND, 5);
	BoxSizer2->Add(StaticBoxSizer1, 0, wxALL|wxEXPAND, 5);
	BoxSizer1->Add(BoxSizer2, 1, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 5);
	StaticLine1 = new wxStaticLine(this, ID_STATICLINE1, wxDefaultPosition, wxSize(10,-1), wxLI_HORIZONTAL, _T("ID_STATICLINE1"));
	BoxSizer1->Add(StaticLine1, 0, wxALL|wxEXPAND, 5);
	StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_OK, _T("确定")));
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_CANCEL, _T("取消")));
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_APPLY, _T("应用")));
	StdDialogButtonSizer1->Realize();
	BoxSizer1->Add(StdDialogButtonSizer1, 0, wxALL|wxALIGN_RIGHT, 5);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	Center();
	//*)

	m_ColorListBox->SetColors(Manager::Get()->GetActiveProject()->GetColorList());
	m_SrcColorList = ColorList(m_ColorListBox->GetColors());
	m_ColorListBox->SelectColor(0);
	ShowColorInforAtIndex(0);
	m_SearchCtrl1->Connect(wxEVT_COMMAND_TEXT_UPDATED, (wxObjectEventFunction)&EditColorDialog::OnSearch, nullptr, this);
	m_SearchCtrl1->Connect(wxEVT_CHAR, (wxObjectEventFunction)&EditColorDialog::OnSearchKeySelected, nullptr, this);
}

EditColorDialog::~EditColorDialog()
{
	//(*Destroy(EditColorDialog)
	//*)
}


void EditColorDialog::OnColorListSelected(wxCommandEvent& event)
{
	ShowColorInforAtIndex(event.GetSelection());
}

void EditColorDialog::OnSliderValueChanged(wxScrollEvent& event)
{
    unsigned char a, r, g, b;

    a = m_ColorASlider->GetValue();
    r = m_ColorRSlider->GetValue();
    g = m_ColorGSlider->GetValue();
    b = m_ColorBSlider->GetValue();

    m_ColorAText->SetLabel(wxString::Format("%d", a));
    m_ColorRText->SetLabel(wxString::Format("%d", r));
    m_ColorGText->SetLabel(wxString::Format("%d", g));
    m_ColorBText->SetLabel(wxString::Format("%d", b));

    m_ColorHexValueStatic->SetLabel(wxString::Format("0x%02X%02X%02X%02X", a, r, g, b));
    m_ShowColorStatic->SetBackgroundColour(wxColour(r, g, b, a));
    m_ShowColorStatic->Refresh();

    UpdateCurrentSelectColorValue();
}

void EditColorDialog::OnColorNameChangeDone(wxCommandEvent& event)
{
	UpdateCurrentSelectColorName();
}

void EditColorDialog::OnNewColor(wxCommandEvent& event)
{
	m_ColorListBox->SetColors(m_SrcColorList);
	m_ColorListBox->AddColor();
	m_SrcColorList.AddColor();
	m_ColorListBox->RefreshSelected();
	ShowColorInforAtIndex(m_ColorListBox->GetSelection());
	m_SearchCtrl1->Clear();
}

void EditColorDialog::OnDeleteColor(wxCommandEvent& event)
{
	int selectDelete = m_ColorListBox->GetSelection();
	wxString DeleteColorName = m_ColorListBox->GetColor(selectDelete).GetName();

	m_ColorListBox->DeleteColor(selectDelete);
	m_ColorListBox->RefreshAll();
	ShowColorInforAtIndex(selectDelete);
	m_SrcColorList.DeleteColor(DeleteColorName);
	m_SearchColorList.DeleteColor(DeleteColorName);
}

void EditColorDialog::OnLostFocus(wxFocusEvent& event)
{
//	if (event.GetId() == IDC_COLOR_NAME)
//	{
//		UpdateCurrentSelectColorName();
//	}
//	event.Skip();
}

void EditColorDialog::OnSearchKeySelected(wxKeyEvent& event)
{
	int currentSelect;
	wxString currentName;

	if (m_SearchCtrl1->GetValue().empty())
	{
		event.Skip();
		return;
	}

	currentSelect = m_ColorListBox->GetSelection();
	if (event.GetKeyCode() == WXK_DOWN)
	{
		currentSelect++;
		if (currentSelect >= m_ColorListBox->GetItemCount())
		{
			currentSelect = 0;
		}
		m_ColorListBox->SetSelection(currentSelect);
		ShowColorInforAtIndex(currentSelect);
	}
	else if (event.GetKeyCode() == WXK_UP)
	{
		currentSelect--;
		if (currentSelect < 0)
		{
			currentSelect = m_ColorListBox->GetItemCount() - 1;
		}
		m_ColorListBox->SetSelection(currentSelect);
		ShowColorInforAtIndex(currentSelect);
	}
	else if (event.GetKeyCode() == WXK_RETURN)
	{
		currentName = m_ColorListBox->GetColor(currentSelect).GetName();
		m_ColorListBox->SetColors(m_SrcColorList);
		m_ColorListBox->SetSelection(m_SrcColorList.FindColor(currentName));
		m_ColorListBox->RefreshSelected();
		m_ColorListBox->RefreshAll();
	    m_SearchCtrl1->Clear();
	    m_ColorListBox->SetFocus();
	}
	else
	{
		event.Skip();
	}
}

void EditColorDialog::OnSearch(wxCommandEvent& event)
{
	wxString text = m_SearchCtrl1->GetValue();
	wxString propety;
	wxString tempStr;
	wxArrayString subStr;
	char *p;
	char *buf;
	bool hasFound = false;
	m_ColorListBox->SetColors(m_SrcColorList);
	if (text.empty())
	{
		m_SearchCtrl1->ShowCancelButton(false);
		m_ColorListBox->RefreshSelected();
		m_ColorListBox->RefreshAll();
		return;
	}
	m_SearchCtrl1->ShowCancelButton(true);

	for (size_t i = 0; i < m_ColorListBox->GetItemCount(); i++)
	{
		propety += m_ColorListBox->GetColors().GetColor(i).GetName();
	}
	memset(&m_SearchColorList, 0, sizeof(ColorList));

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
	for (size_t i = 0; i < m_ColorListBox->GetItemCount(); i++)
	{
		tempStr = m_ColorListBox->GetColors().GetColor(i).GetName();
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
			if (m_SearchColorList.GetColor(m_ColorListBox->GetColor(i).GetName()).GetName() == "")
			{
				m_SearchColorList.AddColor(m_ColorListBox->GetColors().GetColor(i));
			}
		}
	}
	m_ColorListBox->SetColors(m_SearchColorList);
	if (hasFound)
	{
		m_ColorListBox->SetSelection(0);
		ShowColorInforAtIndex(0);
	}
	m_ColorListBox->RefreshSelected();
	m_ColorListBox->RefreshAll();
}

void EditColorDialog::OnSearchCancel(wxCommandEvent& event)
{
	m_SearchCtrl1->ShowCancelButton(false);
    m_ColorListBox->SetColors(m_SrcColorList);
}

void EditColorDialog::OnOK(wxCommandEvent& event)
{
	m_ColorListBox->SetColors(m_SrcColorList);

	OnApply(event);
	EndModal(wxID_OK);
}

void EditColorDialog::OnApply(wxCommandEvent& event)
{
	m_ColorListBox->SetColors(m_SrcColorList);
	m_SearchCtrl1->Clear();

	Manager::Get()->GetActiveProject()->SetColors(m_ColorListBox->GetColors());
	Colors::Get()->Refresh();
	Manager::Get()->GetActiveProject()->DoSetColors();
}


void EditColorDialog::ShowColorInforAtIndex(int index)
{
    uiColor color = m_ColorListBox->GetColor(index);
    unsigned char r, g, b, a;

    m_ColorHexValueStatic->SetLabel(color.GetValue());
    m_ColorNameText->SetLabel(color.GetName());

    a = (color.m_Value & 0xff000000) >> 24;
    m_ColorASlider->SetValue(a);
    m_ColorAText->SetLabel(wxString::Format("%d", a));

    r = (color.m_Value & 0x00ff0000) >> 16;
    m_ColorRSlider->SetValue(r);
    m_ColorRText->SetLabel(wxString::Format("%d", r));

    g = (color.m_Value & 0x0000ff00) >> 8;
    m_ColorGSlider->SetValue(g);
    m_ColorGText->SetLabel(wxString::Format("%d", g));

    b = (color.m_Value & 0x000000ff);
    m_ColorBSlider->SetValue(b);
    m_ColorBText->SetLabel(wxString::Format("%d", b));

    m_ShowColorStatic->SetBackgroundColour(wxColour(r, g, b, a));
    m_ShowColorStatic->Refresh();   /* static window will only refresh when text changed. so, we need force call it */
}

void EditColorDialog::UpdateCurrentSelectColorValue(void)
{
    unsigned char a, r, g, b;
    unsigned int colorValue;

    a = m_ColorASlider->GetValue();
    r = m_ColorRSlider->GetValue();
    g = m_ColorGSlider->GetValue();
    b = m_ColorBSlider->GetValue();
    colorValue = a << 24 | r << 16 | g << 8 | b;

    if (m_ColorListBox->UpdateColorValue(m_ColorListBox->GetSelection(), colorValue))
    	m_ColorListBox->RefreshSelected();

    m_SrcColorList.UpdateColorValue(m_SrcColorList.FindColor(m_ColorListBox->GetColor(m_ColorListBox->GetSelection()).GetName()), colorValue);
}

void EditColorDialog::UpdateCurrentSelectColorName(void)
{
	wxString CurrentName = m_ColorListBox->GetColor(m_ColorListBox->GetSelection()).GetName();

	if (m_ColorListBox->UpdateColorName(m_ColorListBox->GetSelection(), m_ColorNameText->GetValue()))
	{
		m_ColorListBox->RefreshSelected();
	}
	else
	{
		wxMessageBox(wxT("颜色名称已存在!"), APP_NAME, wxOK|wxICON_ERROR, this);
//		m_ColorNameText->SetFocus();
	}

    if (!(m_SrcColorList.UpdateColorName(m_SrcColorList.FindColor(CurrentName), m_ColorNameText->GetValue())))
    {
        wxMessageBox(wxT("颜色名称已存在(1)!"), APP_NAME, wxOK|wxICON_ERROR, this);
    }
}
