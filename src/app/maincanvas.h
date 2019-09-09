/*
 * maincanvas.h
 *
 *  Created on: 2019-3-17
 *      Author: Brant
 */

#ifndef MAIN_CANVAS_H_
#define MAIN_CANVAS_H_


#include <wx/wx.h>
#include "wx/dcbuffer.h"
#include "wx/dcclient.h"
#include "uirender.h"


class CanvasWindow : public wxScrolledWindow
{
public:
	CanvasWindow(wxWindow *parent, int id, wxSize size);


	void ReloadData(void);

	void SetCanvasSize(wxSize size);

	void SetRootWindow(uiWindow *rootWindow) { m_RootWindow = rootWindow; }
	void SetTheme(uiTheme *theme) { m_Theme = theme; SetCanvasSize(theme->m_Size); }

	void SetActiveWindow(uiWindow *activeWindow) { m_ActiveWindow = activeWindow; }
	void RefreshSelection();

    /** \brief Starting sequence of adding new item by pointing with mouse */
    void InsertByPointing(void);

    uiWindow*  GetSubOrMainWindow(void) { return m_MouseSelectedContainer; }

    wxRect GetDragingWindowRect(void);

protected:

	void OnEraseBackground(wxEraseEvent &event);
	void OnPaint(wxPaintEvent &event);

	void OnCanvasClicked(void);
	void OnCanvasChanged(void);
	void OnCanvasWinRectUpdate(void);
	void OnCanvasCancelDrag(void);


private:
    /** \brief Enum type describing placement of drag box */
    enum DragBoxType
    {
        LeftTop = 0,
        Top,
        RightTop,
        Left,
        Right,
        LeftBtm,
        Btm,
        RightBtm,
        /*************/
        DragBoxTypeCnt
    };

    /** \brief enum for available mouse states */
    enum MouseStatesT
    {
        msIdle,
        msDraggingPointInit,
        msDraggingPoint,
        msDraggingItemInit,
        msDraggingItem,
        msTargetSearch,
        msWaitForIdle,
    };

    /** \brief Structure describing one dragging point */
    struct DragPointData
    {
//        wxsItem* Item;                                  ///< \brief Associated item
        uiWindow* Item;
        DragBoxType Type;                               ///< \brief Type of this drag box
        bool Grey;                                      ///< \brief If true, this drag point will be drawn grey
        int PosX;                                       ///< \brief X position of this drag point
        int PosY;                                       ///< \brief Y position of this drag point
        int DragInitPosX;                               ///< \brief X position before dragging
        int DragInitPosY;                               ///< \brief Y position before dragging
        DragPointData* ItemPoints[DragBoxTypeCnt];      ///< \brief Pointers to all drag points for this item
    };

    /** \brief Declaration of vector containing all drag points */
    WX_DEFINE_ARRAY(DragPointData*,DragPointsT);

    MouseStatesT m_MouseState;                          ///< \brief Current mouse state
    DragPointData* m_CurDragPoint;                      ///< \brief Dragged drag point
    int            m_DragInitPosX;                      ///< \brief Initial mouse x position when dragging
    int            m_DragInitPosY;                      ///< \brief Initial mouse y position when dragging
    static const int m_MinDragDistance = 8;             ///< \brief Minimal distance which must be done to apply dragging

    /** \brief Processing mouse events */
    void OnMouse(wxMouseEvent& event);

    void OnMouseIdle(wxMouseEvent& event);
    void OnMouseDraggingPoint(wxMouseEvent& event);
    void OnMouseDraggingPointInit(wxMouseEvent& event);
    void OnMouseDraggingItem(wxMouseEvent& event);
    void OnMouseDraggingItemInit(wxMouseEvent& event);
    void OnMouseTargetSearch(wxMouseEvent& event);

    void MoveWindowByStep(int stepX, int stepY);
    void OnCanvasKeyUp(wxKeyEvent& event);


    void Draw(wxDC& dc);
    void PaintExtra(wxDC* DC);
    void FastRepaint();


    bool FindAbsolutePoint(uiWindow* window, int& PosX, int& PosY);
    uiWindow* FindWindowAtPos(int PosX,int PosY, uiWindow* SearchIn);
    bool FindContainerByWindow(uiWindow* window);

    bool GetRelativePoint(uiWindow* window, int& PosX, int& PosY);
    bool FindAbsoluteRect(uiWindow* window, int& PosX, int& PosY, int& SizeX, int& SizeY);
    void ClearDragPoints();
    void AddDragPoints(uiWindow *activeWindow);


    CanvasWindow::DragPointData* FindDragPointFromItem(uiWindow* Item);
    CanvasWindow::DragPointData* FindDragPointAtPos(int PosX,int PosY);
    inline void SetCur(wxStockCursor Cur) { SetCursor(wxCursor(Cur)); }

	uiWindow *m_RootWindow = nullptr;
	uiTheme  *m_Theme = nullptr;
	uiWindow *m_ActiveWindow = nullptr;
	uiWindow *m_MouseSelectedContainer = nullptr;

	int m_ToolControlChanged = 0;
	int m_SizeX;
	int m_SizeY;

	int m_ActualSizeX;		/* content width must be N * m_ScrollUnitX */
	int m_ActualSizeY;		/* content height must be N * m_ScrollUnitY */

	int m_ScrollUnitX = 10;
	int m_ScrollUnitY = 10;

	int m_BorderSize = 8;	/* add border for drag point */
	int m_DragBoxSize = 8;

	DragPointsT m_DragPoints;                           ///< \brief Array of visible drag points

	wxUint32 *m_CanvasContent = nullptr;

	wxRect *m_ActiveWindowRect = nullptr;

	wxDECLARE_EVENT_TABLE();
};



//-------------------------------------------------------------------------------------------------
// canvas event
//-------------------------------------------------------------------------------------------------
class CanvasEvent : public wxCommandEvent
{
public:
	CanvasEvent(wxEventType commandType = wxEVT_NULL, int id = 0) : wxCommandEvent(commandType, id) {}
	CanvasEvent(const CanvasEvent& event) : wxCommandEvent(event) {}

	virtual wxEvent* Clone(void) const { return new CanvasEvent(*this); }

	DECLARE_DYNAMIC_CLASS(CanvasEvent);
};

typedef void (wxEvtHandler::*CanvasEventHandlerFunction)(CanvasEvent&);


BEGIN_DECLARE_EVENT_TYPES()
	DECLARE_EVENT_TYPE(CANVAS_EVT_COMMAND_CLICKED, 801)
	DECLARE_EVENT_TYPE(CANVAS_EVT_COMMAND_CHANGED, 802)
	DECLARE_EVENT_TYPE(CANVAS_EVT_COMMAND_UPDATE_GRID, 803)
	DECLARE_EVENT_TYPE(CANVAS_EVT_COMMAND_CANCEL_DRAG, 804)
END_DECLARE_EVENT_TYPES()

#define EVT_CANVAS_CLICK(id, fn) DECLARE_EVENT_TABLE_ENTRY(CANVAS_EVT_COMMAND_CLICKED, id, -1, (wxObjectEventFunction)(wxEventFunction)(CanvasEventHandlerFunction)&fn, (wxObject*)NULL),
#define EVT_CANVAS_CHANGE(id, fn) DECLARE_EVENT_TABLE_ENTRY(CANVAS_EVT_COMMAND_CHANGED, id, -1, (wxObjectEventFunction)(wxEventFunction)(CanvasEventHandlerFunction)&fn, (wxObject*)NULL),
#define EVT_CANVAS_UPDATE_GRID(id, fn) DECLARE_EVENT_TABLE_ENTRY(CANVAS_EVT_COMMAND_UPDATE_GRID, id, -1, (wxObjectEventFunction)(wxEventFunction)(CanvasEventHandlerFunction)&fn, (wxObject*)NULL),
#define EVT_CANVAS_CANCEL_DRAG(id, fn) DECLARE_EVENT_TABLE_ENTRY(CANVAS_EVT_COMMAND_CANCEL_DRAG, id, -1, (wxObjectEventFunction)(wxEventFunction)(CanvasEventHandlerFunction)&fn, (wxObject*)NULL),


// ----------------------------------------------------------------------------
// buffering helper.
// ----------------------------------------------------------------------------
class BufferedDC : public wxBufferedDC
{
public:
    // Default ctor, must subsequently call Init for two stage construction.
    BufferedDC()
        : m_dc(NULL),
          m_buffer(NULL),
          m_style(0)
    {
    }

    // Construct a wxBufferedDC using a user supplied buffer.
    BufferedDC(wxDC *dc,
                 wxBitmap& buffer = wxNullBitmap,
                 int style = wxBUFFER_CLIENT_AREA)
        : m_dc(NULL), m_buffer(NULL)
    {
        Init(dc, buffer, style);
    }
    // Construct a wxBufferedDC with an internal buffer of 'area'
    // (where area is usually something like the size of the window
    // being buffered)
    BufferedDC(wxDC *dc, const wxSize& area, int style = wxBUFFER_CLIENT_AREA)
        : m_dc(NULL), m_buffer(NULL)
    {
        Init(dc, area, style);
    }

    // The usually desired  action in the dtor is to blit the buffer.
    virtual ~BufferedDC()
    {
        if ( m_dc )
            UnMask();
    }

   // These reimplement the actions of the ctors for two stage creation
    void Init(wxDC *dc,
              wxBitmap& buffer = wxNullBitmap,
              int style = wxBUFFER_CLIENT_AREA)
    {
        InitCommon(dc, style);

        m_buffer = &buffer;

        UseBuffer();
    }

    void Init(wxDC *dc, const wxSize &area, int style = wxBUFFER_CLIENT_AREA)
    {
        InitCommon(dc, style);

        UseBuffer(area.x, area.y);
    }

    // Blits the buffer to the dc, and detaches the dc from the buffer (so it
    // can be effectively used once only).
    //
    // Usually called in the dtor or by the dtor of derived classes if the
    // BufferedDC must blit before the derived class (which may own the dc it's
    // blitting to) is destroyed.
    void UnMask();

private:
    // common part of Init()s
    void InitCommon(wxDC *dc, int style)
    {
        wxASSERT_MSG( !m_dc, wxT("wxBufferedDC already initialised") );

        m_dc = dc;
        m_style = style;
    }

    // check that the bitmap is valid and use it
    void UseBuffer(wxCoord w = -1, wxCoord h = -1);

    // the underlying DC to which we copy everything drawn on this one in
    // UnMask()
    //
    // NB: Without the existence of a wxNullDC, this must be a pointer, else it
    //     could probably be a reference.
    wxDC *m_dc;

    // the buffer (selected in this DC), initially invalid
    wxBitmap *m_buffer;

    // the buffering style
    int m_style;

    wxSize m_area;
};


#endif /* MAIN_CANVAS_H_ */
