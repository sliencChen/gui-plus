#ifndef EDITCOLORDLG_H
#define EDITCOLORDLG_H

//(*Headers(EditColorDialog)
#include <wx/dialog.h>
class ColorListBox;
class wxBoxSizer;
class wxButton;
class wxSearchCtrl;
class wxSlider;
class wxStaticBoxSizer;
class wxStaticLine;
class wxStaticText;
class wxStdDialogButtonSizer;
class wxTextCtrl;
//*)

class EditColorDialog: public wxDialog
{
	public:

		EditColorDialog(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~EditColorDialog();

		//(*Declarations(EditColorDialog)
		ColorListBox* m_ColorListBox;
		wxButton* m_DeleteColorButton;
		wxButton* m_NewColorButton;
		wxButton* m_ViewBackgroundColor;
		wxSearchCtrl* m_SearchCtrl1;
		wxSlider* m_ColorASlider;
		wxSlider* m_ColorBSlider;
		wxSlider* m_ColorGSlider;
		wxSlider* m_ColorRSlider;
		wxStaticLine* StaticLine1;
		wxStaticLine* StaticLine2;
		wxStaticText* StaticText2;
		wxStaticText* StaticText3;
		wxStaticText* StaticText4;
		wxStaticText* StaticText5;
		wxStaticText* StaticText6;
		wxStaticText* m_ColorHexValueStatic;
		wxStaticText* m_ShowColorStatic;
		wxTextCtrl* m_ColorAText;
		wxTextCtrl* m_ColorBText;
		wxTextCtrl* m_ColorGText;
		wxTextCtrl* m_ColorNameText;
		wxTextCtrl* m_ColorRText;
		//*)

	protected:

		//(*Identifiers(EditColorDialog)
		static const long ID_SEARCHCTRL1;
		static const long IDC_COLOR_NEW;
		static const long IDC_COLOR_DEL;
		static const long IDC_COLOR_LIST;
		static const long IDC_SHOW_COLOR_STATIC;
		static const long IDC_COLOR_VALUE_HEX;
		static const long ID_BUTTON1;
		static const long ID_STATICLINE2;
		static const long ID_STATICTEXT2;
		static const long IDC_COLOR_NAME;
		static const long ID_STATICTEXT3;
		static const long IDC_COLOR_SLIDER_R;
		static const long IDC_COLOR_R;
		static const long ID_STATICTEXT4;
		static const long IDC_COLOR_SLIDER_G;
		static const long IDC_COLOR_G;
		static const long ID_STATICTEXT5;
		static const long IDC_COLOR_SLIDER_B;
		static const long IDC_COLOR_B;
		static const long ID_STATICTEXT6;
		static const long IDC_COLOR_SLIDER_A;
		static const long IDC_COLOR_A;
		static const long ID_STATICLINE1;
		//*)

		void OnColorListSelected(wxCommandEvent& event);
		void OnSliderValueChanged(wxScrollEvent& event);
		void OnColorNameChangeDone(wxCommandEvent& event);
		void OnNewColor(wxCommandEvent& event);
		void OnDeleteColor(wxCommandEvent& event);
		void OnLostFocus(wxFocusEvent& event);
		void OnSearch(wxCommandEvent& event);
		void OnSearchCancel(wxCommandEvent& event);
		void OnSearchKeySelected(wxKeyEvent& event);
		void OnOK(wxCommandEvent& event);
		void OnApply(wxCommandEvent& event);

	private:

		//(*Handlers(EditColorDialog)
		//*)
		void ShowColorInforAtIndex(int index);
		void UpdateCurrentSelectColorValue(void);
		void UpdateCurrentSelectColorName(void);
		ColorList m_SrcColorList;
		ColorList m_SearchColorList;
		DECLARE_EVENT_TABLE()
};

#endif
