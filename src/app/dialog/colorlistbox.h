/*
 * colorlistbox.h
 *
 *  Created on: 2019-4-15
 *      Author: Brant
 */

#ifndef COLOR_LIST_BOX_H_
#define COLOR_LIST_BOX_H_


#include <wx/window.h>
#include <wx/dynarray.h>
#include <wx/vlbox.h>
#include "uitypes.h"


class ColorListBox : public wxVListBox
{
public:
	ColorListBox(wxWindow *parent, wxWindowID id);
	~ColorListBox(void) { }

	void SetColors(const ColorList& colorList);
	ColorList GetColors(void) { return m_ColorList; }

	bool AddColor(uiColor color);
	bool AddColor(void);

	uiColor GetColor(unsigned int index);
	void SelectColor(unsigned int index);

	bool UpdateColor(unsigned int index, uiColor color);
	bool UpdateColorName(unsigned int index, const wxString& color);
	bool UpdateColorValue(unsigned int index, unsigned int value);

	void DeleteColor(unsigned int index);

private:
	ColorList m_ColorList;

	wxCoord OnMeasureItem(size_t n) const;							/* virtual method from <wxVListBox> */
	void OnDrawItem(wxDC &dc, const wxRect &rect, size_t n) const;	/* virtual method from <wxVListBox> */

	wxDECLARE_EVENT_TABLE();
};


#endif /* COLOR_LIST_BOX_H_ */
