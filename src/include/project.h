/*
 * project.h
 *
 *  Created on: 2019-4-13
 *      Author: Brant
 */

#ifndef PROJECT_H_
#define PROJECT_H_


#include <wx/wx.h>
#include "pagefile.h"
#include "sdk_common.h"
#include "images.h"
#include "logos.h"
#include "stringmanager.h"



class Project
{
public:

	/* open an existing project file, if string is empty, create a blank project */
	Project(const wxString& filename = wxEmptyString);

	/* create a new project */
	Project(wxString& name, int width, int height, int colorMode);

	~Project(void);


	/* change project proerty */
	void ChangeProperty(wxString& name, int width, int height, int colorMode);

	/* get project name */
	wxString GetName(void) { return m_Name; }
	wxString GetNameOnTitle(void) { return m_Modified ? m_Name + "*" : m_Name; }
	wxString GetFileName(void) { return m_FileName; }

	/* set project name*/
	void SetName(wxString name) { m_Name = name; }

	/* get project canvas size */
	wxSize GetCanvasSize(void) { return m_CanvasSize; }
    /* set project canvas size*/
	void SetCanvasSize(wxSize size) { m_CanvasSize = size; }

	/* get color mode */
	int GetColorMode(void) { return m_ColorMode; }
    /* set color mode */
	void SetColorMode(int colormode) { m_ColorMode = colormode; }

	/* get project be modified or not */
	bool GetModified(void) { return m_Modified; }

	/* save project */
	bool Save(void);

	/* save project as another file */
	bool SaveAs(void);

	/* export project as source code */
	bool Export(wxString path);


	/* mark project modified */
	void MarkAsModified(void) { SetModifiedFlag(true); }

	/* get color list */
	ColorList GetColorList(void) { return m_Colors; }

	/* set colors */
	void SetColors(const ColorList& colorList);
	void DoSetColors(void);

	/* add page */
	void AddPageFile(PageFile* pagefile);

	/* page count */
	unsigned int GetPageFileCount(void);

	/* get page by index */
	PageFile* GetPageFileAt(unsigned int index);

	/* get editing page */
	void DeletePageFile(PageFile* pagefile);
	void SetActivePageFile(PageFile* pagefile);
	PageFile* GetActivePageFile(void);


	/* discard all data saved in project */
	void Clear(void);

    /* Project Language function */
	void AddLanguageData(wxString path);
	void SetProjectLanguage(int language);



	ImageList m_Images;
	LogoList m_LogoImage;

private:

	void SetModifiedFlag(bool modified);
	void SetProjFileName(const wxString& filename);

	void CreateStringManager(void);

	bool LoadFromFile(const wxString& filename);
	bool DoSaveToFile(const wxString& filename);
	bool SaveToFile(const wxString& filename);

	TiXmlElement* AddElement(TiXmlElement* parent, const char* name, const char* attr, const wxString& attribute);
	TiXmlElement* AddElement(TiXmlElement* parent, const char* name, const char* attr, int attribute);

	TiXmlElement* AddColorElement(TiXmlElement* parent, const char* name, const uiColor& color);
	TiXmlElement* AddImageElement(TiXmlElement* parent, const char* name, Image* image);
	TiXmlElement* AddLogoElement(TiXmlElement* parent, const char* name, Logo* logo);
	TiXmlElement* AddPageElement(TiXmlElement* parent, const char* name, PageFile* page);

	bool LoadProjectInfo(TiXmlElement* parentNode);
	bool LoadColors(TiXmlElement* parentNode);
	bool LoadImages(TiXmlElement* parentNode);
	bool LoadLogos(TiXmlElement* parentNode);
	bool LoadPages(TiXmlElement* parentNode);
    bool LoadStringFile(TiXmlElement* parentNode);

	wxString m_Name = "Unnamed";
	wxString m_FileName = wxEmptyString;
	wxString m_FileBasePath;

	wxSize m_CanvasSize = wxSize(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
	int m_ColorMode = DEFAULT_COLOR_MODE;

	ColorList m_Colors;
//	ImageList m_Images;

	wxVector<PageFile *> m_PageFiles;

	bool m_Modified = false;

	PageFile *m_ActivePage = nullptr;
	char **m_ColorNameList = nullptr;
	unsigned int m_ColorNameListNum = 0;
};


#endif /* PROJECT_H_ */
