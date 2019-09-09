/*
 * uiprompt.h
 *
 *  Created on: 2019-8-11
 *      Author: Antony
 */

#ifndef _UIPROMPT_H_
#define _UIPROMPT_H_


#include "uiwindow.h"


class uiPrompt : public uiWindow
{
public:
	uiPrompt(uiWindow *parent = nullptr);
	~uiPrompt(void);

	wxPGChoices AddEnmuChildProperty(wxString key);

	UIWindow* guiWindow(void);

	TiXmlElement* GetXMLElement(void);

	void SetProperties(const TiXmlElement* element);


private:
	virtual void OnBuildDeclarationsCode();
	virtual void OnBuildCreatingCode();
	virtual void OnBuildIdCode();
};


#endif /* _UIPROMPT_H_ */

