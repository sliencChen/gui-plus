/*
 * main.h
 *
 *  Created on: 2019-3-14
 *      Author: Brant
 */

#ifndef MAINFRAME_H_INCLUDED
#define MAINFRAME_H_INCLUDED


#include <wx/wx.h>
#include <wx/aui/aui.h>
#include <wx/treectrl.h>
#include <wx/propgrid/manager.h>
#include <wx/propgrid/advprops.h>

#include "window/uiwindow.h"

#include "pagelist.h"
#include "maincanvas.h"
#include "recentitemslist.h"

class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString& title);
	~MainFrame(void);

	void OnClose(wxCloseEvent& event);
	void OnQuit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);

	void OnNewProject(wxCommandEvent& event);
	void OnNewPage(wxCommandEvent& event);
	void OnOpen(wxCommandEvent& event);
	void OnSave(wxCommandEvent& event);
	void OnExport(wxCommandEvent& event);
	void OnPackImage(wxCommandEvent& event);

	void OnUndo(wxCommandEvent& event);
	void OnRedo(wxCommandEvent& event);
	void OnCopy(wxCommandEvent& event);
	void OnPaste(wxCommandEvent& event);
	bool CanPaste(void);
	void OnSearch(wxCommandEvent& event);
	void OnClearOutputText(wxCommandEvent& event);

	void OnAddExistingPage(wxCommandEvent& event);
	void OnEditProjectImage(wxCommandEvent& event);
	void OnEditProjectColor(wxCommandEvent& event);
	void OnEditProjectLogo(wxCommandEvent& event);
	void OnEditProjectString(wxCommandEvent& event);
	void OnEditProjectProperty(wxCommandEvent& event);

	void OnEditThemeToggle(wxCommandEvent& event);
	void OnNewUIPageTheme(wxCommandEvent& event);
	void OnNewUIProgressBarTheme(wxCommandEvent& event);

	void OnAddWindowRequest(wxCommandEvent& event);

	void UpdateAppTitle(void);
	//    void UpdatePageList(void);
	void DoOpenProject(const wxString& filename, bool addToHistory = true);

	wxMenu  *LanguageOption         = nullptr;

protected:
	/* create all windows */
	void CreateAllWindow(void);

	void CreateMenuBar(void);
	void CreateControlsToolBar(void);
	void CreateComponentsToolBar(void);
	void CreateWindowTreeControl(void);
	void CreatePropGrid(void);
	void CreateCanvasWindow(void);
	void CreatePageListWindow(void);
	void CreateOutputWindow(void);
	void CreateThemeTreeWindow(void);
	void CreateThemeComponentTreeWindow(void);

	/* layout all windows */
	void LayoutWindow(void);

	void InitializeRecentFilesHistory();
	void TerminateRecentFilesHistory();
	void InitWindowView();


	static const long IDC_PAGE_LIST;
	static const long IDC_MAIN_CANVAS;

	RecentItemsList m_projectsHistory;

private:

	void SwitchEditorMode(void);

	void OnPageListSelected(wxCommandEvent& event);
	void OnCanvasClicked(CanvasEvent& event);
	void OnCanvasChanged(CanvasEvent& event);
	void OnCanvasWinRectChange(CanvasEvent& event);
	void OnCanvasCancelDrag(CanvasEvent& event);
	void OnPropertyGridChange(wxPropertyGridEvent& event);
	void OnWindowTreeSelectChanged(wxTreeEvent& event);
	void OnWindowTreeItemUp(wxCommandEvent& event);
	void OnWindowTreeItemDown(wxCommandEvent& event);
	void OnWindowTreeItemDelete(wxCommandEvent& event);

	void OnUpdateEditUI(wxUpdateUIEvent& event);
	void OnUpdateWindowTreeButtonUI(wxUpdateUIEvent& event);
	void OnUpdateMenuProjItemUI(wxUpdateUIEvent& event);
	void OnUpdateMenuFileItemUI(wxUpdateUIEvent& event);

	void OnFileReopenProject(wxCommandEvent& event);
	void OnFileOpenRecentProjectClearHistory(wxCommandEvent& event);

	/* Project Language Function */
	void OnUpdateProjectLanguage(wxUpdateUIEvent& event);
	void OnOpenProjectLanguage(wxCommandEvent& event);
	void OnChoiceProjectLanguage(wxCommandEvent& event);

	void OnWindowView(wxCommandEvent& event);
	void OnUpdateViewWindowItemUI(wxUpdateUIEvent& event);
	void UpdateWindowView(void);
	void OnWindowViewClose(wxAuiManagerEvent& event);

	void SetActivePage(unsigned int index);

	void AppendWindowTreeItemRecursively(wxTreeItemId& parentId, uiWindow *window);
	void UpdateTreeControl(void);

	void UpdatePropertyGrid(void);

	/* theme */
	void CreateNewThemeFor(wxString controlName, wxString themeName);

	/* Property */
	void RefreshProperty(void);
	wxPGProperty* AddPropertyGrid(PropertyBase *prop, wxString key);

	/* PageListWindow */
	void OnPageDelete(PageListEvent& event);

	/* return true if save/discard, return false if user cancel */
	bool SaveOrDiscardProjectChange(void);

	wxAuiManager m_LayoutManager;

	wxAuiToolBar			*m_ControlsToolBar		= nullptr;
	wxAuiToolBar			*m_ComponentsToolBar	= nullptr;
	PageListWindow			*m_PageList 			= nullptr;
	CanvasWindow			*m_CanvasWindow 		= nullptr;
	wxTreeCtrl				*m_WindowTreeControl 	= nullptr;
	wxPanel				    *m_WindowTreeControlPanel 	= nullptr;
	wxPropertyGridManager	*m_PropGridManager 		= nullptr;
	wxTextCtrl				*m_OutputTextCtrl 		= nullptr;
	wxTreeCtrl				*m_ThemeTree			= nullptr;
	wxTreeCtrl				*m_ThemeComponentTree	= nullptr;
	wxString m_AuiMgrNormalPerspective;
	wxString m_AuiMgrEditThemePerspective;


	PageFile *m_ActivePage = nullptr;
	uiWindow *m_ActiveRootWindow = nullptr;		///< root window of active page
	uiWindow *m_ActiveWindow = nullptr;			///< active window(selected item)
	uiWindow *m_CopyWindow = nullptr;			///< active window(selected item)
	bool m_EditThemeMode = false;				///< editing theme or not
	ConfigManager *m_ActiveConfigManger = nullptr;

	DECLARE_EVENT_TABLE()
};


enum
{
    IDM_FILE_QUIT = wxID_EXIT,
	IDM_HELP_ABOUT = wxID_ABOUT,

    IDM_FILE_NEW,
    IDM_FILE_NEW_PAGE,
    IDM_FILE_NEW_PROJ,
    IDM_FILE_OPEN,
    IDM_FILE_OPEN_RECENT,
    IDM_FILE_CLEAR_RECENT,
    IDM_FILE_SAVE,
    IDM_FILE_SAVE_AS,
    IDM_FILE_EXPORT,
    IDM_FILE_PACK_IMAGE,

    IDM_EDIT_UNDO,
    IDM_EDIT_REDO,
    IDM_EDIT_CUT,
    IDM_EDIT_COPY,
    IDM_EDIT_PASTE,
    IDM_EDIT_DELETE,
    IDM_EDIT_FIND,
    IDM_EDIT_CLEAR_OUTPUT_TEXT,

    IDM_VIEW_PAGE,
    IDM_VIEW_WIN_TREE,
    IDM_VIEW_PROP,
    IDM_VIEW_OUTPUT,

    IDM_PROJ_ADD_EXISTING_PAGE,
    IDM_PROJ_EDIT_IMAGE,
    IDM_PROJ_EDIT_COLOR,
    IDM_OPEN_LANGUAGE_FILE,

    IDM_LANGUAGE_BEGIN,
	IDM_LANGUAGE_END = IDM_LANGUAGE_BEGIN + MAX_LANGUAGE,

    IDM_PROJ_EDIT_LANGUAGE,
    IDM_PROJ_EDIT_LOGO,
    IDM_PROJ_PROPERTY,

    IDM_THEME_EDIT_THEME,
    IDM_THEME_NEW_UI_PAGE,
    IDM_THEME_NEW_UI_PROGRESS_BAR,
    IDM_THEME_NEW_UI_MENU_ITEM,
    IDM_THEME_NEW_UI_LIST_ITEM,
    IDM_THEME_NEW_UI_EDIT_ITEM,

    IDC_WINDOW_TREE_BUTTON_UP,
    IDC_WINDOW_TREE_BUTTON_DOWN,
    IDC_WINDOW_TREE_BUTTON_DEL,


    IDC_PROPERTY_GRID,

    IDC_WINDOW_TREE,
    IDC_THEME_TREE,
    IDC_THEME_COMPONENT_TREE,

    ID_MAX
};


enum
{
	TOOL_BAR_CONTROL	= 1000,
	TOOL_BAR_CONTROL_PLACE_HOLDER,
	TOOL_BAR_CONTROL_LABEL,
	TOOL_BAR_CONTROL_IMAGE,
	TOOL_BAR_CONTROL_LINE_CHART,
	TOOL_BAR_CONTROL_LIST,
	TOOL_BAR_CONTROL_EDIT,
	TOOL_BAR_CONTROL_EDIT_ITEM,
	TOOL_BAR_CONTROL_HELP,
	TOOL_BAR_CONTROL_PROMPT,
	TOOL_BAR_CONTROL_MESSAGE_BOX,
	TOOL_BAR_CONTROL_TEXT_FIELD,
	TOOL_BAR_CONTROL_LIST_SIZER,
	TOOL_BAR_CONTROL_BOX_SIZER,
	TOOL_BAR_CONTROL_MENU_ITEM,
	TOOL_BAR_CONTROL_MENU,
	TOOL_BAR_CONTROL_MAX,
};

enum
{
	TOOL_BAR_COMPONENT		= 2000,
	TOOL_BAR_COMPONENT_LINE,
	TOOL_BAR_COMPONENT_RECT,
	TOOL_BAR_COMPONENT_TRIANGLE,
	TOOL_BAR_COMPONENT_TEXT,
	TOOL_BAR_COMPONENT_IMAGE,
};


enum
{
    wxID_CBFILE01   = 3001, // Recent files...
    wxID_CBFILE02,
    wxID_CBFILE03,
    wxID_CBFILE04,
    wxID_CBFILE05,
    wxID_CBFILE06,
    wxID_CBFILE07,
    wxID_CBFILE08,
    wxID_CBFILE09,
    wxID_CBFILE10,
    wxID_CBFILE11,
    wxID_CBFILE12,
    wxID_CBFILE13,
    wxID_CBFILE14,
    wxID_CBFILE15,
    wxID_CBFILE16,
    wxID_CBFILE17, // 3017  // Starting here for recent projects...
    wxID_CBFILE18,
    wxID_CBFILE19,
    wxID_CBFILE20,
    wxID_CBFILE21,
    wxID_CBFILE22,
    wxID_CBFILE23,
    wxID_CBFILE24,
    wxID_CBFILE25,
    wxID_CBFILE26,
    wxID_CBFILE27,
    wxID_CBFILE28,
    wxID_CBFILE29,
    wxID_CBFILE30,
    wxID_CBFILE31,
    wxID_CBFILE32
};


#endif // MAINFRAME_H_INCLUDED
