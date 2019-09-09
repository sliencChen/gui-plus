/*
 * uiwindow.h
 *
 *  Created on: 2019-4-10
 *      Author: Brant
 */

#ifndef _UIWINDOW_H_
#define _UIWINDOW_H_


#include <wx/wx.h>
#include <wx/hashmap.h>
#include <gui.h>
#include <tinyxml.h>
#include "property.h"
#include "link_map.h"
#include "propname.h"
#include "codegen/codegenerator.h"
#include <wx/treebase.h>
using namespace std;


class uiWindow : public wxsCodeGenerator
{
public:
	uiWindow(uiWindow *parent = nullptr);
	virtual ~uiWindow(void);

	wxString Id(void);

	/* convert to stb uiwindow */
	virtual UIWindow* guiWindow(void);
	virtual wxPGChoices AddEnmuChildProperty(wxString key);
	virtual void AddUpdateChildProperty(const TiXmlElement* element = nullptr);

	/* convert to stb UISizer */
	virtual UISizer* guiSizer(void);

	/* convert to xml node */
	void InsertTextChild(TiXmlElement* element, const char *name, const char *value);
	virtual TiXmlElement* GetXMLElement(void);

	wxString GetName(void) { return GetStrPropValue(PROP_NAME, "UnnamedWindow"); }
	wxString GetVarName(void) { return GetStrPropValue(PROP_VAR_NAME, "window_var_undefined"); }
	wxString GetClassName(void) { return GetStrPropValue(PROP_CLASS, "UnnamedClass");}

	/* create properties by XML element */
	virtual void SetProperties(const TiXmlElement* element);

	/* change window property, usually called by property editor */
	void SetProperty(const wxString& key, const wxAny& value);

	/* change property, usually called by sub class for override */
	void SetIntegerProperty(const wxString& key, const char *value);
	void SetStringProperty(const wxString& key, const char *value);
	void SetColorProperty(const wxString& key, const char* value, unsigned int defaultValue = 0);
	void SetColorProperty(const wxString& key, const wxColour& value);
	void SetImageProperty(const wxString& key, const char *value);
	void SetLogoProperty(const wxString& key, const char *value);
    void SetBoolProperty(const wxString& key, const char *value);
    void SetEnumProperty(const wxString& key, const char *value);
	/* do this after system color changed */
	void RefreshColor(void);
	void RefreshString(void);


	/* search window */
	uiWindow* FindInSubWindow(wxString& Id);


	/* add sub window */
	void AddSubWindow(uiWindow *addThis);


	/* move up window */
	void MoveUpSubWindow(uiWindow *upThis);
	/* move down window */
	void MoveDownSubWindow(uiWindow *downThis);
	/* move down window */
	void DeleteSubWindow(uiWindow *deleteThis);

	const char* GetStrPropValue(wxString key, const char* defaultValue = "");

	/* get window type*/
	int GetWindowType(void) { return m_type; }

	int GetWindowIconIndex(void) { return m_icon_index; }

	void RepositionSubWindowRecursively(uiWindow* parent);

protected:
    enum ICONINDEX
    {
        ICON_CONTROL_PAGE,
        ICON_CONTROL_LABEL,
        ICON_CONTROL_IMAGE,
        ICON_CONTROL_LINE_CHART,
        ICON_CONTROL_LIST,
        ICON_CONTROL_EDIT,
        ICON_CONTROL_EDIT_ITEM,
        ICON_CONTROL_HELP,
        ICON_CONTROL_PROMPT,
        ICON_CONTROL_MESSAGE_BOX,
        ICON_CONTROL_TEXT_FIELD,
        ICON_CONTROL_LIST_SIZER,
        ICON_CONTROL_BOX_SIZER,
        ICON_CONTROL_MENU_ITEM,
        ICON_CONTROL_MENU,
        ICON_CONTROL_MAX,
    };

	int GetIntPropValue(wxString key, int defaultValue = 0);
	bool GetBoolPropValue(wxString key, int defaultValue = 0);
	int GetEnumPropValue(wxString key, int defaultValue = 0);
	wxRect GetRectPropValue(wxString key, wxRect defaultValue = wxRect());
	wxSize GetSizePropValue(wxString key, wxSize defaultValue = wxSize());
	U32 GetColorPropValue(wxString key, U32 defaultValue = 0x00ff00ff);
	const char* GetImagePropValue(wxString key);
	int GetLogoIndexPropValue(wxString key);


	/* build source code */
	virtual void OnBuildDeclarationsCode();
	virtual void OnBuildWindowStructureCode();
	virtual void OnBuildCreatingCode();
	virtual void OnBuildIdCode();

    int m_type;
    int m_icon_index;

public:
	Linked_map<wxString, PropertyBase*> m_Properties;
	uiWindow *m_Parent = nullptr;
	uiWindow *m_Next = nullptr;
	uiWindow *m_Childs = nullptr;
	UIWindow *stb_window = nullptr;
	UISizer *stb_sizer = nullptr;
	wxTreeItemId m_TreeId;

    UIPoint m_absolute_pos;
private:
	wxString RandomId(void);
	uiWindow* FindInSubWindowRecursively(uiWindow *current, wxString& Id);
	void DeleteSubWindowRecursively(uiWindow *current);
};



//-------------------------------------------------------------------------------------------------
// window factory
//-------------------------------------------------------------------------------------------------
class WindowFactory
{
public:
	static uiWindow* Create(const char *className);

private:
	static int ClassId(const char *className);
};



#endif /* _UIWINDOW_H_ */
