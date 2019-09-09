/*
 * main.cpp
 *
 *  Created on: 2019-3-14
 *      Author: Brant
 */

#include <wx/artprov.h>
#include "app_global.h"
#include "designer.h"
#include "main.h"
#include "dialog/newprojdlg.h"
#include "dialog/editimagedlg.h"
#include "dialog/editcolordlg.h"
#include "dialog/editlogosdlg.h"
#include "dialog/searchlistdlg.h"
#include "dialog/packimagedlg.h"
#include "images.h"
#include <wx/clipbrd.h>
#include "wxwidgets/wxsitemresdataobject.h"



const long MainFrame::IDC_PAGE_LIST = wxNewId();
const long MainFrame::IDC_MAIN_CANVAS = wxNewId();


BEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_CLOSE(MainFrame::OnClose)

	EVT_MENU(IDM_HELP_ABOUT, MainFrame::OnAbout)
	EVT_MENU(IDM_FILE_QUIT, MainFrame::OnQuit)

	EVT_MENU(IDM_FILE_NEW_PAGE, MainFrame::OnNewPage)
	EVT_MENU(IDM_FILE_NEW_PROJ, MainFrame::OnNewProject)
	EVT_MENU(IDM_FILE_OPEN, MainFrame::OnOpen)
	EVT_MENU(IDM_FILE_SAVE, MainFrame::OnSave)
	EVT_MENU(IDM_FILE_EXPORT, MainFrame::OnExport)
	EVT_MENU(IDM_FILE_PACK_IMAGE, MainFrame::OnPackImage)

	EVT_MENU(IDM_EDIT_UNDO, MainFrame::OnUndo)
	EVT_MENU(IDM_EDIT_REDO, MainFrame::OnRedo)
	EVT_MENU(IDM_EDIT_COPY, MainFrame::OnCopy)
	EVT_MENU(IDM_EDIT_PASTE, MainFrame::OnPaste)
	EVT_MENU(IDM_EDIT_FIND, MainFrame::OnSearch)
	EVT_MENU(IDM_EDIT_CLEAR_OUTPUT_TEXT, MainFrame::OnClearOutputText)

	EVT_UPDATE_UI_RANGE(IDM_EDIT_UNDO, IDM_EDIT_DELETE, MainFrame::OnUpdateEditUI)

	EVT_MENU(IDM_PROJ_ADD_EXISTING_PAGE, MainFrame::OnAddExistingPage)
	EVT_MENU(IDM_PROJ_EDIT_IMAGE, MainFrame::OnEditProjectImage)
	EVT_MENU(IDM_PROJ_EDIT_COLOR, MainFrame::OnEditProjectColor)
	EVT_MENU(IDM_OPEN_LANGUAGE_FILE, MainFrame::OnOpenProjectLanguage)
	EVT_MENU_RANGE(IDM_LANGUAGE_BEGIN, IDM_LANGUAGE_END, MainFrame::OnChoiceProjectLanguage)


	EVT_MENU(IDM_PROJ_EDIT_LOGO, MainFrame::OnEditProjectLogo)
	EVT_MENU(IDM_PROJ_PROPERTY, MainFrame::OnEditProjectProperty)

	EVT_MENU(IDM_THEME_EDIT_THEME, MainFrame::OnEditThemeToggle)
	EVT_MENU(IDM_THEME_NEW_UI_PAGE, MainFrame::OnNewUIPageTheme)
	EVT_MENU(IDM_THEME_NEW_UI_PROGRESS_BAR, MainFrame::OnNewUIProgressBarTheme)


	EVT_LISTBOX(IDC_PAGE_LIST, MainFrame::OnPageListSelected)
	EVT_CANVAS_CLICK(IDC_MAIN_CANVAS, MainFrame::OnCanvasClicked)
	EVT_CANVAS_CHANGE(IDC_MAIN_CANVAS, MainFrame::OnCanvasChanged)
	EVT_CANVAS_UPDATE_GRID(IDC_MAIN_CANVAS, MainFrame::OnCanvasWinRectChange)
	EVT_CANVAS_CANCEL_DRAG(IDC_MAIN_CANVAS, MainFrame::OnCanvasCancelDrag)

	EVT_PG_CHANGED(IDC_PROPERTY_GRID, MainFrame::OnPropertyGridChange)

    EVT_TREE_SEL_CHANGED(IDC_WINDOW_TREE, MainFrame::OnWindowTreeSelectChanged)

    EVT_BUTTON(IDC_WINDOW_TREE_BUTTON_UP, MainFrame::OnWindowTreeItemUp)
    EVT_BUTTON(IDC_WINDOW_TREE_BUTTON_DOWN, MainFrame::OnWindowTreeItemDown)
    EVT_BUTTON(IDC_WINDOW_TREE_BUTTON_DEL, MainFrame::OnWindowTreeItemDelete)
    EVT_UPDATE_UI_RANGE(IDC_WINDOW_TREE_BUTTON_UP, IDC_WINDOW_TREE_BUTTON_DEL, MainFrame::OnUpdateWindowTreeButtonUI)

    EVT_UPDATE_UI(IDM_PROJ_ADD_EXISTING_PAGE, MainFrame::OnUpdateMenuProjItemUI)
    EVT_UPDATE_UI(IDM_PROJ_EDIT_IMAGE, MainFrame::OnUpdateMenuProjItemUI)
    EVT_UPDATE_UI(IDM_PROJ_EDIT_COLOR, MainFrame::OnUpdateMenuProjItemUI)
    EVT_UPDATE_UI(IDM_PROJ_EDIT_LANGUAGE, MainFrame::OnUpdateMenuProjItemUI)
    EVT_UPDATE_UI(IDM_PROJ_EDIT_LOGO, MainFrame::OnUpdateMenuProjItemUI)
    EVT_UPDATE_UI(IDM_PROJ_PROPERTY, MainFrame::OnUpdateMenuProjItemUI)

    EVT_UPDATE_UI(IDM_FILE_NEW_PAGE, MainFrame::OnUpdateMenuFileItemUI)
    EVT_UPDATE_UI_RANGE(IDM_FILE_SAVE, IDM_FILE_PACK_IMAGE, MainFrame::OnUpdateMenuFileItemUI)

    EVT_UPDATE_UI_RANGE(IDM_LANGUAGE_BEGIN, IDM_LANGUAGE_END, MainFrame::OnUpdateProjectLanguage)

    EVT_MENU_RANGE(wxID_CBFILE17, wxID_CBFILE32,  MainFrame::OnFileReopenProject)
    EVT_MENU(IDM_FILE_CLEAR_RECENT, MainFrame::OnFileOpenRecentProjectClearHistory)

    EVT_MENU_RANGE(TOOL_BAR_CONTROL_LABEL, TOOL_BAR_CONTROL_MENU,  MainFrame::OnAddWindowRequest)

	EVT_MENU_RANGE(IDM_VIEW_PAGE, IDM_VIEW_OUTPUT,  MainFrame::OnWindowView)
	EVT_AUI_PANE_CLOSE(MainFrame::OnWindowViewClose)
	EVT_UPDATE_UI_RANGE(IDM_VIEW_PAGE, IDM_VIEW_OUTPUT,  MainFrame::OnUpdateViewWindowItemUI)

	EVT_PAGELIST_DELETE(IDC_PAGE_LIST, MainFrame::OnPageDelete)

END_EVENT_TABLE()

bool m_CreateNewPage = false;
wxString m_PagePath;

MainFrame::MainFrame(const wxString& title)
                : wxFrame(NULL, wxID_ANY, title),
                m_projectsHistory(wxT("文件(&F)"), wxT("/recent_projects"), IDM_FILE_CLEAR_RECENT, wxID_CBFILE17)
{
	/* save main frame window */
	Manager::Get()->SetAppFrame(this);

    /* app icon */
    SetIcon(wxICON(MAIN_ICON));

    /* config  */
    m_ActiveConfigManger = Manager::Get()->GetConfigManager(_T("app"));
    /* create default project */
//    Manager::Get()->SetActiveProject(new Project());

    /* create all window */
    CreateAllWindow();

    /* layout manager */
    m_LayoutManager.SetManagedWindow(this);
    LayoutWindow();

    /* window size [TODO]need load from config later */
    SetSize(FromDIP(wxSize(1280, 720)));

    UpdateAppTitle();

	InitWindowView();
}

MainFrame::~MainFrame(void)
{
    m_LayoutManager.UnInit();
}


//-------------------------------------------------------------------------------------------------
// menu action
//-------------------------------------------------------------------------------------------------
void MainFrame::OnClose(wxCloseEvent& event)
{
	if (!SaveOrDiscardProjectChange()) return;	/* user cancel */

	m_ActiveConfigManger->WriteWindowViewInfo("Window_View");
	TerminateRecentFilesHistory();
	Destroy();
}

void MainFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg;

    msg.Printf(wxT(APP_NAME_WITH_VERSION));
    msg += "\n\nBuild with wxWidgets-3.1.2 MSW Unicode";
    msg += "\nCopyright (c) 2019 Gotech";
    msg += "\nAuthor: Rixin Bai";
    msg += "\nEmail: bairixin2006@163.com";
    wxMessageBox(msg, wxT("关于 " APP_NAME), wxOK | wxICON_INFORMATION, this);
}


void MainFrame::OnNewPage(wxCommandEvent& event)
{
    wxString name;
	PageFile *page;
	unsigned int index;

    wxFileDialog fileDialog(this, wxT("选择文件"), wxEmptyString, wxEmptyString, "Designer Page File (*.page)|*.page|All File (*.*)|*.*");
    if (fileDialog.ShowModal() == wxID_CANCEL) return;

    if (wxFile::Exists(fileDialog.GetPath()))
    {
        wxString message(wxT("文件 "));
        message += fileDialog.GetFilename();
        message += wxT(" 已存在，是否覆盖？");

        wxMessageDialog askReplace(this, message, APP_NAME, wxYES_NO|wxYES_DEFAULT|wxICON_QUESTION);
//        askSave.SetYesNoLabels(wxT("保存"), wxT("不保存"), wxT("取消"));
        if (askReplace.ShowModal() == wxID_NO) return;
    }

    PageFile *pagefile = new PageFile(fileDialog.GetPath());
    Project *proj = Manager::Get()->GetActiveProject();
    if (proj->GetPageFileCount() > 0)
    {
again:
        name = m_ActivePage->GetCoderContext()->GetUniqueName(_T("Page"));

        for (index = 0;index<proj->GetPageFileCount();index++)
        {
            page = proj->GetPageFileAt(index);
            uiWindow *window = page->GetRootWindow();
            if (name == window->GetStrPropValue(PROP_NAME))
            {
                goto again;
            }
        }
    }
    else
        name = dsC2U("Page_1");

    Manager::Get()->GetActiveProject()->AddPageFile(pagefile);
    m_PageList->ReloadAllData();
    m_PageList->SelectLast();
    SetActivePage(m_PageList->GetSelection());
    m_ActiveWindow->SetStringProperty(PROP_NAME,	(char *)dsU2C(name));
    UpdateTreeControl();
    UpdatePropertyGrid();
    m_ActivePage->MarkAsModified();
}

void MainFrame::OnNewProject(wxCommandEvent& event)
{
    if (!SaveOrDiscardProjectChange()) return;

    NewProjectDialog dlg(this);
    if (dlg.ShowModal() != wxID_OK) return;

    // start create new project
    Project *newProj = new Project(dlg.m_ProjectName, dlg.m_CanvasWidth, dlg.m_CanvasHeight, dlg.m_ColorMode);
    if (!newProj->Save())	/* can't do next step if save fail */
    {
    	delete newProj;
    	return;
    }

    Manager::Get()->SetActiveProject(newProj);
    m_projectsHistory.AddToHistory(newProj->GetFileName());

    uiRender::SetCanvasProperty(newProj->GetCanvasSize(), newProj->GetColorMode());
    m_CanvasWindow->SetCanvasSize(newProj->GetCanvasSize());
    UpdateAppTitle();
}

void MainFrame::OnOpen(wxCommandEvent& event)
{
    if (!SaveOrDiscardProjectChange()) return;

    wxFileDialog openFileDialog(Manager::Get()->GetAppFrame(), wxT("打开"), "", "", "Designer Project File (*.osd)|*.osd|All File (*.*)|*.*", wxFD_OPEN|wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL) return;

    DoOpenProject(openFileDialog.GetPath(), true);
}

void MainFrame::OnSave(wxCommandEvent& event)
{
	if (Manager::Get()->GetActiveProject())
		Manager::Get()->GetActiveProject()->Save();
}

void MainFrame::OnExport(wxCommandEvent& event)
{
	if (!Manager::Get()->GetActiveProject())
		return;

	wxDirDialog dlg(this, wxT("选择导出目录"), wxEmptyString, wxDD_DEFAULT_STYLE|wxDD_DIR_MUST_EXIST|wxDD_NEW_DIR_BUTTON);
	if (dlg.ShowModal() == wxID_OK)
	{
		Manager::Get()->GetActiveProject()->Export(dlg.GetPath());
	}
}

void MainFrame::OnPackImage(wxCommandEvent& event)
{
	PackImageDialog dlg(this);

	dlg.ShowModal();
}

void MainFrame::OnUndo(wxCommandEvent& event)
{
    if (m_ActivePage == nullptr) return;

    m_ActivePage->Undo();

    m_PageList->ReloadAllData();
    SetActivePage(m_PageList->GetSelection());
    m_CanvasWindow->ReloadData();
    UpdateTreeControl();
    UpdatePropertyGrid();
}

void MainFrame::OnRedo(wxCommandEvent& event)
{
    if (m_ActivePage == nullptr) return;

    m_ActivePage->Redo();

    m_PageList->ReloadAllData();
    SetActivePage(m_PageList->GetSelection());
    m_CanvasWindow->ReloadData();
    UpdateTreeControl();
    UpdatePropertyGrid();
}

void MainFrame::OnCopy(wxCommandEvent& event)
{
    if ( !wxTheClipboard->Open() ) return;
    wxsItemResDataObject* Data = new wxsItemResDataObject;
    Data->AddItem(m_ActiveWindow);
    if ( m_ActiveWindow == m_ActiveRootWindow )
    {
        m_CreateNewPage = true;
        m_PagePath = m_ActivePage->GetFileName();
    }
    else
    {
        m_CreateNewPage = false;
    }

	m_CopyWindow = m_ActiveWindow;
    wxTheClipboard->SetData(Data);
    wxTheClipboard->Close();
}

bool MainFrame::CanPaste(void)
{
    if ( !wxTheClipboard->Open() ) return false;
    bool Res = wxTheClipboard->IsSupported(wxsDF_WIDGET);
    wxTheClipboard->Close();
    return Res;
}

void MainFrame::OnPaste(wxCommandEvent& event)
{
    if ( !wxTheClipboard->Open() ) return;

    wxString name;
	PageFile *page;
	unsigned int index;
    wxsItemResDataObject Data;

    if ( wxTheClipboard->GetData(Data) )
    {
        if ( true == m_CreateNewPage )
        {
            m_ActivePage->SaveAs(m_ActiveWindow,m_PagePath);
            wxLogMessage("m_PagePath = %s,m_ActivePage->GetFileName() = %s",m_PagePath,m_ActivePage->GetFileName());
            Manager::Get()->GetActiveProject()->AddPageFile(new PageFile(m_ActivePage->GetFileName(),false));
again:
            name = m_ActivePage->GetCoderContext()->GetUniqueName(_T("Page"));
            Project *proj = Manager::Get()->GetActiveProject();

            for (index = 0;index<proj->GetPageFileCount();index++)
            {
                page = proj->GetPageFileAt(index);
                uiWindow *window = page->GetRootWindow();
                if (name == window->GetStrPropValue(PROP_NAME))
                {
                    goto again;
                }
            }
            m_PageList->ReloadAllData();
            m_PageList->SelectLast();
            SetActivePage(m_PageList->GetSelection());
            m_ActiveWindow->SetStringProperty(PROP_NAME,	(char *)dsU2C(name));
        }
        else
        {
			if (Data.CheckIsItemWindow(m_ActiveWindow, m_CopyWindow) == true)
			{
				Data.LoadFromClipBoard(m_ActiveWindow, m_WindowTreeControl, m_ActivePage);
			}
			else if (Data.CheckIsItemWindow(m_ActiveWindow, m_CopyWindow) == false)
			{
				Data.LoadFromClipBoard(m_ActiveRootWindow, m_WindowTreeControl, m_ActivePage);
			}
        }
        m_ActivePage->MarkAsModified();
        m_CanvasWindow->ReloadData();
        UpdateTreeControl();
		UpdatePropertyGrid();
    }
    wxTheClipboard->Close();
}


void MainFrame::OnAddExistingPage(wxCommandEvent& event)
{
    wxFileDialog addPageDialog(this, wxT("选择文件"), wxEmptyString, wxEmptyString, "Designer Page File (*.page)|*.page|All File (*.*)|*.*",
    		wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);
    if (addPageDialog.ShowModal() == wxID_CANCEL) return;

    wxArrayString paths;
    addPageDialog.GetPaths(paths);

    for (size_t i = 0; i < paths.GetCount(); i++)
    {
    	Manager::Get()->GetActiveProject()->AddPageFile(new PageFile(paths.Item(i), false));
    }

    m_PageList->ReloadAllData();
    m_PageList->SelectLast();
    SetActivePage(m_PageList->GetSelection());
    m_ActivePage->MarkAsModified();
}

void MainFrame::OnEditProjectImage(wxCommandEvent& event)
{
	EditImageDialog dlg(this);
	dlg.ShowModal();
	Manager::Get()->GetActiveProject()->m_Images.ApplyToGUISystem();
	UpdatePropertyGrid();
	m_CanvasWindow->ReloadData();
	m_PageList->ReloadAllData();
}

void MainFrame::OnEditProjectColor(wxCommandEvent& event)
{
	EditColorDialog dlg(this);
	dlg.ShowModal();

	UpdatePropertyGrid();
	m_CanvasWindow->ReloadData();
	m_PageList->ReloadAllData();
}

void MainFrame::OnEditProjectLogo(wxCommandEvent& event)
{
	EditLogosDialog dlg(this);
	dlg.ShowModal();
	Manager::Get()->GetActiveProject()->m_LogoImage.ApplyToGUISystem();
	UpdatePropertyGrid();
	m_CanvasWindow->ReloadData();
	m_PageList->ReloadAllData();
}

void MainFrame::OnEditProjectProperty(wxCommandEvent& event)
{
    NewProjectDialog dlg(this);
	dlg.SetTitle(_T("工程属性") );

    Project *proj = Manager::Get()->GetActiveProject();
    wxSize size = proj->GetCanvasSize();
	dlg.textProjectName->SetValue(proj->GetName());
    dlg.textProjectWidth->SetValue(wxString::Format("%d",size.GetWidth()));
    dlg.textProjectHeight->SetValue(wxString::Format("%d",size.GetHeight()));
    dlg.choiceProjectColor->SetSelection(proj->GetColorMode());

    if (dlg.ShowModal() != wxID_OK) return;
    proj->SetName(dlg.m_ProjectName);
    size.SetWidth(dlg.m_CanvasWidth);
    size.SetHeight(dlg.m_CanvasHeight);
    proj->SetCanvasSize(size);
    proj->SetColorMode(dlg.m_ColorMode);

    PageFile *page;
    unsigned int index;
    if (proj->GetPageFileCount() > 0)
    {
        for (index = 0;index<proj->GetPageFileCount();index++)
        {
            page = proj->GetPageFileAt(index);
            uiWindow *window = page->GetRootWindow();
            window->SetProperty(PROP_POSITION, wxRect(0, 0, dlg.m_CanvasWidth, dlg.m_CanvasHeight));
            page->MarkAsModified();
        }
    }

    if (!proj->Save())	/* can't do next step if save fail */
    {
        return;
    }

    uiRender::SetCanvasProperty(proj->GetCanvasSize(), proj->GetColorMode());
    m_CanvasWindow->SetCanvasSize(proj->GetCanvasSize());
    UpdateAppTitle();
    UpdatePropertyGrid();
    m_CanvasWindow->ReloadData();
	m_PageList->ReloadAllData();
}

void MainFrame::OnSearch(wxCommandEvent& event)
{
    SearchListDialog dlg(this);
    dlg.ShowModal();
}

void MainFrame::OnClearOutputText(wxCommandEvent& event)
{
	m_OutputTextCtrl->Clear();
}

void MainFrame::OnOpenProjectLanguage(wxCommandEvent& event)
{
	wxFileDialog fileDialog(Manager::Get()->GetAppFrame(), wxT("选择文件"), wxEmptyString, wxEmptyString,
			wxT("BIN文件|*.bin"),
			wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	if (fileDialog.ShowModal() == wxID_CANCEL || Manager::Get()->GetActiveProject() == NULL)
	{
		return;
	}
	Manager::Get()->GetActiveProject()->AddLanguageData(fileDialog.GetPath());
}

void MainFrame::OnChoiceProjectLanguage(wxCommandEvent& event)
{
	unsigned int index;
	Manager::Get()->GetActiveProject()->SetProjectLanguage(event.GetId() - IDM_LANGUAGE_BEGIN);

	for (index = 0; index < Manager::Get()->GetActiveProject()->GetPageFileCount(); index++)
	{
		uiWindow *window = m_ActiveRootWindow->m_Childs;
		while (window)
		{
			uiWindow *window_1 = window->m_Childs;
			while (window_1)
			{
				window_1->RefreshString();
				window_1 = window_1->m_Next;
			}
			window->RefreshString();
			window = window->m_Next;
		}
	}

	UpdatePropertyGrid();
	m_ActivePage->MarkAsModified();
	m_CanvasWindow->ReloadData();
	m_PageList->Refresh();
}


void MainFrame::OnEditThemeToggle(wxCommandEvent& event)
{
	m_EditThemeMode = !m_EditThemeMode;
	SwitchEditorMode();
}


void MainFrame::OnNewUIPageTheme(wxCommandEvent& event)
{
	uiPageTheme *newTheme = new uiPageTheme();

	m_CanvasWindow->SetTheme(newTheme);
	CreateNewThemeFor("UIPage", newTheme->FullDisplayName());
}

void MainFrame::OnNewUIProgressBarTheme(wxCommandEvent& event)
{
	CreateNewThemeFor("UIProgressBar", "New ProgressBar Theme");
}


//-------------------------------------------------------------------------------------------------
// create window
//-------------------------------------------------------------------------------------------------
void MainFrame::CreateAllWindow(void)
{
	/* menu bar */
	CreateMenuBar();

	/* status bar */
	CreateStatusBar(2);

	/* tool bar */
	CreateControlsToolBar();
	CreateComponentsToolBar();

	/* window tree */
	CreateWindowTreeControl();

	/* create property grid */
	CreatePropGrid();

	/* canvas window */
	CreateCanvasWindow();

	/* page list window */
	CreatePageListWindow();

	/* output window */
	CreateOutputWindow();

	/* theme tree */
	CreateThemeTreeWindow();

	/* theme component tree */
	CreateThemeComponentTreeWindow();
}

void MainFrame::CreateMenuBar(void)
{
	// file menu
	wxMenu *fileMenu = new wxMenu;

	wxMenu *fileNew = new wxMenu;
	wxMenuItem *fileNewPage = new wxMenuItem(fileNew, IDM_FILE_NEW_PAGE, wxT("页面(&P)...\tCtrl-N"), wxT("添加新页面到当前工程"));
	fileNewPage->SetBitmap(wxArtProvider::GetBitmap(wxART_NEW, wxART_MENU, FromDIP(wxSize(16,16))));
	fileNew->Append(fileNewPage);

	wxMenuItem *fileNewProj = new wxMenuItem(fileNew, IDM_FILE_NEW_PROJ, wxT("工程..."), wxT("创建新的工程"));
	fileNewProj->SetBitmap(wxArtProvider::GetBitmap(wxART_NEW_DIR, wxART_MENU, FromDIP(wxSize(16,16))));
	fileNew->Append(fileNewProj);

	fileMenu->AppendSubMenu(fileNew, wxT("新建(&N)"));
	fileMenu->AppendSeparator();

	wxMenuItem *fileOpen = new wxMenuItem(fileMenu, IDM_FILE_OPEN, wxT("打开(&O)...\tCtrl-O"), wxT("打开文件"));
	fileOpen->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_MENU, FromDIP(wxSize(16,16))));
	fileMenu->Append(fileOpen);

	// recent project
	wxMenu *fileRecentPrj = new wxMenu;
	//    fileMenu->Append(IDM_FILE_OPEN_RECENT, wxT("历史记录"), wxT("从最近编辑的列表中打开文件"));  //[TODO] need implement as sub menu.
	wxMenuItem *fileRecentClear = new wxMenuItem(fileRecentPrj, IDM_FILE_CLEAR_RECENT, wxT("Clear history"), wxT("清除历史记录"));
	fileRecentPrj->Append(fileRecentClear);
	fileMenu->Append(IDM_FILE_OPEN_RECENT, wxT("历史记录"), fileRecentPrj, wxT("从最近编辑的列表中打开文件"));
	fileMenu->AppendSeparator();

	wxMenuItem *saveProject = new wxMenuItem(fileMenu, IDM_FILE_SAVE, wxT("保存工程(&S)\tCtrl-S"), wxT("保存工程文件"));
	saveProject->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_MENU, FromDIP(wxSize(16,16))));
	fileMenu->Append(saveProject);

	wxMenuItem *saveProjectAs = new wxMenuItem(fileMenu, IDM_FILE_SAVE_AS, wxT("工程另存为..."), wxT("指定其它文件名保存工程文件"));
	saveProjectAs->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_SAVE_AS, wxART_MENU, FromDIP(wxSize(16,16))));
	fileMenu->Append(saveProjectAs);
	fileMenu->AppendSeparator();

	fileMenu->Append(IDM_FILE_EXPORT, wxT("导出工程..."), wxT("导出到STB工程"));
	fileMenu->AppendSeparator();

	fileMenu->Append(IDM_FILE_PACK_IMAGE, wxT("图片打包..."), wxT("将图片打包为单个素材文件"));
	fileMenu->AppendSeparator();

	wxMenuItem *quit = new wxMenuItem(fileMenu, IDM_FILE_QUIT, wxT("退出(&Q)\tCtrl-Q"), wxT("退出程序"));
	quit->SetBitmap(wxArtProvider::GetBitmap(wxART_QUIT, wxART_MENU, FromDIP(wxSize(16,16))));
	fileMenu->Append(quit);


	// edit menu
	wxMenu *editMenu = new wxMenu;
	editMenu->Append(IDM_EDIT_UNDO, wxT("撤消\tCtrl-Z"), wxT("撤消本次修改"));
	editMenu->Append(IDM_EDIT_REDO, wxT("重做\tCtrl-Y"), wxT("恢复上次修改"));
	editMenu->AppendSeparator();
	editMenu->Append(IDM_EDIT_CUT, wxT("剪切\tCtrl-X"), wxT("移动当前项到剪贴板"));
	editMenu->Append(IDM_EDIT_COPY, wxT("复制\tCtrl-C"), wxT("复制当前项到剪贴板"));
	editMenu->Append(IDM_EDIT_PASTE, wxT("粘贴\tCtrl-V"), wxT("粘贴到此处"));
	editMenu->Append(IDM_EDIT_DELETE, wxT("删除\tDel"), wxT("删除当前项"));
	editMenu->AppendSeparator();
	editMenu->Append(IDM_EDIT_FIND, wxT("查找\tCtrl-F"), wxT("查找"));
	editMenu->AppendSeparator();
	editMenu->Append(IDM_EDIT_CLEAR_OUTPUT_TEXT, wxT("清空输出框\tCtrl-L"), wxT("清空输出框"));


	// view menu
	wxMenu *viewMenu = new wxMenu;
	viewMenu->AppendCheckItem(IDM_VIEW_PAGE, wxT("页面列表窗口"), wxT("显示/隐藏页面列表窗口"));
	viewMenu->AppendCheckItem(IDM_VIEW_WIN_TREE, wxT("控件树窗口"), wxT("显示/隐藏控件树窗口"));
	viewMenu->AppendCheckItem(IDM_VIEW_PROP, wxT("属性窗口"), wxT("显示/隐藏属性窗口"));
	viewMenu->AppendCheckItem(IDM_VIEW_OUTPUT, wxT("输出窗口"), wxT("显示/隐藏输出窗口"));



	// project menu
	wxMenu *projMenu = new wxMenu;
	wxMenuItem *addExistingPage = new wxMenuItem(projMenu, IDM_PROJ_ADD_EXISTING_PAGE, wxT("添加现有页面(&A)..."), wxT("添加目前已存在的页面文件到工程"));
	addExistingPage->SetBitmap(wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_MENU, FromDIP(wxSize(16,16))));
	projMenu->Append(addExistingPage);
	projMenu->AppendSeparator();
	projMenu->Append(IDM_PROJ_EDIT_IMAGE, wxT("编辑图片(&I)...\tCtrl-Shift-I"), wxT("添加/修改/删除工程使用的图片"));
	projMenu->Append(IDM_PROJ_EDIT_COLOR, wxT("编辑颜色(&C)...\tCtrl-Shift-C"), wxT("添加/修改/删除工程使用的颜色"));

	wxMenu *language = new wxMenu;
	wxMenuItem *NewLanguagefile = new wxMenuItem(language, IDM_OPEN_LANGUAGE_FILE, wxT("打开语言(&P)...\tCtrl-N"), wxT("打开语言添加到工程中"));
	language->Append(NewLanguagefile);
	LanguageOption = new wxMenu;
	language->AppendSubMenu(LanguageOption, wxT("语言选择(&N)"));
	//    projMenu->AppendSubMenu(language, wxT("语言选择窗口"));
	projMenu->Append(IDM_PROJ_EDIT_LANGUAGE, wxT("语言选择窗口"), language);

	projMenu->Append(IDM_PROJ_EDIT_LOGO, wxT("编辑Logo(&L)..."), wxT("添加/修改/删除工程使用的Logo"));
	projMenu->AppendSeparator();
	projMenu->Append(IDM_PROJ_PROPERTY, wxT("属性(&P)..."), wxT("修改工程属性"));


	// theme menu
	wxMenu *themeMenu = new wxMenu;
	themeMenu->Append(IDM_THEME_EDIT_THEME, wxT("编辑主题(&E)\tCtrl-Shift-T"), wxT("编辑控件的主题风格"), true);
	wxMenu *themeNew = new wxMenu;
	themeNew->Append(IDM_THEME_NEW_UI_PAGE, wxT("UIPage"), wxT("新建UIPage的主题风格"));
	themeNew->Append(IDM_THEME_NEW_UI_PROGRESS_BAR, wxT("UIProgressBar"), wxT("新建UIProgressBar的主题风格"));
	themeNew->Append(IDM_THEME_NEW_UI_MENU_ITEM, wxT("UIMenuItem"), wxT("新建UIMenuItem的主题风格"));
	themeNew->Append(IDM_THEME_NEW_UI_LIST_ITEM, wxT("UIListItem"), wxT("新建UIListItem的主题风格"));
	themeNew->Append(IDM_THEME_NEW_UI_EDIT_ITEM, wxT("UIEditItem"), wxT("新建UIEditItem的主题风格"));

	themeMenu->AppendSeparator();
	themeMenu->AppendSubMenu(themeNew, wxT("新建控件主题(&N)"));


	// help menu
	wxMenu *helpMenu = new wxMenu;
	helpMenu->Append(IDM_HELP_ABOUT, wxT("关于(&A)..."), wxT("关于本程序"));


	// menu bar
	wxMenuBar *menuBar = new wxMenuBar();
	menuBar->Append(fileMenu, wxT("文件(&F)"));
	menuBar->Append(editMenu, wxT("编辑(&E)"));
	menuBar->Append(viewMenu, wxT("视图(&V)"));
	menuBar->Append(projMenu, wxT("工程(&P)"));
	menuBar->Append(themeMenu, wxT("主题(&T)"));
	menuBar->Append(helpMenu, wxT("帮助(&H)"));

	SetMenuBar(menuBar);

	InitializeRecentFilesHistory();
}

void MainFrame::CreateControlsToolBar(void)
{
	m_ControlsToolBar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	m_ControlsToolBar->SetToolBitmapSize(FromDIP(wxSize(16,16)));

	wxBitmap toolBarIcon = wxArtProvider::GetBitmap(wxART_FOLDER, wxART_OTHER, FromDIP(wxSize(16,16)));
	m_ControlsToolBar->AddTool(TOOL_BAR_CONTROL_PLACE_HOLDER, 	"Place Holder", 	wxBITMAP_PNG(ARROW_ICON), "Place Holder", wxITEM_RADIO);
	m_ControlsToolBar->AddTool(TOOL_BAR_CONTROL_LABEL, 			"UILabel", 			wxBITMAP_PNG(LABEL_ICON), "UILabel", wxITEM_RADIO);
	m_ControlsToolBar->AddTool(TOOL_BAR_CONTROL_IMAGE, 			"UIImage", 			wxBITMAP_PNG(IMAGES_ICON), "UIImage", wxITEM_RADIO);
	m_ControlsToolBar->AddTool(TOOL_BAR_CONTROL_LINE_CHART, 	"UILineChart", 		wxBITMAP_PNG(LINECHART_ICON), "UILineChart", wxITEM_RADIO);
	m_ControlsToolBar->AddTool(TOOL_BAR_CONTROL_LIST, 			"UIList", 			toolBarIcon, "UIList", wxITEM_RADIO);
	m_ControlsToolBar->AddTool(TOOL_BAR_CONTROL_EDIT, 			"UIEdit", 			toolBarIcon, "UIEdit", wxITEM_RADIO);
	m_ControlsToolBar->AddTool(TOOL_BAR_CONTROL_EDIT_ITEM, 		"UIEditItem", 		toolBarIcon, "UIEditItem", wxITEM_RADIO);
	m_ControlsToolBar->AddTool(TOOL_BAR_CONTROL_MENU, 			"UIMenu", 			toolBarIcon, "UIMenu", wxITEM_RADIO);
	m_ControlsToolBar->AddTool(TOOL_BAR_CONTROL_MENU_ITEM, 		"UIMenuItem", 		toolBarIcon, "UIMenuItem", wxITEM_RADIO);
	m_ControlsToolBar->AddTool(TOOL_BAR_CONTROL_PROMPT, 		"UIPrompt", 		toolBarIcon, "UIPrompt", wxITEM_RADIO);
	m_ControlsToolBar->AddTool(TOOL_BAR_CONTROL_MESSAGE_BOX, 	"UIMessageBox", 	toolBarIcon, "UIMessageBox", wxITEM_RADIO);
	m_ControlsToolBar->AddTool(TOOL_BAR_CONTROL_TEXT_FIELD, 	"UITextField", 		toolBarIcon, "UITextField", wxITEM_RADIO);
	m_ControlsToolBar->AddTool(TOOL_BAR_CONTROL_LIST_SIZER, 	"UIListSizer", 		toolBarIcon, "UIListSizer", wxITEM_RADIO);
	m_ControlsToolBar->AddTool(TOOL_BAR_CONTROL_BOX_SIZER, 		"UIBoxSizer", 		toolBarIcon, "UIBoxSizer", wxITEM_RADIO);
	m_ControlsToolBar->Realize();
}

void MainFrame::CreateComponentsToolBar(void)
{
	m_ComponentsToolBar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	m_ComponentsToolBar->SetToolBitmapSize(FromDIP(wxSize(16,16)));

	m_ComponentsToolBar->AddTool(TOOL_BAR_CONTROL_PLACE_HOLDER, "Place Holder", wxBITMAP_PNG(ARROW_ICON), "Place Holder", wxITEM_RADIO);
	m_ComponentsToolBar->AddTool(TOOL_BAR_COMPONENT_LINE, "Line", wxBITMAP_PNG(LINE_ICON), "Line", wxITEM_RADIO);
	m_ComponentsToolBar->AddTool(TOOL_BAR_COMPONENT_RECT, "Rectangle", wxBITMAP_PNG(RECT_ICON), "Rectangle", wxITEM_RADIO);
	m_ComponentsToolBar->AddTool(TOOL_BAR_COMPONENT_TRIANGLE, "Triangle", wxBITMAP_PNG(TRIANGLE_ICON), "Triangle", wxITEM_RADIO);
	m_ComponentsToolBar->AddTool(TOOL_BAR_COMPONENT_TEXT, "Text", wxBITMAP_PNG(LABEL_ICON), "Text", wxITEM_RADIO);
	m_ComponentsToolBar->AddTool(TOOL_BAR_COMPONENT_IMAGE, "Image", wxBITMAP_PNG(IMAGES_ICON), "Image", wxITEM_RADIO);
	m_ComponentsToolBar->Realize();
}

void MainFrame::CreateWindowTreeControl(void)
{
	if (m_WindowTreeControlPanel == nullptr)
	{
		//m_WindowTreeControl = new wxTreeCtrl(this, IDC_WINDOW_TREE, wxPoint(0,0), FromDIP(wxSize(160, 250)), wxTR_DEFAULT_STYLE | wxNO_BORDER);
		m_WindowTreeControlPanel = new wxPanel(this);

		if (m_WindowTreeControl == nullptr )
		{
			m_WindowTreeControl = new wxTreeCtrl(m_WindowTreeControlPanel, IDC_WINDOW_TREE, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxNO_BORDER);

            wxImageList *images = new wxImageList(16, 16, true);
            wxBusyCursor wait;
            wxIcon icons[TOOL_BAR_CONTROL_MAX - TOOL_BAR_CONTROL_PLACE_HOLDER];
            wxSize iconSize(16, 16);

            icons[TOOL_BAR_CONTROL_PLACE_HOLDER - TOOL_BAR_CONTROL_PLACE_HOLDER] = wxArtProvider::GetIcon(wxART_FOLDER, wxART_OTHER, iconSize);
            icons[TOOL_BAR_CONTROL_LABEL - TOOL_BAR_CONTROL_PLACE_HOLDER] = wxArtProvider::GetIcon(wxART_NORMAL_FILE, wxART_OTHER, iconSize);
            icons[TOOL_BAR_CONTROL_IMAGE - TOOL_BAR_CONTROL_PLACE_HOLDER] = wxArtProvider::GetIcon(wxART_NORMAL_FILE, wxART_OTHER, iconSize);
            icons[TOOL_BAR_CONTROL_LINE_CHART - TOOL_BAR_CONTROL_PLACE_HOLDER] = wxArtProvider::GetIcon(wxART_NORMAL_FILE, wxART_OTHER, iconSize);
            icons[TOOL_BAR_CONTROL_LIST - TOOL_BAR_CONTROL_PLACE_HOLDER] = wxArtProvider::GetIcon(wxART_NORMAL_FILE, wxART_OTHER, iconSize);
            icons[TOOL_BAR_CONTROL_EDIT - TOOL_BAR_CONTROL_PLACE_HOLDER] = wxArtProvider::GetIcon(wxART_FOLDER, wxART_OTHER, iconSize);
            icons[TOOL_BAR_CONTROL_EDIT_ITEM - TOOL_BAR_CONTROL_PLACE_HOLDER] = wxArtProvider::GetIcon(wxART_NORMAL_FILE, wxART_OTHER, iconSize);
            icons[TOOL_BAR_CONTROL_HELP - TOOL_BAR_CONTROL_PLACE_HOLDER] = wxArtProvider::GetIcon(wxART_NORMAL_FILE, wxART_OTHER, iconSize);
            icons[TOOL_BAR_CONTROL_PROMPT - TOOL_BAR_CONTROL_PLACE_HOLDER] = wxArtProvider::GetIcon(wxART_NORMAL_FILE, wxART_OTHER, iconSize);
            icons[TOOL_BAR_CONTROL_MESSAGE_BOX - TOOL_BAR_CONTROL_PLACE_HOLDER] = wxArtProvider::GetIcon(wxART_NORMAL_FILE, wxART_OTHER, iconSize);
            icons[TOOL_BAR_CONTROL_TEXT_FIELD - TOOL_BAR_CONTROL_PLACE_HOLDER] = wxArtProvider::GetIcon(wxART_NORMAL_FILE, wxART_OTHER, iconSize);
            icons[TOOL_BAR_CONTROL_LIST_SIZER - TOOL_BAR_CONTROL_PLACE_HOLDER] = wxArtProvider::GetIcon(wxART_FOLDER, wxART_OTHER, iconSize);
            icons[TOOL_BAR_CONTROL_BOX_SIZER - TOOL_BAR_CONTROL_PLACE_HOLDER] = wxArtProvider::GetIcon(wxART_FOLDER, wxART_OTHER, iconSize);
            icons[TOOL_BAR_CONTROL_MENU_ITEM - TOOL_BAR_CONTROL_PLACE_HOLDER] = wxArtProvider::GetIcon(wxART_NORMAL_FILE, wxART_OTHER, iconSize);
            icons[TOOL_BAR_CONTROL_MENU - TOOL_BAR_CONTROL_PLACE_HOLDER] = wxArtProvider::GetIcon(wxART_FOLDER, wxART_OTHER, iconSize);

            for ( size_t i = 0; i < WXSIZEOF(icons); i++ )
            {
                int sizeOrig = icons[0].GetWidth();
                if ( 16 == sizeOrig )
                {
                    images->Add(icons[i]);
                }
                else
                {
                    images->Add(wxBitmap(wxBitmap(icons[i]).ConvertToImage().Rescale(16, 16)));
                }
            }
            m_WindowTreeControl->AssignImageList(images);
		}

		wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);

		topSizer->Add(m_WindowTreeControl, 1, wxEXPAND | wxALL, 2);

		wxBoxSizer *bottomSizer = new wxBoxSizer(wxHORIZONTAL);
		wxButton *buttonUp = new wxButton(m_WindowTreeControlPanel, IDC_WINDOW_TREE_BUTTON_UP, "Up");
		wxButton *buttonDown = new wxButton(m_WindowTreeControlPanel, IDC_WINDOW_TREE_BUTTON_DOWN, "Down");
		wxButton *buttonDel = new wxButton(m_WindowTreeControlPanel, IDC_WINDOW_TREE_BUTTON_DEL, "Del");

		bottomSizer->Add(buttonUp, 0, wxEXPAND | wxALL, 2);
		bottomSizer->Add(buttonDown, 0, wxEXPAND | wxALL, 2);
		bottomSizer->Add(buttonDel, 0, wxEXPAND | wxALL, 2);

		topSizer->Add(bottomSizer, 0, wxEXPAND | wxALL, 2);
		m_WindowTreeControlPanel->SetSizer(topSizer);
	}
}

void MainFrame::CreatePropGrid(void)
{
	if (m_PropGridManager == nullptr)
	{
		m_PropGridManager = new wxPropertyGridManager(this, IDC_PROPERTY_GRID, wxDefaultPosition, wxDefaultSize, wxPG_BOLD_MODIFIED|wxPG_TOOLTIPS);
	}
}

void MainFrame::CreateCanvasWindow(void)
{
	if (m_CanvasWindow == nullptr)
	{
		Project *project = Manager::Get()->GetActiveProject();
		m_CanvasWindow = new CanvasWindow(this, IDC_MAIN_CANVAS, project ? project->GetCanvasSize() : wxSize(1280, 720));
	}
}

void MainFrame::CreatePageListWindow(void)
{
	if (m_PageList == nullptr)
	{
		m_PageList = new PageListWindow(this, IDC_PAGE_LIST);
	}
}

void MainFrame::CreateOutputWindow(void)
{
	if (m_OutputTextCtrl == nullptr)
	{
		m_OutputTextCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY);
		delete wxLog::SetActiveTarget(new wxLogTextCtrl(m_OutputTextCtrl));
	}
}

void MainFrame::CreateThemeTreeWindow(void)
{
	if (m_ThemeTree == nullptr)
	{
		m_ThemeTree = new wxTreeCtrl(this, IDC_THEME_TREE, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxNO_BORDER);
		m_ThemeTree->AddRoot(wxT("Themes"));
	}
}

void MainFrame::CreateThemeComponentTreeWindow(void)
{
	if (m_ThemeComponentTree == nullptr)
	{
		m_ThemeComponentTree = new wxTreeCtrl(this, IDC_THEME_COMPONENT_TREE, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxNO_BORDER);
	}
}


//-------------------------------------------------------------------------------------------------
// layout window
//-------------------------------------------------------------------------------------------------
void MainFrame::LayoutWindow(void)
{
	m_LayoutManager.AddPane(m_ControlsToolBar, wxAuiPaneInfo()
			.Name("toolbar_control")
			.ToolbarPane()
			.Top());

	m_LayoutManager.AddPane(m_CanvasWindow, wxAuiPaneInfo()
			.Name("canvas")
			.CenterPane());

	m_LayoutManager.AddPane(m_PageList, wxAuiPaneInfo()
			.Name("page_list")
			.Caption(wxT("页面列表"))
			.Left()
			.BestSize(FromDIP(wxSize(300, 200)))
			.MinSize(FromDIP(wxSize(200, 200))));

	m_LayoutManager.AddPane(m_WindowTreeControlPanel, wxAuiPaneInfo()
			.Name("window_tree")
			.Caption(wxT("控件树"))
			.Right().Layer(0).Position(0)
			.BestSize(FromDIP(wxSize(400, 200)))
			.MinSize(FromDIP(wxSize(300, 200))));

	m_LayoutManager.AddPane(m_PropGridManager, wxAuiPaneInfo()
			.Name("prop_grid")
			.Caption(wxT("属性"))
			.Right().Layer(0).Position(1)
			.BestSize(FromDIP(wxSize(400, 200)))
			.MinSize(FromDIP(wxSize(300, 200))));

	m_LayoutManager.AddPane(m_OutputTextCtrl, wxAuiPaneInfo()
			.Name("output")
			.Caption(wxT("输出"))
			.Bottom()
			.MinSize(FromDIP(wxSize(100, 100))));

	m_AuiMgrNormalPerspective = m_LayoutManager.SavePerspective();


	m_LayoutManager.GetPane("toolbar_control").Hide();
	m_LayoutManager.AddPane(m_ComponentsToolBar, wxAuiPaneInfo()
			.Name("toolbar_components")
			.ToolbarPane()
			.Top());

	m_LayoutManager.GetPane("page_list").Hide();
	m_LayoutManager.AddPane(m_ThemeTree, wxAuiPaneInfo()
			.Name("theme_tree")
			.Caption(wxT("主题风格"))
			.Left()
			.BestSize(FromDIP(wxSize(300, 200)))
			.MinSize(FromDIP(wxSize(200, 200))));
	m_LayoutManager.GetPane("window_tree").Hide();
	m_LayoutManager.AddPane(m_ThemeComponentTree, wxAuiPaneInfo()
			.Name("component_tree")
			.Caption(wxT("组件"))
			.Right().Layer(0).Position(0)
			.BestSize(FromDIP(wxSize(400, 200)))
			.MinSize(FromDIP(wxSize(300, 200))));

	m_AuiMgrEditThemePerspective = m_LayoutManager.SavePerspective();

	SwitchEditorMode();
}


void MainFrame::InitializeRecentFilesHistory()
{
	m_projectsHistory.Initialize();
}

void MainFrame::TerminateRecentFilesHistory()
{
	m_projectsHistory.TerminateHistory();
}

void MainFrame::InitWindowView()
{
	m_ActiveConfigManger->ReadWindowView("Window_View");
	UpdateWindowView();
}

//-------------------------------------------------------------------------------------------------
// private - internal
//-------------------------------------------------------------------------------------------------
void MainFrame::SwitchEditorMode(void)
{
	m_LayoutManager.LoadPerspective(m_EditThemeMode ? m_AuiMgrEditThemePerspective : m_AuiMgrNormalPerspective);
	m_LayoutManager.Update();
}

void MainFrame::SetActivePage(unsigned int index)
{
	Project *proj = Manager::Get()->GetActiveProject();

	if (proj)
	{
		m_ActivePage = proj->GetPageFileAt(index);
		proj->SetActivePageFile(m_ActivePage);
	}

	if (m_ActivePage )
	{
		m_ActiveWindow = m_ActiveRootWindow = m_ActivePage->GetRootWindow();
		m_CanvasWindow->SetRootWindow(m_ActiveRootWindow);
		m_CanvasWindow->ReloadData();
		UpdateTreeControl();
		UpdatePropertyGrid();
	}
}

void MainFrame::AppendWindowTreeItemRecursively(wxTreeItemId& parentId, uiWindow *window)
{
	if (window == nullptr) return;

    int image_index = window->GetWindowIconIndex();
//    wxLogMessage("%s---%d [%d]", window->Id(),image_index, m_WindowTreeControl->GetImageList()->GetImageCount());
	// append current
	wxTreeItemId itemId = m_WindowTreeControl->AppendItem(parentId, window->Id(), image_index);
	window->m_TreeId = itemId;
	// append child
	uiWindow *child = window->m_Childs;
	while (child)
	{
		AppendWindowTreeItemRecursively(itemId, child);
		child = child->m_Next;
	}
}

void MainFrame::UpdateTreeControl(void)
{
	m_WindowTreeControl->DeleteAllItems();

	if (m_ActiveRootWindow == nullptr)
	{
		return;
	}

    int image_index = m_ActiveRootWindow->GetWindowIconIndex();

	wxTreeItemId root = m_WindowTreeControl->AddRoot(m_ActiveRootWindow->Id(),image_index);
	m_ActiveRootWindow->m_TreeId = root;
	// add all sub items
	uiWindow *child = m_ActiveRootWindow->m_Childs;
	while (child)
	{
		AppendWindowTreeItemRecursively(root, child);
		child = child->m_Next;
	}
	m_WindowTreeControl->SelectItem(m_ActiveWindow->m_TreeId);
	m_WindowTreeControl->ExpandAll();
}

wxPGProperty* MainFrame::AddPropertyGrid(PropertyBase *prop, wxString key)
{
	wxPGProperty *property = nullptr;
	if (prop->m_Type == PropertyType_Integer)
	{
		PropertyInteger *intProperty = (PropertyInteger *)prop;
		property = new wxIntProperty(key, wxPG_LABEL, intProperty->IntValue());
	}
	else if (prop->m_Type == PropertyType_Bool)
	{
		PropertyBool *BoolProperty = (PropertyBool *)prop;
		property = new wxBoolProperty (key, wxPG_LABEL, BoolProperty->BoolValue());
	}
	else if (prop->m_Type == PropertyType_Rect)
	{
		PropertyRect *rectProperty = (PropertyRect *)prop;
		property = new RectPGProperty(key, wxPG_LABEL, rectProperty->Value(), \
			rectProperty->string_x, rectProperty->string_y, rectProperty->string_width, rectProperty->string_height);
	}
	else if (prop->m_Type == PropertyType_Color)
	{
		PropertyColor *colorProperty = (PropertyColor *)prop;
		long index = Colors::Get()->GetColorId(colorProperty->Name());
		wxColour value = colorProperty->Value();

		property = new ColorPGProperty(key, wxPG_LABEL, wxColourPropertyValue(index, value));
	}
	else if (prop->m_Type == PropertyType_Image)
	{
		PropertyImage *imageProperty = (PropertyImage *)prop;
		Project *proj = Manager::Get()->GetActiveProject();
		wxArrayString names;
		wxArrayInt values;
		long value = 0xFFFFFF;

		if (proj)
		{
			names = proj->m_Images.GetNames();
			values = proj->m_Images.GetValues();
			value = proj->m_Images.FindImage(imageProperty->Value());
		}
		if (value == 0xFFFFFF)
		{
			names.Add(imageProperty->Value() + " (!)");
			values.Add(value);
		}
		property = new wxEnumProperty(key, wxPG_LABEL, names, values, value);
	}
	else if (prop->m_Type == PropertyType_Logo)
	{
		PropertyLogo *logoProperty = (PropertyLogo *)prop;
		Project *proj = Manager::Get()->GetActiveProject();
		wxArrayString names;
		wxArrayInt values;
		long value = 0xFFFFFF;

		if (proj)
		{
			names = proj->m_LogoImage.GetNames();
			values = proj->m_LogoImage.GetValues();
			value = proj->m_LogoImage.FindLogo(logoProperty->Value());
		}
		if (value == 0xFFFFFF)
		{
			names.Add(logoProperty->Value() + " (!)");
			values.Add(value);
		}
		names.Add("(!)");
		values.Add(-1);
		property = new wxEnumProperty(key, wxPG_LABEL, names, values, value);
	}
	else if (prop->m_Type == PropertyType_InternalString)
	{
		PropertyInternalString *StringProperty = (PropertyInternalString *)prop;
		int i = 0;
		wxArrayString names;
		wxArrayInt values;
		wxString value;
		if (!StringManager::Get()->m_Stringpaths.IsEmpty())
		{
			for (i = 0; i < str_max_strings; i++)
			{
				names.Add(StringManager::Get()->GetString(i, false));
				values.Add(i);
			}
			value = StringProperty->toString();
			value = dsC2U(StringManager::Get()->SetIDStringByRealString((char *)dsU2C(value)));
			property = new wxEditEnumProperty(key, wxPG_LABEL, names, values, value);
		}
		else
		{
			property = new wxStringProperty(key, wxPG_LABEL, prop->toString());
		}
	}
	else if (prop->m_Type == PropertyType_Fold)
	{
		property = new wxPropertyCategory(prop->toString());
	}
	else if (prop->m_Type == PropertyType_Size)
	{
		PropertySize *sizeProperty = (PropertySize *)prop;
		property = new SizeProperty(key, wxPG_LABEL, sizeProperty->Value(), sizeProperty->m_WidthString, sizeProperty->m_HeightString);
	}
	else if (prop->m_Type == PropertyType_Enum)
	{
		wxPGChoices choice;
		choice = m_ActiveWindow->AddEnmuChildProperty(key);
		if (!choice.IsOk())
		{
			wxString message = "You have not add any enum value for ";
			message += key;
			wxMessageBox(message);
		}
		else
		{
			PropertyEnum *enumProperty = (PropertyEnum *)prop;
			property = new wxEnumProperty(key, wxPG_LABEL, choice, enumProperty->EnumValue());
		}
	}
	else	// default is string property
	{
		if (prop->toString().IsSameAs(""))
		{
			wxLogMessage("same as");
			property = new wxStringProperty(key, wxPG_LABEL, "<composed>");
		}
		else
			property = new wxStringProperty(key, wxPG_LABEL, prop->toString());
	}
	return property;
}

void MainFrame::UpdatePropertyGrid(void)
{
	m_PropGridManager->Clear();
	m_PropGridManager->AddPage("Standard Items");

	wxPropertyGridPage *page = m_PropGridManager->GetPage(0);

	if (m_ActiveWindow == nullptr) return;

	Linked_map<wxString, PropertyBase*>::iterator it;
	m_ActiveWindow->AddUpdateChildProperty();
	for (it = m_ActiveWindow->m_Properties.begin(); it != m_ActiveWindow->m_Properties.end(); ++it)
	{
		wxString key = it->first;
		PropertyBase *prop = it->second;
		wxPGProperty *property = nullptr;

		char message[128];
		sprintf(message, "key <%s> have no property value!", dsU2C(key));
		wxASSERT_MSG(prop != nullptr, message);
		property = AddPropertyGrid(prop, key);

		if (property)
		{
			property->Enable(prop->m_Enable);
			property->Hide(!prop->m_Visiable);
			if (prop->m_Type == PropertyType_Fold)
			{
				PropertyFold *foldProperty = (PropertyFold *)prop;
				if (!foldProperty->parent.IsEmpty())
				{
					wxPGProperty* parent = page->GetPropertyByName(foldProperty->parent);
					page->AppendIn(parent, property);
				}
				else
					page->Append(property);
			}
			else
				page->Append(property);
		}
	}
}

void MainFrame::RefreshProperty(void)
{
	if (m_PropGridManager)
	{
		Linked_map<wxString, PropertyBase*>::iterator it;
		wxPropertyGridPage *page = m_PropGridManager->GetPage(0);
		for (it = m_ActiveWindow->m_Properties.begin(); it != m_ActiveWindow->m_Properties.end(); it++)
		{
			wxString key = it->first;
			PropertyBase *prop = it->second;
			wxPGProperty *property = page->GetPropertyByName(key);
//			wxLogMessage(key);
			if (property)
			{
				property->Enable(prop->m_Enable);
				property->Hide(!prop->m_Visiable);
			}
		}
	}
}


//-------------------------------------------------------------------------------------------------
// private - theme
//-------------------------------------------------------------------------------------------------
void MainFrame::CreateNewThemeFor(wxString controlName, wxString themeName)
{
	if (m_ThemeTree == nullptr)
		return;

	wxTreeItemIdValue cookie;
	wxTreeItemId parent = m_ThemeTree->GetRootItem();
	wxTreeItemId item;
	bool find = false;
	for (item = m_ThemeTree->GetFirstChild(parent, cookie); item.IsOk(); item = m_ThemeTree->GetNextChild(parent, cookie))
	{
		if (m_ThemeTree->GetItemText(item) == controlName)
		{
			find = true;
			break;
		}
	}
	if (!find)
	{
		item = m_ThemeTree->AppendItem(parent, controlName);
	}

	m_ThemeTree->SelectItem(m_ThemeTree->AppendItem(item, themeName), true);
}


//-------------------------------------------------------------------------------------------------
// private - event handler
//-------------------------------------------------------------------------------------------------
void MainFrame::OnPageListSelected(wxCommandEvent& event)
{
	SetActivePage(event.GetSelection());
}

void MainFrame::OnCanvasClicked(CanvasEvent& event)
{
	uiWindow *window = nullptr;
	wxString Name;
	wxString class_id;
	wxTreeItemIdValue cookie;
	wxTreeItemId parent = m_WindowTreeControl->GetRootItem();
	wxTreeItemId item, brother_item;
	int select = -1;
	for (int i = TOOL_BAR_CONTROL_LABEL; i < TOOL_BAR_CONTROL_MAX; i++)
	{
		if (m_ControlsToolBar->GetToolToggled(i))
		{
			select = i;
			wxLogMessage("canvas clicked select=%d", i);
			break;
		}
	}

    uiWindow* parent_win = m_CanvasWindow->GetSubOrMainWindow();
    if (parent_win == nullptr) parent_win = m_ActiveRootWindow;
    wxLogMessage("parent_win = %s", parent_win->Id());
	if (parent_win && select >= 0)
	{
		if (select == TOOL_BAR_CONTROL_LABEL)
		{
		    window = new uiLabel();
		    class_id =  "Label";
		}
		else if (select == TOOL_BAR_CONTROL_IMAGE)
		{
			window = new uiImage();
			class_id =  "Image";
		}
		else if (select == TOOL_BAR_CONTROL_LIST)
		{
			window = new uiList();
			class_id =  "List";
		}
		else if (select == TOOL_BAR_CONTROL_LINE_CHART)
		{
			window = new uiLineChart();
			class_id =  "LineChart";
		}
		else if (select == TOOL_BAR_CONTROL_PROMPT)
		{
			window = new uiPrompt();
			class_id = "Prompt";
		}
		else if (select == TOOL_BAR_CONTROL_MESSAGE_BOX)
		{
			window = new uiMessageBox();
			class_id = "Message_Box";
		}
		else if (select == TOOL_BAR_CONTROL_TEXT_FIELD)
		{
			window = new uiTextField();
			class_id = "Text_Field";
		}
		else if (select == TOOL_BAR_CONTROL_EDIT)
		{
			window = new uiEdit();
			class_id =  "Edit";
		}
		else if (select == TOOL_BAR_CONTROL_EDIT_ITEM)
		{
			if ((parent_win->GetClassName().IsSameAs(wxT("UIEdit"))))
			{
				window = new uiEditItem();
				class_id =  "Edit_Item";
			}
			else
			{
				wxMessageBox("You must create it under the edit control");
			}
		}
		else if (select == TOOL_BAR_CONTROL_MENU)
		{
			window = new uiMenu();
			class_id =  "Menu";
		}
		else if (select == TOOL_BAR_CONTROL_MENU_ITEM)
		{
			if ((parent_win->GetClassName().IsSameAs(wxT("UIBoxSizer")))||(parent_win->GetClassName().IsSameAs(wxT("UIListSizer"))))
			{
				window = new uiMenuItem();
				class_id =  "Menu_Item";
			}
			else
			{
				wxMessageBox("You must create it under the menu control");
			}
		}
		else if (select == TOOL_BAR_CONTROL_LIST_SIZER)
		{
			window = new uiListSizer();
			class_id =  "ListSizer";
		}
		else if (select == TOOL_BAR_CONTROL_BOX_SIZER)
		{
			window = new uiBoxSizer();
			class_id =  "BoxSizer";
		}

		if (window)
		{
again:
			Name = m_ActivePage->GetCoderContext()->GetUniqueName(class_id);
			for (item = m_WindowTreeControl->GetFirstChild(parent, cookie); item.IsOk(); item = m_WindowTreeControl->GetNextVisible(item))
			{
				if (m_WindowTreeControl->GetItemText(item) == Name)
				{
					goto again;
				}
			}
			window->SetStringProperty(PROP_NAME,	(char *)dsU2C(Name));
			parent_win->AddSubWindow(window);
			m_ActiveWindow = window;
		}
		else
		{
			m_ControlsToolBar->ToggleTool(TOOL_BAR_CONTROL_PLACE_HOLDER, true);
			m_ControlsToolBar->Refresh();
			return;
		}
		m_ActivePage->MarkAsModified();
		m_CanvasWindow->ReloadData();
		UpdateTreeControl();
		UpdatePropertyGrid();

		m_ControlsToolBar->ToggleTool(TOOL_BAR_CONTROL_PLACE_HOLDER, true);
		m_ControlsToolBar->Refresh();
	}
}

void MainFrame::OnCanvasChanged(CanvasEvent& event)
{
    m_ActivePage->MarkAsModified();
	UpdatePropertyGrid();
	m_CanvasWindow->ReloadData();
	m_CanvasWindow->RefreshSelection();
	m_PageList->Refresh();
}

void MainFrame::OnCanvasWinRectChange(CanvasEvent& event)
{
    wxRect rect = m_CanvasWindow->GetDragingWindowRect();
    if(m_ActiveWindow)
    {
         m_ActiveWindow->SetProperty(PROP_POSITION, rect);
         UpdatePropertyGrid();
    }
}

void MainFrame::OnCanvasCancelDrag(CanvasEvent& event)
{
    m_ControlsToolBar->ToggleTool(TOOL_BAR_CONTROL_PLACE_HOLDER, true);
	m_ControlsToolBar->Refresh();
}


void MainFrame::OnPropertyGridChange(wxPropertyGridEvent& event)
{
	wxPGProperty* property = event.GetProperty();
	const wxString& name = property->GetName();
	wxAny value = property->GetValue();
	wxString class_name = m_ActiveWindow->GetClassName();

	m_ActiveWindow->SetProperty(name, value);
	m_ActivePage->MarkAsModified();
	m_CanvasWindow->ReloadData();
	m_CanvasWindow->RefreshSelection();
	m_PageList->Refresh();
	/* tree node text must sync with window id all the time */
	if (name == PROP_NAME)
	{
		wxTreeItemId itemId = m_WindowTreeControl->GetSelection();
		if (itemId.IsOk())
		{
			m_WindowTreeControl->SetItemText(itemId, value.As<wxString>());
		}
		else	/* error, selection of tree control lost, need refresh tree and prop grid */
		{
			UpdateTreeControl();
			UpdatePropertyGrid();
		}
	}
	else if (class_name.IsSameAs("UILineChart"))
	{
		UpdatePropertyGrid();
	}
	RefreshProperty();
}

void MainFrame::OnWindowTreeSelectChanged(wxTreeEvent& event)
{
	wxTreeItemId itemId = event.GetItem();
	if (!itemId.IsOk()) return;

	wxString text = m_WindowTreeControl->GetItemText(itemId);
	//wxLogMessage("OnWindowTreeSelectChanged %s", text);

	uiWindow *window = m_ActiveRootWindow->FindInSubWindow(text);
	if (window)
	{
		m_ActiveWindow = window;
		UpdatePropertyGrid();

		m_CanvasWindow->SetActiveWindow(window);
		m_CanvasWindow->RefreshSelection();
	}
}

void MainFrame::OnWindowTreeItemUp(wxCommandEvent& event)
{
	wxTreeItemId upItemId = m_WindowTreeControl->GetSelection();
	if (!upItemId.IsOk())
	{
		wxLogMessage("item is invalid ");
		return;
	}
	if(m_WindowTreeControl->GetItemText(upItemId).c_str() == m_ActiveRootWindow->Id())
	{
		wxLogMessage("can not move(up) root");
		return;
	}
	m_ActiveRootWindow->MoveUpSubWindow(m_ActiveWindow);
	//wxLogMessage("up select %s %s", m_ActiveWindow->Id(), m_WindowTreeControl->GetItemText(upItemId).c_str());

	wxTreeItemId prevItemId = m_WindowTreeControl->GetPrevSibling(upItemId);
	if( !prevItemId.IsOk() )
	{
		wxLogMessage("item is header ");
		return;
	}
	wxString upText = m_WindowTreeControl->GetItemText(upItemId);
	wxTreeItemId parentItemId = m_WindowTreeControl->GetItemParent(upItemId);
	wxString insertText = m_WindowTreeControl->GetItemText(prevItemId);
	int image_index = m_WindowTreeControl->GetItemImage(prevItemId);
	m_WindowTreeControl->Delete(prevItemId);
//	wxLogMessage("up delete %s - %d", insertText, image_index);
	m_WindowTreeControl->InsertItem(parentItemId, upItemId, insertText, image_index);

	wxTreeItemId insertItemId = m_WindowTreeControl->GetNextSibling(upItemId);
	uiWindow *insertWindow = m_ActiveRootWindow->FindInSubWindow(insertText);
	if(insertWindow->m_Childs)
	{
		// add sub items
		uiWindow *upChild = insertWindow->m_Childs;
		while (upChild)
		{
			AppendWindowTreeItemRecursively(insertItemId, upChild);
			upChild = upChild->m_Next;
		}
	}

	m_WindowTreeControl->SelectItem(upItemId);
	m_WindowTreeControl->SetItemDropHighlight(upItemId, true);
	m_WindowTreeControl->SetFocusedItem(upItemId);

	m_ActivePage->MarkAsModified();
	m_CanvasWindow->ReloadData();
	m_CanvasWindow->RefreshSelection();
	m_PageList->Refresh();
}

void MainFrame::OnWindowTreeItemDown(wxCommandEvent& event)
{
	wxTreeItemId downItemId = m_WindowTreeControl->GetSelection();
	if (!downItemId.IsOk())
	{
		wxLogMessage("item is invalid ");
		return;
	}

	if(m_WindowTreeControl->GetItemText(downItemId).c_str() == m_ActiveRootWindow->Id())
	{
		wxLogMessage("can not move(down) root");
		return;
	}
	m_ActiveRootWindow->MoveDownSubWindow(m_ActiveWindow);
	//wxLogMessage("down select %s %s", m_ActiveWindow->Id(), m_WindowTreeControl->GetItemText(downItemId).c_str());

	wxTreeItemId nextItemId = m_WindowTreeControl->GetNextSibling(downItemId);
	if( !nextItemId.IsOk() )
	{
		wxLogMessage("item is last ");
		return;
	}
	wxString downText = m_WindowTreeControl->GetItemText(downItemId);
	wxString insertText = m_WindowTreeControl->GetItemText(nextItemId);
	wxTreeItemId parentItemId = m_WindowTreeControl->GetItemParent(downItemId);
	wxTreeItemId prevItemId = m_WindowTreeControl->GetPrevSibling(downItemId);
	int image_index = m_WindowTreeControl->GetItemImage(nextItemId);
//	wxLogMessage("down delete %s - %d", insertText, image_index);
	m_WindowTreeControl->Delete(nextItemId);

	if( !prevItemId.IsOk() )
	{
		m_WindowTreeControl->InsertItem(parentItemId, 0, insertText, image_index);
	}
	else
	{
		m_WindowTreeControl->InsertItem(parentItemId, prevItemId, insertText, image_index);
	}

	wxTreeItemId insertItemId = m_WindowTreeControl->GetPrevSibling(downItemId);
	uiWindow *insertWindow = m_ActiveRootWindow->FindInSubWindow(insertText);
	if(insertWindow->m_Childs)
	{
		// add sub items
		uiWindow *downChild = insertWindow->m_Childs;
		while (downChild)
		{
			AppendWindowTreeItemRecursively(insertItemId, downChild);
			downChild = downChild->m_Next;
		}
	}

	m_WindowTreeControl->SelectItem(downItemId);
	m_WindowTreeControl->SetItemDropHighlight(downItemId, true);
	m_WindowTreeControl->SetFocusedItem(downItemId);

	m_ActivePage->MarkAsModified();
	m_CanvasWindow->ReloadData();
	m_CanvasWindow->RefreshSelection();
	m_PageList->Refresh();
}

void MainFrame::OnWindowTreeItemDelete(wxCommandEvent& event)
{
	wxTreeItemId itemId = m_WindowTreeControl->GetSelection();
	if (!itemId.IsOk())
	{
		wxLogMessage("item is invalid ");
		return;
	}

	//wxLogMessage("Delete %s", m_WindowTreeControl->GetItemText(itemId).c_str());

	if(m_WindowTreeControl->GetItemText(itemId).c_str() == m_ActiveRootWindow->Id())
	{
		wxLogMessage("can not delete root");
		return;
	}

	m_ActiveRootWindow->DeleteSubWindow(m_ActiveWindow);
	m_ActivePage->MarkAsModified();
	m_CanvasWindow->ReloadData();
	m_PageList->Refresh();
	m_WindowTreeControl->Delete(itemId);
}


void MainFrame::OnUpdateEditUI(wxUpdateUIEvent& event)
{
	int EditId = event.GetId();

	if (m_ActivePage == nullptr)
	{
		event.Enable(false);
		return;
	}

	switch(EditId)
	{
	case IDM_EDIT_UNDO:
		if (m_ActivePage->CanUndo())
		{
			event.Enable(true);
		}
		else
		{
			event.Enable(false);
		}
		break;
	case IDM_EDIT_REDO:
		if (m_ActivePage->CanRedo())
		{
			event.Enable(true);
		}
		else
		{
			event.Enable(false);
		}
		break;
	case IDM_EDIT_CUT:
		event.Enable(true);
		break;
	case IDM_EDIT_COPY:
		event.Enable(true);
		break;
	case IDM_EDIT_PASTE:
	    if(CanPaste())
        {
            event.Enable(true);
        }
        else
        {
            event.Enable(false);
        }
		break;
	default:
		break;
	}
}

void MainFrame::OnUpdateWindowTreeButtonUI(wxUpdateUIEvent& event)
{
	int buttonId = event.GetId();
	wxTreeItemId itemId = m_WindowTreeControl->GetSelection();

	if ( buttonId == IDC_WINDOW_TREE_BUTTON_UP )
	{
		if((m_WindowTreeControl->GetCount() > 0 ) && (m_WindowTreeControl->GetPrevSibling(itemId)))
		{
			event.Enable(true);
		}
		else
		{
			event.Enable(false);
		}
	}
	else if ( buttonId == IDC_WINDOW_TREE_BUTTON_DOWN )
	{
		if((m_WindowTreeControl->GetCount() > 0 ) && (m_WindowTreeControl->GetNextSibling(itemId)))
		{
			event.Enable(true);
		}
		else
		{
			event.Enable(false);
		}
	}
	else if( buttonId == IDC_WINDOW_TREE_BUTTON_DEL )
	{
		if((m_WindowTreeControl->GetCount() > 0 ) && (m_WindowTreeControl->GetRootItem() != itemId))
		{
			event.Enable(true);
		}
		else
		{
			event.Enable(false);
		}
	}
}

void MainFrame::OnUpdateMenuProjItemUI(wxUpdateUIEvent& event)
{
//    int buttonId = event.GetId();
	Project *proj = Manager::Get()->GetActiveProject();

	if (!proj)
	{
		event.Enable(false);
	}
	else
	{
		event.Enable(true);
	}
}

void MainFrame::OnUpdateMenuFileItemUI(wxUpdateUIEvent& event)
{
	int buttonId = event.GetId();

	Project *proj = Manager::Get()->GetActiveProject();

	if (!proj)
	{
		event.Enable(false);
	}
	else
	{
		event.Enable(true);
		if (buttonId == IDM_FILE_SAVE)
		{
			if (!proj->GetModified())
			{
				event.Enable(false);
			}
		}
	}
}

void MainFrame::OnUpdateProjectLanguage(wxUpdateUIEvent& event)
{
	Project *proj = Manager::Get()->GetActiveProject();
	if (proj)
	{
		int which = (event.GetId() - IDM_LANGUAGE_BEGIN);
		if (which == StringManager::Get()->GetSaveLanguageIndex())
			event.Check(true);
		else
			event.Check(false);
	}
}



void MainFrame::OnFileReopenProject(wxCommandEvent& event)
{
	size_t id = event.GetId() - wxID_CBFILE17;
	wxString fname = m_projectsHistory.GetHistoryFile(id);
//	if (!OpenGeneric(fname, true))
//		m_projectsHistory.AskToRemoveFileFromHistory(id);

	Project *currentPrj = Manager::Get()->GetActiveProject();
	if ( currentPrj != nullptr)
	{
		if (fname == currentPrj->GetFileName()) return;

		if (!SaveOrDiscardProjectChange()) return;
	}

	DoOpenProject(fname, true);
}

void MainFrame::OnFileOpenRecentProjectClearHistory(wxCommandEvent& event)
{
	m_projectsHistory.ClearHistory();
}


void MainFrame::OnAddWindowRequest(wxCommandEvent& event)
{
    if (!m_ActivePage)
    {
        m_ControlsToolBar->ToggleTool(TOOL_BAR_CONTROL_PLACE_HOLDER, true);
        m_ControlsToolBar->Refresh();
        return;
    }

    m_CanvasWindow->InsertByPointing();
}

void MainFrame::UpdateWindowView(void)
{
	if (!m_ActiveConfigManger->PageListView)
	{
		m_LayoutManager.GetPane("page_list").Hide();
	}
	if (!m_ActiveConfigManger->WindowTreeView)
	{
		m_LayoutManager.GetPane("window_tree").Hide();
	}
	if (!m_ActiveConfigManger->PropGridView)
	{
		m_LayoutManager.GetPane("prop_grid").Hide();
	}
	if (!m_ActiveConfigManger->OutputView)
	{
		m_LayoutManager.GetPane("output").Hide();
	}
	m_LayoutManager.Update();
}

void MainFrame::OnWindowViewClose(wxAuiManagerEvent& event)
{
	if (event.GetPane()->name.IsSameAs("page_list"))
	{
		m_ActiveConfigManger->PageListView = 0;
	}
	if (event.GetPane()->name.IsSameAs("window_tree"))
	{
		m_ActiveConfigManger->WindowTreeView = 0;
	}
	if (event.GetPane()->name.IsSameAs("prop_grid"))
	{
		m_ActiveConfigManger->PropGridView = 0;
	}
	if (event.GetPane()->name.IsSameAs("output"))
	{
		m_ActiveConfigManger->OutputView = 0;
	}
}

void MainFrame::OnUpdateViewWindowItemUI(wxUpdateUIEvent& event)
{
	int id = event.GetId() - IDM_VIEW_PAGE;
	event.Check(false);
	if (m_ActiveConfigManger->PageListView && id == 0)
	{
		event.Check(true);
	}
	else if (m_ActiveConfigManger->WindowTreeView && id == 1)
	{
		event.Check(true);
	}
	else if (m_ActiveConfigManger->PropGridView && id == 2)
	{
		event.Check(true);
	}
	else if (m_ActiveConfigManger->OutputView && id == 3)
	{
		event.Check(true);
	}
}

void MainFrame::OnWindowView(wxCommandEvent& event)
{
	int id = event.GetId() - IDM_VIEW_PAGE;

	if (id == 0)
	{
		m_ActiveConfigManger->PageListView = !m_ActiveConfigManger->PageListView;
		if (m_ActiveConfigManger->PageListView)
			m_LayoutManager.GetPane("page_list").Show();
		else
			m_LayoutManager.GetPane("page_list").Hide();

	}
	else if (id == 1)
	{
		m_ActiveConfigManger->WindowTreeView = !m_ActiveConfigManger->WindowTreeView;
		if (m_ActiveConfigManger->WindowTreeView)
			m_LayoutManager.GetPane("window_tree").Show();
		else
			m_LayoutManager.GetPane("window_tree").Hide();
	}
	else if (id == 2)
	{
		m_ActiveConfigManger->PropGridView = !m_ActiveConfigManger->PropGridView;
		if (m_ActiveConfigManger->PropGridView)
			m_LayoutManager.GetPane("prop_grid").Show();
		else
			m_LayoutManager.GetPane("prop_grid").Hide();
	}
	else if (id == 3)
	{
		m_ActiveConfigManger->OutputView = !m_ActiveConfigManger->OutputView;
		if (m_ActiveConfigManger->OutputView)
			m_LayoutManager.GetPane("output").Show();
		else
			m_LayoutManager.GetPane("output").Hide();
	}
	m_LayoutManager.Update();
}


//-------------------------------------------------------------------------------------------------
// private - project
//-------------------------------------------------------------------------------------------------
bool MainFrame::SaveOrDiscardProjectChange(void)
{
	/* do nothing if no project opened or haven't been modified */
	if (!(Manager::Get()->GetActiveProject()) || !(Manager::Get()->GetActiveProject()->GetModified()))
		return true;

	wxString message(wxT("是否将更改保存到 "));
	message += Manager::Get()->GetActiveProject()->GetName() + wxT("?");

	wxMessageDialog askSave(this, message, APP_NAME, wxYES_NO|wxCANCEL|wxYES_DEFAULT|wxICON_QUESTION);
	askSave.SetYesNoCancelLabels(wxT("保存"), wxT("不保存"), wxT("取消"));

	int action = askSave.ShowModal();

	if (action == wxID_CANCEL) return false;

	if (action == wxID_YES)
	{
		Manager::Get()->GetActiveProject()->Save();
	}

    return true;
}


void MainFrame::UpdateAppTitle(void)
{
	wxString title = wxString(APP_NAME_WITH_VERSION);

	if (Manager::Get()->GetActiveProject())
		title += " - " + Manager::Get()->GetActiveProject()->GetNameOnTitle();

	this->SetTitle(title);

	if (m_PageList)
		m_PageList->Refresh();
}

//void MainFrame::UpdatePageList(void)
//{
//	if (m_PageList)
//		m_PageList->Refresh();
//}


void MainFrame::DoOpenProject(const wxString& filename, bool addToHistory)
{
	Project *newProj = new Project(filename);
	Manager::Get()->SetActiveProject(newProj);
	uiRender::SetCanvasProperty(newProj->GetCanvasSize(), newProj->GetColorMode());
	m_CanvasWindow->SetCanvasSize(newProj->GetCanvasSize());
	UpdateAppTitle();

	if (addToHistory)
	{
		m_projectsHistory.AddToHistory(newProj->GetFileName());
	}

	m_PageList->ReloadAllData();
	m_PageList->SelectFirst();
	SetActivePage(m_PageList->GetSelection());
}

//-------------------------------------------------------------------------------------------------
// private - PageListWindow
//-------------------------------------------------------------------------------------------------
void MainFrame::OnPageDelete(PageListEvent& event)
{
	if (Manager::Get()->GetActiveProject())
	{
		PageFile *pagefile = Manager::Get()->GetActiveProject()->GetActivePageFile();
		if (pagefile)
		{
			Manager::Get()->GetActiveProject()->DeletePageFile(pagefile);
			m_PageList->SelectFirst();
			m_PageList->ReloadAllData();
			if (Manager::Get()->GetActiveProject()->GetPageFileCount() > 0)
			{
				SetActivePage(0);
			}
			else
			{
				m_ActiveWindow = m_ActiveRootWindow = nullptr;
				m_CanvasWindow->SetRootWindow(m_ActiveRootWindow);
				m_CanvasWindow->ReloadData();
				UpdateTreeControl();
				UpdatePropertyGrid();
			}
			m_ActivePage->MarkAsModified();
		}
	}
}
