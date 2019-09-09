#ifndef _LOGO_LIST_BOX_H_
#define _LOGO_LIST_BOX_H_


#include <wx/window.h>
#include <wx/vlbox.h>
#include "logos.h"


class LogoListBox : public wxVListBox
{
public:
	LogoListBox(wxWindow *parent, wxWindowID id);
	~LogoListBox(void) { }

	bool AddLogo(Logo* logo);
	bool DeleteLogo(Logo *logo);
    bool UpLogo(Logo *logo);
    bool DownLogo(Logo *logo);
	void SetLogo(LogoList* logoList);
	Logo* GetLogo(unsigned int index);
	Logo* GetLogo(const wxString& logoName);

private:
    LogoList  *m_LogoImage = nullptr;
	wxCoord OnMeasureItem(size_t n) const;							/* virtual method from <wxVListBox> */
	void OnDrawItem(wxDC &dc, const wxRect &rect, size_t n) const;	/* virtual method from <wxVListBox> */

	wxDECLARE_EVENT_TABLE();
};


#endif /* _LOGO_LIST_BOX_H_ */
