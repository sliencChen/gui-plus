/*
 * pagelist.h
 *
 *  Created on: 2019-4-3
 *      Author: Brant
 */

#ifndef SEARCHLIST_H_
#define SEARCHLIST_H_


#include <wx/wx.h>
#include <wx/vlbox.h>
#include <wx/treectrl.h>
#include <wx/propgrid/manager.h>
#include "window/uiwindow.h"
#include "uirender.h"
#include "pagefile.h"

class SearchResultItem
{
public:
    PageFile *pagefile;
    uiWindow *window;
    //PropertyBase property;
    wxString propety;
};

class SearchListBox : public wxVListBox, public uiWindow
{
public:
	SearchListBox(wxWindow *parent, wxWindowID id);

	void SetActivePage(PageFile* pagefile);
	void SearchText(wxString text);
	void ReloadAllData(void);
	void ClearAllData(void);

private:
	wxCoord OnMeasureItem(size_t n) const;							/* virtual method from <wxVListBox> */
	void OnDrawItem(wxDC &dc, const wxRect &rect, size_t n) const;	/* virtual method from <wxVListBox> */
	unsigned int GetResultItemCount(void) const { return SearchResultItems.size(); }
    SearchResultItem* GetResultItemAt(unsigned int i) const
    {
        if (i >= SearchResultItems.size())
            return nullptr;
        return SearchResultItems.at(i);
    }
	SearchResultItem* GetResultItemAt(unsigned int index);
	void AddResult(SearchResultItem *resultitem);

	wxVector<SearchResultItem *> SearchResultItems;
    PageFile *m_ActivePage = nullptr;
    uiWindow *m_ActiveRootWindow = nullptr;		///< root window of active page
    uiWindow *m_ActiveWindow = nullptr;			///< active window(selected item)

	wxDECLARE_EVENT_TABLE();
};


#endif /* SEARCHLIST_H_ */
