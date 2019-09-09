/*
 * uiedit_item.h
 *
 *  Created on: 2019-8-6
 *      Author: Antony
 */

#ifndef _UIEDIT_ITEM_H_
#define _UIEDIT_ITEM_H_


#include "uiwindow.h"


class uiEditItem : public uiWindow
{
public:
	uiEditItem(uiWindow *parent = nullptr);
	~uiEditItem(void);

	wxPGChoices AddEnmuChildProperty(wxString key);

	UIWindow* guiWindow(void);

	TiXmlElement* GetXMLElement(void);

	void SetProperties(const TiXmlElement* element);


private:
	virtual void OnBuildDeclarationsCode();
	virtual void OnBuildWindowStructureCode();
	virtual void OnBuildCreatingCode();
	virtual void OnBuildIdCode();
};


#endif /* _UIEDIT_ITEM_H_ */


