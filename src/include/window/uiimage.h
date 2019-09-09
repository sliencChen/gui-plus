/*
 * uiimage.h
 *
 *  Created on: 2019-4-25
 *      Author: Brant
 */

#ifndef _UIIMAGE_H_
#define _UIIMAGE_H_


#include "uiwindow.h"


class uiImage : public uiWindow
{
public:
	uiImage(uiWindow *parent = nullptr);
	~uiImage(void);

	wxPGChoices AddEnmuChildProperty(wxString key);

	UIWindow* guiWindow(void);

	TiXmlElement* GetXMLElement(void);

	void SetProperties(const TiXmlElement* element);


private:
	virtual void OnBuildDeclarationsCode();
	virtual void OnBuildCreatingCode();
	virtual void OnBuildIdCode();
};


#endif /* _UIIMAGE_H_ */
