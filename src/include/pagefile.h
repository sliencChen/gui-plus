/*
 * pagefile.h
 *
 *  Created on: 2019-4-13
 *      Author: Brant
 */

#ifndef PAGEFILE_H_
#define PAGEFILE_H_


#include <wx/wx.h>
#include "window/uiWindow.h"
#include "codegen/codercontext.h"
#include "wxwidgets/wxsitemundobuffer.h"

class Project;

class PageFile
{
public:

	/*
	 * if filename is empty, create a blank page
	 * otherwise, load from given file
	 */
	PageFile(const wxString& filename, bool create = true);


	/* root window of current page */
	uiWindow* GetRootWindow(void) { return m_WindowTree; }

	/* page file have be modified or not */
	bool GetModified(void) { return m_Undo->IsModified(); }

	/* get page name */
	wxString GetName(void) { return m_Name; }

	/* get file name */
	wxString GetFileName(void) { return m_FileName; }


	/* set this page belongs which project */
	void SetOwner(Project *owner) { m_Owner = owner; }

	/* when system color changed, need refresh page */
	void RefreshPageColor(void);

	/* mark this page have been modified */
	void MarkAsModified(void);


	/* save to file */
	bool Save(void);
	bool SaveAs(uiWindow *window, const wxString& filename);

	/* export to c source file */
	bool Export(wxString path);

	inline wxsCoderContext* GetCoderContext() { return m_Context; }

	/* ************************ */
    /*  Undo buffer operations  */
    /* ************************ */
    /** \brief Checking if can Undo */
    inline bool CanUndo() { return m_Undo->CanUndo(); }

    /** \brief Checking if can Redo */
    inline bool CanRedo() { return m_Undo->CanRedo(); }

    /** \brief Undoing */
    inline void Undo() { SetXmlData(m_Undo->Undo()); }

    /** \brief Redoing */
    inline void Redo() { SetXmlData(m_Undo->Redo()); }

private:

	/* set modify flag */
	void SetModifiedFlag(bool modified);

	/* load page file */
	bool LoadFromFile(const wxString& filename);

	/* convert window to xml format and insert into parent node as child */
	void AddPageObjectElement(uiWindow *window, TiXmlElement* parent);

	/* create window from xml node */
	void LoadPageObject(TiXmlElement* node, uiWindow *parent);

	/* refresh window color */
	void RefreshWindowColor(uiWindow *window);

	/*check the code if having same name of control, using the nonexistent source...*/
	bool CheckCode(wxsCoderContext* Ctx);

	/* export source code internal */
	void CreateBlankCFile(wxString filename);
	bool ExportAllSourceCode();



	/* build code for all window */
	void BuildCodeRecursively(uiWindow *window, wxsCoderContext *context);

	/* for collect code */
	wxString DeclarationsCode(wxsCoderContext* Ctx);
	wxString WindowStructureCode(wxsCoderContext* Ctx);
	wxString IdentifiersCode(wxsCoderContext* Ctx);
	wxString InitializeCode(wxsCoderContext* Ctx);
	wxString IdInitCode(wxsCoderContext* Ctx);
	wxString HeadersCode(wxsCoderContext* Ctx);


	wxString GetXmlData(void);
    /** \brief Restoring resource data from string with xml data */
    bool SetXmlData(const wxString& XmlData);
    bool StoreChange(void);


	wxString m_Name;
	wxString m_FileName;
	//bool m_Modified;
	bool m_FileLost;

	uiWindow *m_WindowTree = nullptr;
	Project *m_Owner = nullptr;

	wxString m_SrcFileName;
	wxString m_HdrFileName;

	wxsCoderContext* m_Context;

	wxsItemUndoBuffer* m_Undo = nullptr;
	bool m_FirstOpenPage = true;
	TiXmlDocument m_XmlDoc;
};


#endif /* PAGEFILE_H_ */
