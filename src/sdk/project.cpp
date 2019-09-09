/*
 * project.cpp
 *
 *  Created on: 2019-4-13
 *      Author: Brant
 */

#include <stdio.h>
#include "sdk_common.h"
#include "main.h"
#include "colors.h"
#include "project.h"

Project::Project(const wxString& filename)
{
	if (!filename.IsEmpty())
	{
		LoadFromFile(filename);
		return;
	}
	m_Name = "unnamed";
	m_FileName = wxEmptyString;
	m_FileBasePath = wxEmptyString;
	m_CanvasSize = wxSize(1280, 720);
	m_ColorMode = 0;
}

Project::Project(wxString& name, int width, int height, int colorMode)
{
	m_Name = name;
	m_CanvasSize = wxSize(width, height);
	m_ColorMode = colorMode;
}

Project::~Project(void)
{
	wxVector<PageFile *>::iterator it;
	for (it = m_PageFiles.begin(); it != m_PageFiles.end(); it++)
	{
		delete (*it);
	}

	delete m_ColorNameList;
}


void Project::ChangeProperty(wxString& name, int width, int height, int colorMode)
{
	if (m_Name != name) { m_Name = name; SetModifiedFlag(true);}
	if (m_CanvasSize.GetWidth() != width) { m_CanvasSize.SetWidth(width); SetModifiedFlag(true); }
	if (m_CanvasSize.GetHeight() != height) { m_CanvasSize.SetHeight(height); SetModifiedFlag(true); }
	if (m_ColorMode != colorMode) { m_ColorMode = colorMode; SetModifiedFlag(true); }
}


bool Project::Save(void)
{
	if (m_FileName.IsEmpty())
	{
		return SaveAs();
	}

	return SaveToFile(m_FileName);
}

bool Project::SaveAs(void)
{
	wxFileDialog saveFileDialog(Manager::Get()->GetAppFrame(), wxT("保存"), "", "", "Designer Project File (*.osd)|*.osd|All File (*.*)|*.*", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

	saveFileDialog.SetFilename(m_Name);
	if (saveFileDialog.ShowModal() == wxID_CANCEL)
		return false;     // the user changed idea...

	return SaveToFile(saveFileDialog.GetPath());
}

bool Project::Export(wxString path)
{
	bool result = true;
	wxVector<PageFile *>::iterator it;
	for (it = m_PageFiles.begin(); it != m_PageFiles.end(); it++)
	{
		result = result && (*it)->Export(path);
	}
	return result;
}


void Project::AddLanguageData(wxString path)
{
	StringManager::Get()->DeleteLanguageData();
	StringManager::Get()->m_Stringpaths = path;
	StringManager::Get()->LoadLanguageData();
}

void Project::SetProjectLanguage(int language)
{
	StringManager::Get()->SetLanguage(language);
}

void Project::SetColors(const ColorList& colorList)
{
	if (m_Colors != colorList)
	{
		m_Colors = ColorList(colorList);
		SetModifiedFlag(true);
	}
}

void Project::DoSetColors(void)
{
	for (unsigned int i = 0; i < GetPageFileCount(); i++)
	{
		GetPageFileAt(i)->RefreshPageColor();
	}
}


void Project::AddPageFile(PageFile* pagefile)
{
	pagefile->SetOwner(this);
	m_PageFiles.push_back(pagefile);
}

unsigned int Project::GetPageFileCount(void)
{
	return m_PageFiles.size();
}

PageFile* Project::GetPageFileAt(unsigned int index)
{
	if (index < GetPageFileCount())
	{
		return m_PageFiles.at(index);
	}
	return nullptr;
}

void Project::DeletePageFile(PageFile* pagefile)
{
    wxVector<PageFile *>::iterator it;
	PageFile *item;

	for (it = m_PageFiles.begin(); it != m_PageFiles.end(); it++)
	{
		item = *it;
		
		if (pagefile->GetFileName().IsSameAs(item->GetFileName()))
		{
			m_PageFiles.erase(it);
			break;
		}
	}
}

void Project::SetActivePageFile(PageFile* pagefile)
{
	if (pagefile)
	{
		m_ActivePage = pagefile;
	}
}

PageFile* Project::GetActivePageFile(void)
{
	return m_ActivePage;
}


void Project::Clear(void)
{
	/* reset project property */
	m_Name = "";
	m_FileName = "";
	m_CanvasSize = wxSize(0, 0);
	m_ColorMode = -1;

	/* delete all pages */

	/* delete all colors */

	/* delete all images */

	SetModifiedFlag(false);
}



void Project::SetModifiedFlag(bool modified)
{
	if (m_Modified != modified)
	{
		m_Modified = modified;
		Manager::Get()->GetMainFrame()->UpdateAppTitle();
	}
}

void Project::SetProjFileName(const wxString& filename)
{
	m_FileName = filename;

	wxFileName projFileName(filename);
	m_FileBasePath = projFileName.GetPath();
}


bool Project::LoadFromFile(const wxString& filename)
{
	TiXmlDocument doc;
	if (!TinyXML::LoadDocument(filename, &doc))
	{
		wxLogMessage(_T("Parse XML format error..."));
		return false;
	}

	TiXmlElement* root;
	TiXmlElement* proj;

	root = doc.FirstChildElement("Designer_project_file");
	if (!root)
	{
		wxLogMessage(_T("Not a valid Designer project file..."));
		return false;
	}
	proj = root->FirstChildElement("Project");
	if (!proj)
	{
		wxLogMessage(_T("No 'Project' element in file..."));
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
		wxLogMessage(_T("Project file format is newer/unknown"));
//		_("This project file was saved with a newer version of Code::Blocks.\n"
//		"Will try to load, but you should make sure all the settings were loaded correctly..."),
	}

	SetProjFileName(filename);

	LoadProjectInfo(proj);
	LoadColors(proj);
	LoadImages(proj);
	LoadLogos(proj);
	LoadStringFile(proj); /* must be before load pages */
	LoadPages(proj);

	return true;
}

bool Project::DoSaveToFile(const wxString& filename)
{
	const char* ROOT_TAG = "Designer_project_file";
	unsigned int i;

	TiXmlDocument doc;
	doc.SetCondenseWhiteSpace(false);
	doc.InsertEndChild(TiXmlDeclaration("1.0", "UTF-8", "yes"));
	TiXmlElement *rootNode = static_cast<TiXmlElement*>(doc.InsertEndChild(TiXmlElement(ROOT_TAG)));
	if (!rootNode)
		return false;
	rootNode->InsertEndChild(TiXmlElement("FileVersion"));
	rootNode->FirstChildElement("FileVersion")->SetAttribute("major", PROJECT_FILE_MAJOR_VERSION);
	rootNode->FirstChildElement("FileVersion")->SetAttribute("minor", PROJECT_FILE_MINOR_VERSION);



	rootNode->InsertEndChild(TiXmlElement("Project"));
	TiXmlElement *projectNode = rootNode->FirstChildElement("Project");

	AddElement(projectNode, "Info", "name", m_Name);
	AddElement(projectNode, "Info", "width", m_CanvasSize.GetWidth());
	AddElement(projectNode, "Info", "height", m_CanvasSize.GetHeight());
	AddElement(projectNode, "Info", "colorMode", m_ColorMode);

	projectNode->InsertEndChild(TiXmlElement("Colors"));
	TiXmlElement *colorsNode = projectNode->FirstChildElement("Colors");
	for (i = 0; i < m_Colors.Count(); i++)
	{
		AddColorElement(colorsNode, "Color", m_Colors.GetColor(i));
	}

	projectNode->InsertEndChild(TiXmlElement("Images"));
	TiXmlElement *imagesNode = projectNode->FirstChildElement("Images");
	for (i = 0; i < m_Images.Count(); i++)
	{
		AddImageElement(imagesNode, "Image", m_Images.GetImageAt(i));
	}

	projectNode->InsertEndChild(TiXmlElement("Logos"));
	TiXmlElement *logosNode = projectNode->FirstChildElement("Logos");
	for (i = 0; i < m_LogoImage.Count(); i++)
	{
		AddLogoElement(logosNode, "Logo", m_LogoImage.GetLogoAt(i));
	}

	if (!StringManager::Get()->m_Stringpaths.IsEmpty())
	{
		projectNode->InsertEndChild(TiXmlElement("StringFile"));
		TiXmlElement *StringNode = projectNode->FirstChildElement("StringFile");
		StringNode->InsertEndChild(TiXmlElement("FileInfo"));
		{
			wxFileName StringFileName(StringManager::Get()->m_Stringpaths);
			StringFileName.MakeRelativeTo(m_FileBasePath);
			StringNode->FirstChildElement("FileInfo")->SetAttribute("name", dsU2C(StringFileName.GetFullPath()));
			StringNode->FirstChildElement("FileInfo")->SetAttribute("Language", dsC2U(StringManager::Get()->GetSaveLanguageMark()));
		}
	}



	projectNode->InsertEndChild(TiXmlElement("Pages"));
	TiXmlElement *pagesNode = projectNode->FirstChildElement("Pages");
	wxVector<PageFile *>::iterator it;
	for (it = m_PageFiles.begin(); it != m_PageFiles.end(); it++)
	{
		AddPageElement(pagesNode, "Page", *it);
		(*it)->Save();
	}

	return dsSaveTinyXMLDocument(&doc, filename);
}

bool Project::SaveToFile(const wxString& filename)
{
	wxString message = "Saving ";
	message += filename;
	wxLogMessage(message);

	bool result = DoSaveToFile(filename);
	if (result)
	{
		if (m_FileName != filename) { SetProjFileName(filename); }		/* from SaveAs */
		SetModifiedFlag(false);
	}

	message = result ? "Save success" : "Save fail!";
	wxLogMessage(message);

	return result;
}



TiXmlElement* Project::AddElement(TiXmlElement* parent, const char* name, const char* attr, const wxString& attribute)
{
	TiXmlElement elem(name);

	if (attr)
		elem.SetAttribute(attr, dsU2C(attribute));

	return parent->InsertEndChild(elem)->ToElement();
}

TiXmlElement* Project::AddElement(TiXmlElement* parent, const char* name, const char* attr, int attribute)
{
	TiXmlElement elem(name);

	if (attr)
		elem.SetAttribute(attr, attribute);

	return parent->InsertEndChild(elem)->ToElement();
}

TiXmlElement* Project::AddColorElement(TiXmlElement* parent, const char* name, const uiColor& color)
{
	TiXmlElement elem(name);

	elem.SetAttribute("name", dsU2C(color.GetName()));
	elem.SetAttribute("value", dsU2C(color.GetValue()));

	return parent->InsertEndChild(elem)->ToElement();
}

TiXmlElement* Project::AddImageElement(TiXmlElement* parent, const char* name, Image* image)
{
	TiXmlElement elem(name);

	elem.SetAttribute("name", dsU2C(image->GetName()));

	if (image->GetColorMode() == ColorMode_Auto)
		elem.SetAttribute("colorMode", "auto");
	else
		elem.SetAttribute("colorMode", image->GetColorMode());

	if (image->HaveKeyColor())
	{
		char keyColor[32];

		sprintf(keyColor, "0x%08X", image->GetKeyColor());
		elem.SetAttribute("keyColor", keyColor);
	}

	if (image->GetSliceMode() > 0)
	{
		elem.InsertEndChild(TiXmlElement("Slice"));
		TiXmlElement *slice = elem.FirstChildElement("Slice");

		slice->SetAttribute("mode", image->GetSliceMode());
		slice->SetAttribute("left", image->GetSliceLeft());
		slice->SetAttribute("right", image->GetSliceRight());
		slice->SetAttribute("top", image->GetSliceTop());
		slice->SetAttribute("bottom", image->GetSliceBottom());
		slice->SetAttribute("fillMode", image->GetSliceFillMode());
		slice->SetAttribute("fillWidth", image->GetSliceFillWidth());
		slice->SetAttribute("fillHeight", image->GetSliceFillHeight());
	}

	wxFileName imageFileName(image->GetFileName());
	imageFileName.MakeRelativeTo(m_FileBasePath);
	elem.SetAttribute("filename", dsU2C(imageFileName.GetFullPath()));

	return parent->InsertEndChild(elem)->ToElement();
}

TiXmlElement* Project::AddLogoElement(TiXmlElement* parent, const char* name, Logo* logo)
{
	TiXmlElement elem(name);

	elem.SetAttribute("name", dsU2C(logo->GetName()));
	wxFileName imageFileName(logo->GetFileName());
	imageFileName.MakeRelativeTo(m_FileBasePath);
	elem.SetAttribute("filename", dsU2C(imageFileName.GetFullPath()));

	return parent->InsertEndChild(elem)->ToElement();
}

TiXmlElement* Project::AddPageElement(TiXmlElement* parent, const char* name, PageFile* page)
{
	TiXmlElement elem(name);

	wxFileName pageFileName(page->GetFileName());
	pageFileName.MakeRelativeTo(m_FileBasePath);

	elem.SetAttribute("name", dsU2C(page->GetName()));
	elem.SetAttribute("filename", dsU2C(pageFileName.GetFullPath()));

	return parent->InsertEndChild(elem)->ToElement();
}


bool Project::LoadProjectInfo(TiXmlElement* parentNode)
{
	TiXmlElement* node = parentNode->FirstChildElement("Info");
	if (!node)
		return false; // no options

	wxString name;
	int width = 1280;
	int height = 720;
	int colorMode = 0;

	while (node)
	{
		if (node->Attribute("name"))
		{
			name = dsC2U(node->Attribute("name"));
			if (name.Trim().IsEmpty())
			{
				name = _T("unnamed");
				wxLogMessage("Can not find project name, use <unnamed> as default.");
			}
		}
		else if (node->Attribute("width"))
		{
			if (node->QueryIntAttribute("width", &width) != TIXML_SUCCESS)
			{
				width = 1280;
				wxLogMessage("Can not find project width, use 1280 as default.");
			}
		}
		else if (node->Attribute("height"))
		{
			if (node->QueryIntAttribute("height", &height) != TIXML_SUCCESS)
			{
				height = 720;
				wxLogMessage("Can not find project height, use 720 as default.");
			}
		}
		else if (node->Attribute("colorMode"))
		{
			if (node->QueryIntAttribute("colorMode", &colorMode) != TIXML_SUCCESS)
			{
				colorMode = 0;
				wxLogMessage("Can not find project color mode, use ARGB-8888 as default.");
			}
		}

	node = node->NextSiblingElement("Info");
	}

	m_Name = name;
	m_CanvasSize = wxSize(width, height);
	m_ColorMode = colorMode;

	return true;
}

bool Project::LoadColors(TiXmlElement* parentNode)
{
	TiXmlElement* colorsNode = parentNode->FirstChildElement("Colors");
	if (!colorsNode)
		return false; // no colors

	TiXmlElement* node = colorsNode->FirstChildElement("Color");
	wxString name;
	unsigned int value;

	while (node)
	{
		if (node->Attribute("name") && node->Attribute("value"))
		{
			name = dsC2U(node->Attribute("name"));
			name.Trim();

			sscanf(node->Attribute("value"), "0x%08X", &value);

			m_Colors.AddColor(uiColor(value, name));
		}
		else
		{
			wxLogMessage("Bad color record, ignore it");
		}
		node = node->NextSiblingElement("Color");
	}
	Colors::Get()->Refresh(this);

	return true;
}

bool Project::LoadImages(TiXmlElement* parentNode)
{
	TiXmlElement* colorsNode = parentNode->FirstChildElement("Images");
	if (!colorsNode)
		return false; // no images

	TiXmlElement* node = colorsNode->FirstChildElement("Image");
	wxString localFileName;
	while (node)
	{
		if (node->Attribute("name") && node->Attribute("filename"))
		{
			wxString name = dsC2U(node->Attribute("name"));
			name.Trim();

			wxString filename = dsC2U(node->Attribute("filename"));
			localFileName = filename;
			wxFileName fname(filename);
			if (!fname.IsAbsolute())
			{
				fname.MakeAbsolute(m_FileBasePath);
				localFileName = fname.GetFullPath();
			}
//			fname.Assign(m_FileBasePath + wxFILE_SEP_PATH + localFileName);
//			fname.Normalize(wxPATH_NORM_DOTS | wxPATH_NORM_TILDE, m_FileBasePath);

			Image *image = new Image(fname.GetFullPath(), name);

			/* read other optional item */
			if (node->Attribute("colorMode"))
			{
				wxString mode = dsC2U(node->Attribute("colorMode"));
				if (mode.CmpNoCase("auto") == 0)
					image->SetColorMode(ColorMode_Auto);
				else
				{
					long value;
					if (mode.ToLong(&value))
						image->SetColorMode(value);
				}
			}
			if (node->Attribute("keyColor"))
			{
				image->SetUseKeyColor(true);
				image->SetKeyColor(HexStringValue(node->Attribute("keyColor")));
			}
			else
			{
				image->SetUseKeyColor(false);
			}

			/* read slice */
			TiXmlElement *slice = node->FirstChildElement("Slice");
			if (slice)
			{
				if (slice->Attribute("mode"))
					image->SetSliceMode(atoi(slice->Attribute("mode")));

				if (slice->Attribute("left"))
					image->SetSliceLeft(atoi(slice->Attribute("left")));

				if (slice->Attribute("right"))
					image->SetSliceRight(atoi(slice->Attribute("right")));

				if (slice->Attribute("top"))
					image->SetSliceTop(atoi(slice->Attribute("top")));

				if (slice->Attribute("bottom"))
					image->SetSliceBottom(atoi(slice->Attribute("bottom")));

				if (slice->Attribute("fillMode"))
					image->SetSliceFillMode(atoi(slice->Attribute("fillMode")));

				if (slice->Attribute("fillWidth"))
					image->SetSliceFillWidth(atoi(slice->Attribute("fillWidth")));

				if (slice->Attribute("fillHeight"))
					image->SetSliceFillHeight(atoi(slice->Attribute("fillHeight")));

			}

			m_Images.AddImage(image);
		}
		else
		{
			wxLogMessage("Bad image record, ignore it");
		}
		node = node->NextSiblingElement("Image");
	}
	m_Images.ApplyToGUISystem();

	return true;
}

bool Project::LoadLogos(TiXmlElement* parentNode)
{
	TiXmlElement* colorsNode = parentNode->FirstChildElement("Logos");
	if (!colorsNode)
		return false; // no images

	TiXmlElement* node = colorsNode->FirstChildElement("Logo");
	wxString localFileName;
	while (node)
	{
		if (node->Attribute("name") && node->Attribute("filename"))
		{
			wxString name = dsC2U(node->Attribute("name"));
			name.Trim();

			wxString filename = dsC2U(node->Attribute("filename"));
			localFileName = filename;
			wxFileName fname(filename);
			if (!fname.IsAbsolute())
			{
				fname.MakeAbsolute(m_FileBasePath);
				localFileName = fname.GetFullPath();
			}
//			fname.Assign(m_FileBasePath + wxFILE_SEP_PATH + localFileName);
//			fname.Normalize(wxPATH_NORM_DOTS | wxPATH_NORM_TILDE, m_FileBasePath);

			Logo *logo = new Logo(fname.GetFullPath(), name);

			/* read other optional item */
//			if (node->Attribute("colorMode"))
//			{
//				wxString mode = dsC2U(node->Attribute("colorMode"));
//				if (mode.CmpNoCase("auto") == 0)
//					image->SetColorMode(ColorMode_Auto);
//				else
//				{
//					long value;
//					if (mode.ToLong(&value))
//						image->SetColorMode(value);
//				}
//			}
//			if (node->Attribute("keyColor"))
//			{
//				image->SetUseKeyColor(true);
//				image->SetKeyColor(HexStringValue(node->Attribute("keyColor")));
//			}
//			else
//			{
//				image->SetUseKeyColor(false);
//			}

			m_LogoImage.AddLogo(logo);
		}
		else
		{
			wxLogMessage("Bad logo image record, ignore it");
		}
		node = node->NextSiblingElement("Logo");
	}
	m_LogoImage.ApplyToGUISystem();

	return true;
}


bool Project::LoadPages(TiXmlElement* parentNode)
{
	TiXmlElement* pagesNode = parentNode->FirstChildElement("Pages");
	if (!pagesNode)
		return false; // no pages

	TiXmlElement* node = pagesNode->FirstChildElement("Page");
	wxString filename;
	wxString localFileName;

	while (node)
	{
		if (node->Attribute("filename"))
		{
			filename = dsC2U(node->Attribute("filename"));
			localFileName = filename;

			wxFileName fname(filename);
			if (!fname.IsAbsolute())
			{
				fname.MakeAbsolute(m_FileBasePath);
				localFileName = fname.GetFullPath();
			}
//			fname.Assign(m_FileBasePath + wxFILE_SEP_PATH + localFileName);
//			fname.Normalize(wxPATH_NORM_DOTS | wxPATH_NORM_TILDE, m_FileBasePath);

			AddPageFile(new PageFile(fname.GetFullPath(), false));
		}
		else
		{
			wxLogMessage("Bad page record, ignore it");
		}
		node = node->NextSiblingElement("Page");
	}

	return true;
}

bool Project::LoadStringFile(TiXmlElement* parentNode)
{

	TiXmlElement* StringNode = parentNode->FirstChildElement("StringFile");
	if (!StringNode)
		return false; // no stringfile

	TiXmlElement* node = StringNode->FirstChildElement("FileInfo");
	wxString filename;
	wxString localFileName;

	if (node)
	{
		if (node->Attribute("name") && node->Attribute("Language"))
		{
			filename = dsC2U(node->Attribute("name"));
			localFileName = filename;

			wxFileName fname(filename);
			if (!fname.IsAbsolute())
			{
				fname.MakeAbsolute(m_FileBasePath);
				localFileName = fname.GetFullPath();
			}
//			if (!fname.IsAbsolute())
//			{
//				fname.Assign(m_FileBasePath + wxFILE_SEP_PATH + localFileName);
//				fname.Normalize(wxPATH_NORM_DOTS | wxPATH_NORM_TILDE, m_FileBasePath);
//			}
			if (!StringManager::Get()->m_Stringpaths.IsEmpty())
			{
				StringManager::Get()->DeleteLanguageData();
			}
			StringManager::Get()->m_Stringpaths = fname.GetFullPath();
			StringManager::Get()->SetSaveLanguageIndex((char *)node->Attribute("Language"));
		}
		if (!StringManager::Get()->m_Stringpaths.IsEmpty())
		{
			StringManager::Get()->LoadLanguageData();
		}
	}

	return true;
}
