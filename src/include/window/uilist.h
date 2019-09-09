/*
 * uilist.h
 *
 *  Created on: 2019-8-2
 *      Author: Antony
 */

#ifndef _UILIST_H_
#define _UILIST_H_


#include "uiwindow.h"


class uiList : public uiWindow
{
public:
	uiList(uiWindow *parent = nullptr);
	~uiList(void);

	wxPGChoices AddEnmuChildProperty(wxString key);

	UIWindow* guiWindow(void);

	TiXmlElement* GetXMLElement(void);

	void SetProperties(const TiXmlElement* element);


private:
	virtual void OnBuildDeclarationsCode();
	virtual void OnBuildCreatingCode();
	virtual void OnBuildIdCode();
};


#endif /* _UILIST_H_ */


