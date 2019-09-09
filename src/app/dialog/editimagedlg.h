#ifndef EDITIMAGEDLG_H
#define EDITIMAGEDLG_H

//(*Headers(EditImageDialog)
#include <wx/dialog.h>
class ImageListBox;
class wxBoxSizer;
class wxButton;
class wxCheckBox;
class wxChoice;
class wxScrolledWindow;
class wxSearchCtrl;
class wxSlider;
class wxSpinCtrl;
class wxSpinEvent;
class wxStaticBitmap;
class wxStaticBoxSizer;
class wxStaticLine;
class wxStaticText;
class wxStdDialogButtonSizer;
//*)

class EditImageDialog: public wxDialog
{
	public:

		EditImageDialog(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~EditImageDialog();

		//(*Declarations(EditImageDialog)
		ImageListBox* m_ImageList;
		wxButton* m_AddButton;
		wxButton* m_AutoSliceButton;
		wxButton* m_DeleteButton;
		wxButton* m_ScaleDownButton;
		wxButton* m_ScaleOriginButton;
		wxButton* m_ScaleUpButton;
		wxButton* m_SelectKeyColorButton;
		wxCheckBox* m_UseKeyColor;
		wxChoice* m_ColorModeChoice;
		wxChoice* m_FillModeChoice;
		wxChoice* m_SliceModeChoice;
		wxScrolledWindow* m_PreviewImageContainer;
		wxSearchCtrl* m_SearchCtrl1;
		wxSlider* m_ScaleSlider;
		wxSpinCtrl* m_FillHeight;
		wxSpinCtrl* m_FillWidth;
		wxSpinCtrl* m_SliceBottom;
		wxSpinCtrl* m_SliceLeft;
		wxSpinCtrl* m_SliceRight;
		wxSpinCtrl* m_SliceTop;
		wxStaticBitmap* m_PreviewImage;
		wxStaticLine* StaticLine1;
		wxStaticLine* StaticLine2;
		wxStaticText* StaticText1;
		wxStaticText* StaticText2;
		wxStaticText* StaticText3;
		wxStaticText* StaticText4;
		wxStaticText* StaticText5;
		wxStaticText* StaticText6;
		wxStaticText* StaticText7;
		wxStaticText* StaticText8;
		wxStaticText* StaticText9;
		wxStaticText* m_ScaleLabel;
		//*)

	protected:

		//(*Identifiers(EditImageDialog)
		static const long ID_SEARCHCTRL1;
		static const long IDC_ADD_BUTTON;
		static const long IDC_DELETE_BUTTON;
		static const long IDC_IMAGE_LIST;
		static const long IDC_PREVIEW_IMAGE;
		static const long IDC_PREVIEW_IMAGE_CONTAINER;
		static const long IDC_SCALE_DOWN_BUTTON;
		static const long IDC_SCALE_SLIDER;
		static const long IDC_SCALE_LABEL;
		static const long IDC_SCALE_UP_BUTTON;
		static const long IDC_SCALE_ORIGIN_BUTTON;
		static const long ID_STATICTEXT1;
		static const long IDC_COLOR_MODE;
		static const long IDC_USE_KEY_COLOR;
		static const long IDC_SELECT_KEY_COLOR;
		static const long ID_STATICTEXT2;
		static const long IDC_SLICE_MODE;
		static const long IDC_AUTO_SLICE_BUTTON;
		static const long ID_STATICTEXT3;
		static const long IDC_SLICE_LEFT;
		static const long ID_STATICTEXT4;
		static const long IDC_SLICE_RIGHT;
		static const long ID_STATICTEXT5;
		static const long IDC_SLICE_TOP;
		static const long ID_STATICTEXT6;
		static const long IDC_SLICE_BOTTOM;
		static const long ID_STATICLINE2;
		static const long ID_STATICTEXT7;
		static const long IDC_FILL_MODE;
		static const long ID_STATICTEXT8;
		static const long IDC_FILL_WIDTH;
		static const long ID_STATICTEXT9;
		static const long IDC_FILL_HEIGHT;
		static const long ID_STATICLINE1;
		//*)

	private:

		//(*Handlers(EditImageDialog)
		void Onm_SearchComboBoxSelected(wxCommandEvent& event);
		//*)

		void OnResize(wxSizeEvent& event);

		void OnSearchKeySelected(wxKeyEvent& event);
		void OnSearchCancel(wxCommandEvent& event);
		void OnSearch(wxCommandEvent& event);
		void OnAdd(wxCommandEvent& event);
		void OnDelete(wxCommandEvent& event);

		void OnImageListSelectChanged(wxCommandEvent& event);
		void OnImageItemRename(wxCommandEvent& event);

		void OnScaleSliderValueChanged(wxCommandEvent& event);
		void OnScaleSliderValueChanged2(wxScrollEvent& event);
		void OnScaleDown(wxCommandEvent& event);
		void OnScaleUp(wxCommandEvent& event);
		void OnScaleOrigin(wxCommandEvent& event);

		void OnColorModeSelectChanged(wxCommandEvent& event);
		void OnUseKeyColorChanged(wxCommandEvent& event);
		void OnSelectKeyColor(wxCommandEvent& event);

		void OnSliceModeSelectionChanged(wxCommandEvent& event);
		void OnFillModeSelectionChanged(wxCommandEvent& event);
		void OnAutoSlice(wxCommandEvent& event);
		void OnSliceLeftValueChanged(wxSpinEvent& event);
		void OnSliceRightValueChanged(wxSpinEvent& event);
		void OnSliceTopValueChanged(wxSpinEvent& event);
		void OnSliceBottomValueChanged(wxSpinEvent& event);
		void OnFillWidthValueChanged(wxSpinEvent& event);
		void OnFillHeightValueChanged(wxSpinEvent& event);

		void UpdateCurrentImage(int selection);
		void ResizePreviewImage(void);
		void UpdateSliceMode(void);
		void UpdateSlicePositionValue(void);
		void ShowImage(Image* image);

		Image *m_CurrentImage = nullptr;
		ImageList *m_SearchImageList = nullptr;
		DECLARE_EVENT_TABLE()
};

#endif
