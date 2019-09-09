/*
 * uimenu_item.h
 *
 *  Created on: 2019-8-9
 *      Author: Antony
 */

#ifndef _UIMENU_ITEM_H_
#define _UIMENU_ITEM_H_


#include "uiwindow.h"


class uiMenuItem : public uiWindow
{
public:
	uiMenuItem(uiWindow *parent = nullptr);
	~uiMenuItem(void);

	wxPGChoices AddEnmuChildProperty(wxString key);

	UIWindow* guiWindow(void);

	TiXmlElement* GetXMLElement(void);

	void SetProperties(const TiXmlElement* element);


private:
	virtual void OnBuildDeclarationsCode();
	virtual void OnBuildCreatingCode();
	virtual void OnBuildIdCode();
};


#endif /* _UIMENU_ITEM_H_ */



