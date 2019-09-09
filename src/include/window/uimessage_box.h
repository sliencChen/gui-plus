/*
 * uimessage_box.h
 *
 *  Created on: 2019-8-11
 *      Author: Antony
 */

#ifndef _UIMESSAGE_BOX_H_
#define _UIMESSAGE_BOX_H_


#include "uiwindow.h"


class uiMessageBox : public uiWindow
{
public:
	uiMessageBox(uiWindow *parent = nullptr);
	~uiMessageBox(void);

	wxPGChoices AddEnmuChildProperty(wxString key);

	UIWindow* guiWindow(void);

	TiXmlElement* GetXMLElement(void);

	void SetProperties(const TiXmlElement* element);


private:
	virtual void OnBuildDeclarationsCode();
	virtual void OnBuildCreatingCode();
	virtual void OnBuildIdCode();
};


#endif /* _UIMESSAGE_BOX_H_ */


