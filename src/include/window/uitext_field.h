/*
 * uitext_field.h
 *
 *  Created on: 2019-8-6
 *      Author: Antony
 */

#ifndef _UITEXT_FIELD_H_
#define _UITEXT_FIELD_H_


#include "uiwindow.h"
class MainFrame;

class uiTextField : public uiWindow
{
public:
	uiTextField(uiWindow *parent = nullptr);
	~uiTextField(void);

	wxPGChoices AddEnmuChildProperty(wxString key);

	UIWindow* guiWindow(void);

	TiXmlElement* GetXMLElement(void);

	void SetProperties(const TiXmlElement* element);


private:
	virtual void OnBuildDeclarationsCode();
	virtual void OnBuildCreatingCode();
	virtual void OnBuildIdCode();
};


#endif /* _UITEXT_FIELD_H_ */




