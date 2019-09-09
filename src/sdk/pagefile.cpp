/*
 * pagefile.cpp
 *
 *  Created on: 2019-4-15
 *      Author: Brant
 */

#include "sdk_common.h"
//#include "main.h"
#include "pagefile.h"
#include "window/uipage.h"
#include "window/uilabel.h"
#include "codegen/coder.h"


PageFile::PageFile(const wxString& filename, bool create)
{
	m_FileName = filename;
	//m_Modified = false;
    m_Context = new wxsCoderContext;
    if(m_Undo == nullptr)
    {
        m_Undo = new wxsItemUndoBuffer();
    }

	if (!create)
	{
		LoadFromFile(filename);
		return;
	}

	m_Name = "Unnamed Page";
	m_WindowTree = new uiPage();
	Save();
}



void PageFile::RefreshPageColor(void)
{
	RefreshWindowColor(m_WindowTree);
}

void PageFile::MarkAsModified(void)
{
	SetModifiedFlag(true);
	StoreChange();
}


bool PageFile::Save(void)
{
	if (m_FileName.IsEmpty())
		return false;

	const char* PAGE_ROOT_TAG = "Designer_page_file";

//    TiXmlDocument doc;
    if (m_FirstOpenPage)
    {
        m_XmlDoc.SetCondenseWhiteSpace(false);
        m_XmlDoc.InsertEndChild(TiXmlDeclaration("1.0", "UTF-8", "yes"));
        TiXmlElement *rootNode = static_cast<TiXmlElement*>(m_XmlDoc.InsertEndChild(TiXmlElement(PAGE_ROOT_TAG)));
        if (!rootNode)
            return false;

        rootNode->InsertEndChild(TiXmlElement("FileVersion"));
        rootNode->FirstChildElement("FileVersion")->SetAttribute("major", PAGE_FILE_MAJOR_VERSION);
        rootNode->FirstChildElement("FileVersion")->SetAttribute("minor", PAGE_FILE_MINOR_VERSION);

        AddPageObjectElement(m_WindowTree, rootNode);

        m_Undo->StoreChange(GetXmlData());
        m_FirstOpenPage = false;
    }

    bool result = dsSaveTinyXMLDocument(&m_XmlDoc, m_FileName);
    if (result)
        m_Undo->Saved();//SetModifiedFlag(false);

    return result;
}

bool PageFile::SaveAs(uiWindow *window, const wxString& filename)
{
    if (!wxFile::Exists(filename))
	{
		m_FileLost = true;
		return false;
	}

    m_FileName = window->GetName();
    //wxLogMessage("copy save as filename =  %s,m_FileName = %s!",filename,m_FileName);
    wxFileDialog saveFileDialog(Manager::Get()->GetAppFrame(), wxT("save"), "", "", "Designer Page File (*.page)|*.page|All File (*.*)|*.*", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
    saveFileDialog.SetFilename(m_FileName);
    if (saveFileDialog.ShowModal() == wxID_CANCEL)
        return false;     // the user changed idea...

    m_FileName = saveFileDialog.GetPath();
	if (m_FileName.IsEmpty())
		return false;

//    TiXmlDocument doc;
//    if (!TinyXML::LoadDocument(filename, &m_XmlDoc))
//    {
//    	wxLogMessage(_T("Parse XML format error..."));
//        return false;
//    }

    //wxLogMessage("copy save as %s!",m_FileName);
    bool result = dsSaveTinyXMLDocument(&m_XmlDoc, m_FileName);
    if (result)
    	m_Undo->Saved();//SetModifiedFlag(false);

    return result;
}


bool PageFile::Export(wxString path)
{
	if (!m_WindowTree)
		return false;

	if (path.Right(1) != "\\")
		path += "\\";

	wxString fileFullName = path + m_WindowTree->GetVarName() + "_ui.c";
	wxLogMessage(fileFullName + " now exporting...");

	if (!wxFile::Exists(fileFullName))
	{
		CreateBlankCFile(fileFullName);
	}

	m_SrcFileName = fileFullName;
	return ExportAllSourceCode();
}


//-------------------------------------------------------------------------------------------------
// private
//-------------------------------------------------------------------------------------------------
void PageFile::SetModifiedFlag(bool modified)
{
	if (modified && m_Owner)
		m_Owner->MarkAsModified();

//	if (m_Modified != modified)
//	{
//		m_Modified = modified;
////		Manager::Get()->GetMainFrame()->UpdatePageList();
//	}
}


bool PageFile::LoadFromFile(const wxString& filename)
{
	if (!wxFile::Exists(filename))
	{
		m_FileLost = true;
		return false;
	}

//    TiXmlDocument doc;
    if (!TinyXML::LoadDocument(filename, &m_XmlDoc))
    {
    	wxLogMessage(_T("Parse XML format error..."));
        return false;
    }

    TiXmlElement* root;
    root = m_XmlDoc.FirstChildElement("Designer_page_file");
    if (!root)
    {
    	wxLogMessage(_T("Not a valid Designer page file..."));
    	return false;
    }

    TiXmlElement* version = root->FirstChildElement("FileVersion");
	int major = PROJECT_FILE_MAJOR_VERSION;
	int minor = PROJECT_FILE_MINOR_VERSION + 1;
	if (version)
	{
		version->QueryIntAttribute("major", &major);
		version->QueryIntAttribute("minor", &minor);
	}
	if (major >= PROJECT_FILE_MAJOR_VERSION && minor > PROJECT_FILE_MINOR_VERSION)
	{
		wxLogMessage(_T("Page file format is newer/unknown"));
//		_("This project file was saved with a newer version of Code::Blocks.\n"
//		"Will try to load, but you should make sure all the settings were loaded correctly..."),
	}

	/* load all ui objects */
    TiXmlElement* rootObject;
    const char *rootObjectClass = nullptr;
    rootObject = root->FirstChildElement("object");
    if (rootObject)
    {
    	rootObjectClass = rootObject->Attribute("class");
    }
	if (!rootObjectClass || strcmp(rootObjectClass, "UIPage") != 0)
	{
		wxLogMessage(_T("Root object must be a UIPage"));
		return false;
	}

	LoadPageObject(rootObject, nullptr);

	if (m_FirstOpenPage)
    {
        m_FirstOpenPage = false;
        m_Undo->StoreChange(GetXmlData());
    }

	m_FileLost = false;

	return true;
}


void PageFile::LoadPageObject(TiXmlElement* element, uiWindow *parent)
{
	if (element == nullptr)
		return;

	while (element)
	{
		const char* nodeClass = element->Attribute("class");
		uiWindow *window = WindowFactory::Create(nodeClass);

		if (parent == nullptr)
			m_WindowTree = window;
		else
			parent->AddSubWindow(window);

		window->SetProperties(element);
		/* add all child */
		LoadPageObject(element->FirstChildElement("object"), window);

		element = element->NextSiblingElement("object");
	}
}

void PageFile::AddPageObjectElement(uiWindow *window, TiXmlElement* parent)
{
	if (window == nullptr)
		return;

	TiXmlElement* element = window->GetXMLElement();
	parent->LinkEndChild(element);

	uiWindow *child = window->m_Childs;
	while (child)
	{
		AddPageObjectElement(child, element);
		child = child->m_Next;
	}
}


void PageFile::RefreshWindowColor(uiWindow *window)
{
	if (window)
		window->RefreshColor();

	uiWindow *child = window->m_Childs;
	while (child)
	{
		RefreshWindowColor(child);
		child = child->m_Next;
	}
}


void PageFile::CreateBlankCFile(wxString filename)
{
	wxFile cfile(filename, wxFile::write);

	cfile.Write("#include <gui.h>\n\n\n");

	wxString pageVarName = m_WindowTree->GetVarName();
	wxString pageName = m_WindowTree->GetName();

	cfile.Write(wxString::Format(wxT("void init_%s_ui(void)\n"), pageVarName.wx_str()));
	cfile.Write("{\n");
	cfile.Write(wxString::Format(wxT("\t%s\n"), CodeMarks::Beg(CodingLanguage_CPP, wxT("Declarations"), pageName).wx_str()));
	cfile.Write(wxString::Format(wxT("\t%s\n"), CodeMarks::End(CodingLanguage_CPP).wx_str()));
	cfile.Write("\n");

	cfile.Write(wxString::Format(wxT("\t%s\n"), CodeMarks::Beg(CodingLanguage_CPP, wxT("WindowStructure"), pageName).wx_str()));
	cfile.Write(wxString::Format(wxT("\t%s\n"), CodeMarks::End(CodingLanguage_CPP).wx_str()));
	cfile.Write("\n");

	cfile.Write(wxString::Format(wxT("\t%s\n"), CodeMarks::Beg(CodingLanguage_CPP, wxT("Initialize"), pageName).wx_str()));
	cfile.Write(wxString::Format(wxT("\t%s\n"), CodeMarks::End(CodingLanguage_CPP).wx_str()));
	cfile.Write("}\n");

	cfile.Close();
}

bool PageFile::ExportAllSourceCode()
{
    wxsCoderContext context;
	BuildCodeRecursively(m_WindowTree, &context);
//	wxLogMessage("\nm_LocalDeclarations = %s\n",context.m_LocalDeclarations);
//	wxLogMessage("\nm_WindowStructures = %s\n",context.m_WindowStructures);
	//wxLogMessage("\nm_BuildingCode = %s\n",context.m_BuildingCode);

	//check code
	if (!CheckCode(&context))
	{
//		if (!wxFile::Exists(m_SrcFileName))
//		{
//			//remove the file
//		}
//		return false;
	}
	if (m_WindowTree)
		m_Name = m_WindowTree->GetName();
    wxsCoder::Get()->AddCode(
    	m_SrcFileName,
        CodeMarks::Beg(CodingLanguage_CPP, wxT("Declarations"), m_Name),
        CodeMarks::End(CodingLanguage_CPP),
        DeclarationsCode(&context),
        false );

    wxsCoder::Get()->AddCode(
    	m_SrcFileName,
        CodeMarks::Beg(CodingLanguage_CPP, wxT("WindowStructure"), m_Name),
        CodeMarks::End(CodingLanguage_CPP),
        WindowStructureCode(&context),
        false );

    wxsCoder::Get()->AddCode(
        m_SrcFileName,
        CodeMarks::Beg(CodingLanguage_CPP, wxT("Initialize"), m_Name),
        CodeMarks::End(CodingLanguage_CPP),
        InitializeCode(&context),
        false );

//    wxsCoder::Get()->AddCode(
//        m_HdrFileName,
//        CodeMarks::Beg(CodingLanguage_CPP, wxT("Identifiers"), m_ClassName),
//        CodeMarks::End(CodingLanguage_CPP),
//        IdentifiersCode(&context),
//        false );
//
//    wxsCoder::Get()->AddCode(
//        m_SrcFileName,
//        CodeMarks::Beg(CodingLanguage_CPP, wxT("IdInit"), m_ClassName),
//        CodeMarks::End(CodingLanguage_CPP),
//        IdInitCode(&context),
//        false );

    wxsCoder::Get()->Flush(0);

	return true;
}

void PageFile::BuildCodeRecursively(uiWindow *window, wxsCoderContext *context)
{
	if (window == nullptr)
		return;
	if (window->GetClassName() == "UIEditItem")
		return;

	window->BuildCode(context);

	uiWindow *child;
	child = window->m_Childs;
	while (child)
	{
		BuildCodeRecursively(child, context);
		child = child->m_Next;
	}
}

wxString GenerateCodeFromSet(const wxsCoderContext::wxStringSet &set, const wxString &prefix, const wxString &suffix)
{
    std::vector<wxString> array;
    array.reserve(set.size());
    for (const wxString &item : set)
        array.push_back(item);

    std::sort(array.begin(), array.end());
    wxString Code;
    for (const wxString &item : array)
    {
        Code += prefix;
        Code += item;
        Code += suffix;
    }
    return Code;
}

wxString GenerateCodeFromArray(const wxArrayString &array, const wxString &prefix, const wxString &suffix)
{
    wxString Code;
    for (const wxString &item : array)
    {
        Code += prefix;
        Code += item;
        Code += suffix;
    }
    return Code;
}

bool PageFile::CheckCode(wxsCoderContext* Ctx)
{
	wxString code;
	wxArrayString NameArr;
	int index = 0;
	int StartPos = 0;
	int	EndPos = 0;

	code = DeclarationsCode(Ctx);

	//check if having the same name of control
	if (code.IsEmpty())
		return false;

	while(index < code.length())
	{
		index = code.find("=", index);
		if (index != wxString::npos)
		{
			if (code.at(index - 1) == ' ')
			{
				EndPos = index - 1;
				StartPos = index - 2;
			}
			else
			{
				EndPos = index;
				StartPos = index - 1;
			}
			while(code.at(StartPos--) != ' ' && StartPos >= 0);
			if (StartPos < 0)
			{
				return false;
			}
			StartPos += 2;
			NameArr.Add(code.substr(StartPos, EndPos - StartPos));
			index++;
		}
		else
		{
			break;
		}
	}
	for (size_t i = 0; i < NameArr.Count() - 1; i++)
	{
		for (size_t j = i + 1 ; j < NameArr.Count(); j++)
		{
			if (NameArr[i] == NameArr[j])
			{
			   // wxMessageBox(wxString::Format(wxT("导出错误:控件名重复(%s)"),NameArr[j].wx_str()), "Confirm", wxOK);
				wxLogMessage(wxT("创建控件出错：%s 控件变量名重复！\n"), NameArr[i]);
			    return false;
			}
		}
	}

	return true;
}

wxString PageFile::DeclarationsCode(wxsCoderContext* Ctx)
{
    // Enumerate all class members
    wxString Code = _T("\n");
//    Code += GenerateCodeFromSet(Ctx->m_GlobalDeclarations, wxEmptyString, wxT("\n"));
    Code += GenerateCodeFromArray(Ctx->m_LocalDeclarations, wxEmptyString, wxT("\n"));
    return Code;
}

wxString PageFile::WindowStructureCode(wxsCoderContext* Ctx)
{
	wxString Code = _T("\n");
	Code += GenerateCodeFromArray(Ctx->m_WindowStructures, wxEmptyString, wxT("\n"));
	return Code;
}

wxString PageFile::IdentifiersCode(wxsCoderContext* Ctx)
{
    // Enumerate all ids
    wxString Code = _T("\n");
    for ( size_t Count = Ctx->m_IdEnumerations.Count(), Index=0; Count>0; Index++, Count-- )
    {
        Code += Ctx->m_IdEnumerations[Index];
        Code += _T("\n");
    }
    return Code;
}

wxString PageFile::InitializeCode(wxsCoderContext* Ctx)
{
    wxString Code = _T("\n");

    // First there are local variables
//    Code += GenerateCodeFromSet(Ctx->m_LocalDeclarations, wxEmptyString, wxT("\n"));

    if ( Code.Length()>1 )
    {
        Code += _T("\n");
    }

//    // Next load resource's content
//    if ( Ctx->m_Flags & flSource )
//    {
//        // If in source mode, add building code
        Code += Ctx->m_BuildingCode;
//    }
//    else
//    {
//        // If in XRC mode, add XRC loading code
//        Code += XRCLoadingCode();
//        Code += Ctx->m_XRCFetchingCode;
//    }

//    if ( !Ctx->m_EventsConnectingCode.IsEmpty() )
//    {
//        // And finally attach event handlers
//        Code += _T("\n");
//        Code += Ctx->m_EventsConnectingCode;
//    }

    return Code;
}

wxString PageFile::IdInitCode(wxsCoderContext* Ctx)
{
    wxString Code = _T("\n");
    for ( size_t Count = Ctx->m_IdInitializions.Count(), Index=0; Count>0; Index++, Count-- )
    {
        Code += Ctx->m_IdInitializions[Index];
        Code += _T("\n");
    }
    return Code;
}

wxString PageFile::HeadersCode(wxsCoderContext* Ctx)
{
    wxString Code;
    // Enumerate global includes (those in header file)
    Code += GenerateCodeFromSet(Ctx->m_GlobalHeaders, wxT("\n#include "), wxEmptyString);
    Code += GenerateCodeFromSet(Ctx->m_ForwardDeclarations, wxT("\nclass "), wxT(";"));
    return Code + _T("\n");
}


wxString PageFile::GetXmlData(void)
{
    TiXmlPrinter printer;
    printer.SetIndent("\t");
    m_XmlDoc.Accept(&printer);

    return dsC2U(printer.CStr());
}

bool PageFile::StoreChange(void)
{
	const char* PAGE_ROOT_TAG = "Designer_page_file";
    //wxLogMessage("StoreChange");

    m_XmlDoc.Clear();
    m_XmlDoc.SetCondenseWhiteSpace(false);
    m_XmlDoc.InsertEndChild(TiXmlDeclaration("1.0", "UTF-8", "yes"));
    TiXmlElement *rootNode = static_cast<TiXmlElement*>(m_XmlDoc.InsertEndChild(TiXmlElement(PAGE_ROOT_TAG)));
    if (!rootNode)
        return false;

    rootNode->InsertEndChild(TiXmlElement("FileVersion"));
    rootNode->FirstChildElement("FileVersion")->SetAttribute("major", PAGE_FILE_MAJOR_VERSION);
    rootNode->FirstChildElement("FileVersion")->SetAttribute("minor", PAGE_FILE_MINOR_VERSION);

    AddPageObjectElement(m_WindowTree, rootNode);

    m_Undo->StoreChange(GetXmlData());

    return true;
}


bool PageFile::SetXmlData(const wxString& XmlData)
{
    if(XmlData.IsEmpty()) return false;
//    TiXmlDocument doc;
    m_XmlDoc.Clear();
    m_XmlDoc.Parse(dsU2C(XmlData));
    if ( m_XmlDoc.Error() )
    {
        wxLogMessage(_T("Parse XML format error..."));
        return false;
    }

    TiXmlElement* root;
    root = m_XmlDoc.FirstChildElement("Designer_page_file");
    if (!root)
    {
    	wxLogMessage(_T("Not a valid Designer page file..."));
    	return false;
    }

    TiXmlElement* version = root->FirstChildElement("FileVersion");
	int major = PROJECT_FILE_MAJOR_VERSION;
	int minor = PROJECT_FILE_MINOR_VERSION + 1;
	if (version)
	{
		version->QueryIntAttribute("major", &major);
		version->QueryIntAttribute("minor", &minor);
	}
	if (major >= PROJECT_FILE_MAJOR_VERSION && minor > PROJECT_FILE_MINOR_VERSION)
	{
		wxLogMessage(_T("Page file format is newer/unknown"));
//		_("This project file was saved with a newer version of Code::Blocks.\n"
//		"Will try to load, but you should make sure all the settings were loaded correctly..."),
	}

	/* load all ui objects */
	TiXmlElement* rootObject;
	const char *rootObjectClass = nullptr;
	rootObject = root->FirstChildElement("object");
	if (rootObject)
	{
		rootObjectClass = rootObject->Attribute("class");
	}
	if (!rootObjectClass || strcmp(rootObjectClass, "UIPage") != 0)
	{
		wxLogMessage(_T("Root object must be a UIPage"));
		return false;
	}

	LoadPageObject(rootObject, nullptr);

	m_FileLost = false;

	return true;
}

