/*
 * uitypes.h
 *
 *  Created on: 2019-4-15
 *      Author: Brant
 */

#ifndef UI_TYPES_H_
#define UI_TYPES_H_


#include <wx/dynarray.h>


class wxString;

class uiColor
{
public:
	uiColor(unsigned int value, wxString name = wxEmptyString) { m_Name = name; m_Value = value; }

	wxString GetName(void) const { return m_Name; }
	wxString GetValue(void) const { return wxString::Format("0x%08X", m_Value); }

	void SetName(const wxString& name) { m_Name = name; }
	void SetValue(unsigned int value) { m_Value = value; }

	unsigned int m_Value;

	bool operator!=(const uiColor& rhs);

private:
	wxString m_Name;
};


WX_DECLARE_OBJARRAY(uiColor, ColorArray);

class ColorList
{
public:
	ColorList(void);
	ColorList(const ColorList& anotherList);
	~ColorList(void);

	unsigned int Count(void) const { return m_Colors.Count(); }

	bool AddColor(void);
	bool AddColor(uiColor color);

	uiColor GetColor(unsigned int index) const;
	uiColor GetColor(wxString name) const;
	int FindColor(wxString name) const;

	bool UpdateColor(unsigned int index, uiColor color);
	bool UpdateColorName(unsigned int index, const wxString& name);
	bool UpdateColorValue(unsigned int index, unsigned int value);

	bool DeleteColor(unsigned int index);
	bool DeleteColor(wxString name);
	void Empty(void);


	bool operator!=(const ColorList& rhs);

private:
	ColorArray m_Colors;
};


#endif /* UI_TYPES_H_ */
