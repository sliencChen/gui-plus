/*
 * uilabel.h
 *
 *  Created on: 2019-4-18
 *      Author: Brant
 */

#ifndef _UILABEL_H_
#define _UILABEL_H_


#include "uiwindow.h"


class uiLabel : public uiWindow
{
public:
	uiLabel(uiWindow *parent = nullptr);
	~uiLabel(void);

	wxPGChoices AddEnmuChildProperty(wxString key);

	UIWindow* guiWindow(void);

	TiXmlElement* GetXMLElement(void);

	void SetProperties(const TiXmlElement* element);


private:
	virtual void OnBuildDeclarationsCode();
	virtual void OnBuildCreatingCode();
	virtual void OnBuildIdCode();
};


#endif /* _UILABEL_H_ */
