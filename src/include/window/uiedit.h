/*
 * uiedit.h
 *
 *  Created on: 2019-8-6
 *      Author: Antony
 */

#ifndef _UIEDIT_H_
#define _UIEDIT_H_


#include "uiwindow.h"


class uiEdit : public uiWindow
{
public:
	uiEdit(uiWindow *parent = nullptr);
	~uiEdit(void);

	wxPGChoices AddEnmuChildProperty(wxString key);
	void AddUpdateChildProperty(const TiXmlElement* element);
	UIWindow* guiWindow(void);

	TiXmlElement* GetXMLElement(void);

	void SetProperties(const TiXmlElement* element);
	void BuildCode(wxsCoderContext* Context);


private:
	virtual void OnBuildDeclarationsCode();
	virtual void OnBuildCreatingCode();
	virtual void OnBuildIdCode();
};


#endif /* _UIEDIT_H_ */



