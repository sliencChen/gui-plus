/*
 * uipage.h
 *
 *  Created on: 2019-4-18
 *      Author: Brant
 */

#ifndef _UIPAGE_H_
#define _UIPAGE_H_


#include "uiwindow.h"


class uiPage : public uiWindow
{
public:
	uiPage(void);

	wxPGChoices AddEnmuChildProperty(wxString key);

	UIWindow* guiWindow(void);
    TiXmlElement* GetXMLElement(void);
	void SetProperties(const TiXmlElement* element);


private:
	virtual void OnBuildDeclarationsCode();
	virtual void OnBuildCreatingCode();
	virtual void OnBuildIdCode();
};


#endif /* _UIPAGE_H_ */
