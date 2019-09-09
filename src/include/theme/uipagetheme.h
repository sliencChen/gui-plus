/*
 * uipagetheme.h
 *
 *  Created on: 2019-6-5
 *      Author: Brant
 */

#ifndef UI_PAGE_THEME_H_
#define UI_PAGE_THEME_H_

#include "theme.h"


class uiPageTheme : public uiTheme
{
public:
	uiPageTheme(const wxString& name = wxT("New UIPage Theme"), const wxString& subName = wxT("Default"));
};


#endif /* UI_PAGE_THEME_H_ */
