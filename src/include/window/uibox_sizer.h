/*
 * uibox_sizer.h
 *
 *  Created on: 2019-8-14
 *      Author: Antony
 */

#ifndef _UIBOX_SIZER_H_
#define _UIBOX_SIZER_H_


#include "uiwindow.h"


class uiBoxSizer : public uiWindow
{
public:
	uiBoxSizer();
	~uiBoxSizer(void);

	UISizer* guiSizer(void);
	wxPGChoices AddEnmuChildProperty(wxString key);
	TiXmlElement* GetXMLElement(void);

	void SetProperties(const TiXmlElement* element);


private:
	virtual void OnBuildDeclarationsCode();
	virtual void OnBuildCreatingCode();
	virtual void OnBuildIdCode();
};


#endif /* _UIBOX_SIZER_H_ */





