/*
 * uilist_sizer.h
 *
 *  Created on: 2019-8-14
 *      Author: Antony
 */

#ifndef _UILIST_SIZER_H_
#define _UILIST_SIZER_H_


#include "uiwindow.h"


class uiListSizer : public uiWindow
{
public:
	uiListSizer();
	~uiListSizer(void);

	UISizer* guiSizer(void);
	wxPGChoices AddEnmuChildProperty(wxString key);
	TiXmlElement* GetXMLElement(void);

	void SetProperties(const TiXmlElement* element);


private:
	virtual void OnBuildDeclarationsCode();
	virtual void OnBuildCreatingCode();
	virtual void OnBuildIdCode();
};


#endif /* _UILIST_SIZER_H_ */




