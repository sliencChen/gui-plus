/*
 * pagelist.h
 *
 *  Created on: 2019-4-3
 *      Author: Brant
 */

#ifndef PAGELIST_H_
#define PAGELIST_H_


#include <wx/wx.h>
#include <wx/vlbox.h>
#include "window/uiwindow.h"
#include "uirender.h"


class PageListWindow : public wxVListBox
{
public:
	PageListWindow(wxWindow *parent, wxWindowID id);

	void ReloadAllData(void);
	void ReloadCurrent(void);

	void SelectFirst(void);
	void SelectLast(void);

private:
	void OnDeletePage(wxCommandEvent& event);
	void OnRightDownMenu(wxContextMenuEvent& event);
	void OnUpdateUIRightDown(wxUpdateUIEvent& event);
	wxCoord OnMeasureItem(size_t n) const;							/* virtual method from <wxVListBox> */
	void OnDrawItem(wxDC &dc, const wxRect &rect, size_t n) const;	/* virtual method from <wxVListBox> */

	wxDECLARE_EVENT_TABLE();
};

class PageListEvent : public wxCommandEvent
{
public:
	PageListEvent(wxEventType commandType = wxEVT_NULL, int id = 0) : wxCommandEvent(commandType, id) {}
	PageListEvent(const PageListEvent& event) : wxCommandEvent(event) {}

	virtual wxEvent* Clone(void) const { return new PageListEvent(*this); }

	DECLARE_DYNAMIC_CLASS(PageListEvent);
};

typedef void (wxEvtHandler::*PagelistEventHandlerFunction)(PageListEvent&);

BEGIN_DECLARE_EVENT_TYPES()
	DECLARE_EVENT_TYPE(PAGELIST_EVT_COMMAND_DELETE, 901)
END_DECLARE_EVENT_TYPES()

#define EVT_PAGELIST_DELETE(id, fn) DECLARE_EVENT_TABLE_ENTRY(PAGELIST_EVT_COMMAND_DELETE, id, -1, (wxObjectEventFunction)(wxEventFunction)(PagelistEventHandlerFunction)&fn, (wxObject*)NULL),

enum
{
    IDC_PAGELIST_BEGIN_EVENT = 100,
    IDC_PAGELIST_DELETE_EVENT,
    IDC_PAGELIST_MAX_EVENT
};


#endif /* PAGELIST_H_ */
