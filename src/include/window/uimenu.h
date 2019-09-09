/*
 * uimenu.h
 *
 *  Created on: 2019-8-9
 *      Author: Antony
 */

#ifndef _UIMENU_H_
#define _UIMENU_H_


#include "uiwindow.h"


class uiMenu : public uiWindow
{
public:
	uiMenu(uiWindow *parent = nullptr);
	~uiMenu(void);

	wxPGChoices AddEnmuChildProperty(wxString key);

	UIWindow* guiWindow(void);

	TiXmlElement* GetXMLElement(void);

	void SetProperties(const TiXmlElement* element);


private:
	virtual void OnBuildDeclarationsCode();
	virtual void OnBuildCreatingCode();
	virtual void OnBuildIdCode();
};


#endif /* _UIMENU_H_ */




