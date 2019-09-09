/*
 * uipagetheme.cpp
 *
 *  Created on: 2019-6-5
 *      Author: Brant
 */

#include <theme/uipagetheme.h>


uiPageTheme::uiPageTheme(const wxString& name, const wxString& subName)
{
	m_Name = name;
	m_SubName = subName;

	m_Size = wxSize(100, 100);
}
