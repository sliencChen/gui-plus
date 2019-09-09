/*
 * maincanvas.cpp
 *
 *  Created on: 2019-3-17
 *      Author: Brant
 */

#include <string.h>
#include <wx/dcbuffer.h>
#include "app_global.h"
#include "maincanvas.h"


DEFINE_EVENT_TYPE(CANVAS_EVT_COMMAND_CLICKED)
DEFINE_EVENT_TYPE(CANVAS_EVT_COMMAND_CHANGED)
DEFINE_EVENT_TYPE(CANVAS_EVT_COMMAND_UPDATE_GRID)
DEFINE_EVENT_TYPE(CANVAS_EVT_COMMAND_CANCEL_DRAG)
IMPLEMENT_DYNAMIC_CLASS(CanvasEvent, wxCommandEvent)



wxBEGIN_EVENT_TABLE(CanvasWindow, wxScrolledWindow)

	EVT_ERASE_BACKGROUND(CanvasWindow::OnEraseBackground)
	EVT_PAINT(CanvasWindow::OnPaint)

	EVT_MOUSE_EVENTS(CanvasWindow::OnMouse)
	EVT_KEY_UP(CanvasWindow::OnCanvasKeyUp)

wxEND_EVENT_TABLE()


CanvasWindow::CanvasWindow(wxWindow *parent, int id, wxSize size)
			: wxScrolledWindow(parent, id, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL | wxNO_FULL_REPAINT_ON_RESIZE)
{
    m_RootWindow = nullptr;
    if(m_ActiveWindowRect == nullptr)
    {
        m_ActiveWindowRect = new wxRect();
    }
    SetCanvasSize(size);
}

/******************************************************************************
*  public
******************************************************************************/
void CanvasWindow::ReloadData(void)
{
	if (m_RootWindow)
	{
		uiRender::GetBitmap(m_RootWindow, m_CanvasContent, m_SizeX, m_SizeY);
	}
	else
	{
		memset(m_CanvasContent, 0xff, m_SizeX * m_SizeY * sizeof(unsigned int));
	}
	Refresh();
}


void CanvasWindow::SetCanvasSize(wxSize size)
{
	int newSizeX, newSizeY;

//	newSizeX = FromDIP(size.GetWidth());
	newSizeX = size.GetWidth();
//	newSizeY = FromDIP(size.GetHeight());
	newSizeY = size.GetHeight();

	if (m_SizeX == newSizeX && m_SizeY == newSizeY)
		return;

	m_SizeX = newSizeX;
	m_SizeY = newSizeY;

	delete m_CanvasContent;
    m_CanvasContent = new unsigned int[m_SizeX * m_SizeY];
    memset(m_CanvasContent, 0xff, m_SizeX * m_SizeY * sizeof(unsigned int));

    m_ScrollUnitX = wxMax(m_ScrollUnitX, 1);
    m_ScrollUnitY = wxMax(m_ScrollUnitY, 1);

    m_ActualSizeX = m_SizeX + m_BorderSize * 2;
    m_ActualSizeY = m_SizeY + m_BorderSize * 2;

    m_ActualSizeX = (m_ActualSizeX + m_ScrollUnitX - 1) / m_ScrollUnitX * m_ScrollUnitX;
    m_ActualSizeY = (m_ActualSizeY + m_ScrollUnitY - 1) / m_ScrollUnitY * m_ScrollUnitY;

    SetScrollbars(m_ScrollUnitX, m_ScrollUnitY, m_ActualSizeX / m_ScrollUnitX, m_ActualSizeY / m_ScrollUnitY);

	Refresh();
}

void CanvasWindow::RefreshSelection()
{
	ClearDragPoints();
	AddDragPoints(m_ActiveWindow);

	Refresh();
}


void CanvasWindow::InsertByPointing(void)
{
    m_ToolControlChanged = 1;
}


wxRect CanvasWindow::GetDragingWindowRect(void)
{
    DragPointData* leftTop = m_CurDragPoint->ItemPoints[LeftTop];
    DragPointData* rightBtm = m_CurDragPoint->ItemPoints[RightBtm];

    m_ActiveWindowRect->x = leftTop->PosX;
    m_ActiveWindowRect->y = leftTop->PosY;
    m_ActiveWindowRect->width = rightBtm->PosX - leftTop->PosX;
    m_ActiveWindowRect->height = rightBtm->PosY - leftTop->PosY;

    GetRelativePoint(m_ActiveWindow, m_ActiveWindowRect->x, m_ActiveWindowRect->y);

    return *m_ActiveWindowRect;
}


/******************************************************************************
*  draw
******************************************************************************/
void CanvasWindow::OnEraseBackground(wxEraseEvent &event)
{
	wxDC *dc = event.GetDC();

	dc->SetPen(wxPen(wxTransparentColor, 0, wxPENSTYLE_TRANSPARENT));
	dc->SetBrush(wxColour(210, 210, 210));
	dc->DrawRectangle(0, 0, GetSize().GetWidth(), GetSize().GetHeight());
}

void CanvasWindow::OnPaint(wxPaintEvent &event)
{
	wxPaintDC pdc(this);

	PrepareDC(pdc);
	Draw(pdc);
}

void CanvasWindow::Draw(wxDC& dc)
{
    wxBitmap content((const char *)m_CanvasContent, m_SizeX, m_SizeY, 32);
	dc.DrawBitmap(content, m_BorderSize, m_BorderSize);
	dc.SetPen(wxPen(wxColour(170, 170, 170)));
	dc.SetBrush(wxBrush(wxColour(170, 170, 170)));
	dc.DrawRectangle(m_BorderSize + m_SizeX, m_BorderSize * 2, m_BorderSize, m_SizeY - m_BorderSize);
	dc.DrawRectangle(m_BorderSize * 2, m_BorderSize + m_SizeY, m_SizeX, m_BorderSize);

	for (size_t i = m_DragPoints.Count(); i-- > 0;)
    {
        DragPointData* DPD = m_DragPoints[i];
        if ( DPD->Grey )
        {
        	dc.SetPen(*wxGREY_PEN);
        	dc.SetBrush(*wxGREY_BRUSH);
        }
        else
        {
        	dc.SetPen(*wxBLACK_PEN);
        	dc.SetBrush(*wxRED_BRUSH);
        }
        int PosX = DPD->PosX - m_BorderSize/2;
        int PosY = DPD->PosY - m_BorderSize/2;
		dc.DrawRectangle(PosX, PosY, m_BorderSize, m_BorderSize);
    }
}


void CanvasWindow::PaintExtra(wxDC* DC)
{
    for ( size_t i = m_DragPoints.Count(); i-- > 0; )
    {
        DragPointData* DPD = m_DragPoints[i];
        if ( DPD->Grey )
        {
            DC->SetPen(*wxGREY_PEN);
            DC->SetBrush(*wxGREY_BRUSH);
        }
        else
        {
            DC->SetPen(*wxBLACK_PEN);
            DC->SetBrush(*wxRED_BRUSH);
        }
        int PosX = DPD->PosX - m_BorderSize/2 - m_BorderSize;
        int PosY = DPD->PosY - m_BorderSize/2 - m_BorderSize;
        DC->DrawRectangle(PosX , PosY, m_BorderSize, m_BorderSize );
    }
}

void CanvasWindow::FastRepaint()
{
    wxClientDC ClientDC(this);
    PrepareDC(ClientDC);

    wxBitmap content((const char *)m_CanvasContent, m_SizeX, m_SizeY, 32);
    BufferedDC DC(&ClientDC, content, wxBUFFER_VIRTUAL_AREA);

    PaintExtra(&DC);
}

/******************************************************************************
*  canvas events
******************************************************************************/
void CanvasWindow::OnCanvasClicked(void)
{
	CanvasEvent canvasEvt(CANVAS_EVT_COMMAND_CLICKED, GetId());
	canvasEvt.SetEventObject(this);
	GetEventHandler()->ProcessEvent(canvasEvt);
}


void CanvasWindow::OnCanvasChanged(void)
{
	CanvasEvent canvasEvt(CANVAS_EVT_COMMAND_CHANGED, GetId());
	canvasEvt.SetEventObject(this);
	GetEventHandler()->ProcessEvent(canvasEvt);
}

void CanvasWindow::OnCanvasWinRectUpdate(void)
{
	CanvasEvent canvasEvt(CANVAS_EVT_COMMAND_UPDATE_GRID, GetId());
	canvasEvt.SetEventObject(this);
	GetEventHandler()->ProcessEvent(canvasEvt);
}

void CanvasWindow::OnCanvasCancelDrag(void)
{
	CanvasEvent canvasEvt(CANVAS_EVT_COMMAND_CANCEL_DRAG, GetId());
	canvasEvt.SetEventObject(this);
	GetEventHandler()->ProcessEvent(canvasEvt);
}

/******************************************************************************
*  mouse events
******************************************************************************/
void CanvasWindow::OnMouse(wxMouseEvent& event)
{
    if( !m_RootWindow ) return;

	static bool IsRunning = false;
    if ( IsRunning ) return;
    IsRunning = true;

    if ( event.ButtonDown() )
    {
        SetFocus();
    }
    else if ( m_MouseState == msWaitForIdle )
    {
        m_MouseState = msIdle;
    }

    int NewX = event.m_x;
    int NewY = event.m_y;
    CalcUnscrolledPosition(NewX,NewY,&NewX,&NewY);
    event.m_x = NewX;
    event.m_y = NewY;
    switch ( m_MouseState )
    {
        case msDraggingPointInit: OnMouseDraggingPointInit (event); break;
        case msDraggingPoint:     OnMouseDraggingPoint     (event); break;
        case msDraggingItemInit:  OnMouseDraggingItemInit  (event); break;
        case msDraggingItem:      OnMouseDraggingItem      (event); break;
        case msTargetSearch:      OnMouseTargetSearch      (event); break;
        case msWaitForIdle:                                         break;
        case msIdle: // fall-through
        default:                  OnMouseIdle              (event); break;
    }

    IsRunning = false;
}

void CanvasWindow::OnMouseIdle(wxMouseEvent& event)
{
//    BlockFetch(false);
    m_DragInitPosX = event.GetX();
    m_DragInitPosY = event.GetY();

    int MouseX = event.GetX();
    int MouseY = event.GetY();

    uiWindow* OnCursor = FindWindowAtPos(MouseX, MouseY, m_RootWindow);
    if ( !OnCursor ) OnCursor = m_RootWindow;

    FindContainerByWindow(OnCursor);

    if (m_ToolControlChanged && event.RightDown())
    {
        m_ToolControlChanged = 0;
        OnCanvasCancelDrag();
    }

    if ( event.LeftDown() && !event.LeftDClick() && !event.RightIsDown() && !event.MiddleIsDown() )
    {
        if (m_ToolControlChanged)
        {
            m_MouseState = msTargetSearch;
            return;
        }

        DragPointData* DPD = FindDragPointAtPos(MouseX,MouseY);

        if ( DPD )
        {
            // If there's drag point, starting point-dragging sequence
            m_CurDragPoint = DPD;
            m_MouseState = msDraggingPointInit;
        }
        else
        {
//            wxLogMessage("LeftDown and OnCursor=[%s]", OnCursor->Id());
            m_CurDragPoint = FindDragPointFromItem(OnCursor);
            m_MouseState = msDraggingItemInit;

            if (( !m_CurDragPoint )|| (OnCursor != m_ActiveWindow ))
            {
                // If we're here, preview has probably not been updated yet
//                wxLogMessage("m_CurDragPoint null and OnCursor != m_ActiveWindow");
                m_MouseState = msWaitForIdle;
            }
        }
    }

    if ( !event.LeftIsDown() && !event.RightIsDown() && !event.MiddleIsDown() )
    {
        // Updating cursor

        DragPointData* DPD = FindDragPointAtPos(event.GetX(),event.GetY());

        if ( DPD )
        {
            switch ( DPD->Type )
            {
                case LeftTop:
                case RightBtm:
                    SetCur(wxCURSOR_SIZENWSE);
                    break;

                case Top:
                case Btm:
                    SetCur(wxCURSOR_SIZENS);
                    break;

                case RightTop:
                case LeftBtm:
                    SetCur(wxCURSOR_SIZENESW);
                    break;

                case Left:
                case Right:
                    SetCur(wxCURSOR_SIZEWE);
                    break;

                case DragBoxTypeCnt: // fall-through
                default:
                    SetCur(wxCURSOR_ARROW);
            }
        }
        else
        {
            SetCur(wxCURSOR_ARROW);
        }

    }
}

void CanvasWindow::OnMouseDraggingPointInit(wxMouseEvent& event)
{
//    BlockFetch(true);
//
    if ( event.RightIsDown() || event.MiddleIsDown() || !event.LeftIsDown() )
    {
        m_MouseState = msIdle;
        return;
    }

    int DeltaX = event.GetX() - m_DragInitPosX;
    if ( DeltaX<0 ) DeltaX = -DeltaX;
    int DeltaY = event.GetY() - m_DragInitPosY;
    if ( DeltaY<0 ) DeltaY = -DeltaY;

    if ( DeltaX + DeltaY > m_MinDragDistance )
    {
        m_MouseState = msDraggingPoint;
    }
}

void CanvasWindow::OnMouseDraggingPoint(wxMouseEvent& event)
{
    if ( event.RightIsDown() || event.MiddleIsDown() )
    {
        // Cancelling change
        for ( size_t i=0; i<m_DragPoints.Count(); i++ )
        {
            m_DragPoints[i]->PosX = m_DragPoints[i]->DragInitPosX;
            m_DragPoints[i]->PosY = m_DragPoints[i]->DragInitPosY;
        }
        m_MouseState = msIdle;
        OnCanvasWinRectUpdate();
        FastRepaint();
        return;
    }

    if ( !event.LeftIsDown() )
    {
        if ( m_ActiveWindow )
        {
            DragPointData* leftTop = m_CurDragPoint->ItemPoints[LeftTop];
            DragPointData* rightBtm = m_CurDragPoint->ItemPoints[RightBtm];
            int OldPosX = leftTop->DragInitPosX;
            int OldPosY = leftTop->DragInitPosY;
            int OldSizeX = rightBtm->DragInitPosX - OldPosX;
            int OldSizeY = rightBtm->DragInitPosY - OldPosY;
            int NewPosX = leftTop->PosX;
            int NewPosY = leftTop->PosY;
            int NewSizeX = rightBtm->PosX - NewPosX;
            int NewSizeY = rightBtm->PosY - NewPosY;

            if ( NewSizeX < 0 )
            {
                NewPosX += NewSizeX;
                NewSizeX = -NewSizeX;
            }

            if ( NewSizeY < 0 )
            {
                NewPosY += NewSizeY;
                NewSizeY = -NewSizeY;
            }

            if ( NewPosX!=OldPosX || NewPosY!=OldPosY || NewSizeX!=OldSizeX || NewSizeY!=OldSizeY )
            {
                GetRelativePoint(m_ActiveWindow, NewPosX, NewPosY);
//                wxLogMessage("NewPosX=%d, NewPosY=%d, NewSizeX=%d, NewSizeY=%d", NewPosX,NewPosY,NewSizeX,NewSizeY);
                wxRect rect(NewPosX, NewPosY, NewSizeX, NewSizeY);
                m_ActiveWindow->SetProperty(PROP_POSITION, rect);
                OnCanvasChanged();
            }
        }

        m_MouseState = msIdle;
        return;
    }

    int DeltaX = event.GetX() - m_DragInitPosX;
    int DeltaY = event.GetY() - m_DragInitPosY;

    DragPointData* leftTop = m_CurDragPoint->ItemPoints[LeftTop];
    DragPointData* rightBtm = m_CurDragPoint->ItemPoints[RightBtm];

    switch ( m_CurDragPoint->Type )
    {
        case LeftTop:
            leftTop->PosX = leftTop->DragInitPosX + DeltaX;
            leftTop->PosY = leftTop->DragInitPosY + DeltaY;
            break;

        case Top:
            leftTop->PosY = leftTop->DragInitPosY + DeltaY;
            break;

        case RightTop:
            rightBtm->PosX = rightBtm->DragInitPosX + DeltaX;
            leftTop->PosY = leftTop->DragInitPosY + DeltaY;
            break;

        case Left:
            leftTop->PosX = leftTop->DragInitPosX + DeltaX;
            break;

        case Right:
            rightBtm->PosX = rightBtm->DragInitPosX + DeltaX;
            break;

        case LeftBtm:
            leftTop->PosX = leftTop->DragInitPosX + DeltaX;
            rightBtm->PosY = rightBtm->DragInitPosY + DeltaY;
            break;

        case Btm:
            rightBtm->PosY = rightBtm->DragInitPosY + DeltaY;
            break;

        case RightBtm:
            rightBtm->PosX = rightBtm->DragInitPosX + DeltaX;
            rightBtm->PosY = rightBtm->DragInitPosY + DeltaY;
            break;

        case DragBoxTypeCnt: // fall-through
        default:
            break;
    }

    int LX = leftTop->PosX;
    int LY = leftTop->PosY;
    int RX = rightBtm->PosX;
    int RY = rightBtm->PosY;

    DragPointData** ItemPoints = leftTop->ItemPoints;

    ItemPoints[Top]->PosX = (LX+RX)/2;
    ItemPoints[Top]->PosY = LY;
    ItemPoints[RightTop]->PosX = RX;
    ItemPoints[RightTop]->PosY = LY;
    ItemPoints[Left]->PosX = LX;
    ItemPoints[Left]->PosY = (LY+RY) / 2;
    ItemPoints[Right]->PosX = RX;
    ItemPoints[Right]->PosY = (LY+RY) / 2;
    ItemPoints[LeftBtm]->PosX = LX;
    ItemPoints[LeftBtm]->PosY = RY;
    ItemPoints[Btm]->PosX = (LX+RX)/2;
    ItemPoints[Btm]->PosY = RY;

    OnCanvasWinRectUpdate();
    FastRepaint();
}

void CanvasWindow::OnMouseDraggingItemInit(wxMouseEvent& event)
{
//    BlockFetch(true);
//
    if ( event.RightIsDown() || event.MiddleIsDown() || !event.LeftIsDown() )
    {
        m_MouseState = msIdle;
        return;
    }

    int DeltaX = event.GetX() - m_DragInitPosX;
    if ( DeltaX<0 ) DeltaX = -DeltaX;
    int DeltaY = event.GetY() - m_DragInitPosY;
    if ( DeltaY<0 ) DeltaY = -DeltaY;

    if ( DeltaX + DeltaY > m_MinDragDistance )
    {
        m_MouseState = msDraggingItem;
        SetCur(wxCURSOR_SIZING);
    }
}

void CanvasWindow::OnMouseDraggingItem(wxMouseEvent& event)
{
    if ( event.RightIsDown() || event.MiddleIsDown() )
    {
        // Cancelling change
        for ( size_t i=0; i<m_DragPoints.Count(); i++ )
        {
            m_DragPoints[i]->PosX = m_DragPoints[i]->DragInitPosX;
            m_DragPoints[i]->PosY = m_DragPoints[i]->DragInitPosY;
        }
        m_MouseState = msIdle;
        OnCanvasWinRectUpdate();
        FastRepaint();
        return;
    }
//
    if ( !event.LeftIsDown() )
    {
        if ( !m_CurDragPoint )
        {
            // TODO: Enable this anti-crash check after tests
//            return;
        }


        if ( m_CurDragPoint->PosX != m_CurDragPoint->DragInitPosX ||
             m_CurDragPoint->PosY != m_CurDragPoint->DragInitPosY )
        {
            PropertyRect *rect = (PropertyRect *)m_ActiveWindow->m_Properties.find(PROP_POSITION)->second;
            if (!rect)
                return ;

            GetRelativePoint(m_ActiveWindow, m_CurDragPoint->PosX, m_CurDragPoint->PosY);
//            wxLogMessage("item NewPosX=%d, NewPosY=%d, NewSizeX=%d, NewSizeY=%d", m_CurDragPoint->PosX,m_CurDragPoint->PosY,rect->GetWidth(),rect->GetHeight());
            wxRect temp_rect(m_CurDragPoint->PosX, m_CurDragPoint->PosY, rect->GetWidth(), rect->GetHeight());
            m_ActiveWindow->SetProperty(PROP_POSITION, temp_rect);
            OnCanvasChanged();
        }

        m_MouseState = msIdle;
        return;
    }

    int DeltaX = event.GetX() - m_DragInitPosX;
    int DeltaY = event.GetY() - m_DragInitPosY;

    for ( size_t i=0; i<m_DragPoints.Count(); i++ )
    {
        m_DragPoints[i]->PosX = m_DragPoints[i]->DragInitPosX + DeltaX;
        m_DragPoints[i]->PosY = m_DragPoints[i]->DragInitPosY + DeltaY;
    }

    OnCanvasWinRectUpdate();
    FastRepaint();
}

void CanvasWindow::OnMouseTargetSearch(wxMouseEvent& event)
{
//    if ( event.RightDown() )
//    {
//        m_MouseState = msIdle;
//        OnCanvasCancelDrag();
//        return;
//    }

    //if ( event.LeftDown() )
    //{
        OnCanvasClicked();
        m_MouseState = msIdle;
        m_ToolControlChanged = 0;
    //}
}


/******************************************************************************
*  window
******************************************************************************/
bool CanvasWindow::GetRelativePoint(uiWindow* window, int& PosX, int& PosY)
{
    if (!window)
    	return false;

	uiWindow* parent = window->m_Parent;
	while(parent)
    {
        PropertyRect *rect = (PropertyRect *)parent->m_Properties.find(PROP_POSITION)->second;
        if (!rect)
            return false;
//        wxLogMessage("GetX=%d, GetY=%d", rect->GetX(), rect->GetY());
        PosX -= rect->GetX();
        PosY -= rect->GetY();
        parent = parent->m_Parent;
    }

    PosX -= m_BorderSize;
    PosY -= m_BorderSize;

    return true;
}

bool CanvasWindow::FindAbsoluteRect(uiWindow* window, int& PosX, int& PosY, int& SizeX, int& SizeY)
{
	if (!window)
    	return false;

	PropertyRect *rect = (PropertyRect *)window->m_Properties.find(PROP_POSITION)->second;
	if (!rect)
		return false;

    PosX = rect->GetX();
    PosY = rect->GetY();
    SizeX = rect->GetWidth();
    SizeY = rect->GetHeight();

    return true;
}


uiWindow* CanvasWindow::FindWindowAtPos(int PosX,int PosY, uiWindow* SearchIn)
{
    int itemPosX;
    int itemPosY;
    int itemSizeX;
    int itemSizeY;

    uiWindow *find_node = SearchIn;

    if ( !FindAbsoluteRect(SearchIn,itemPosX,itemPosY,itemSizeX,itemSizeY) ) return nullptr;

    if ( PosX < itemPosX ) return nullptr;
    if ( PosX >= (itemPosX+itemSizeX) ) return nullptr;
    if ( PosY < itemPosY ) return nullptr;
    if ( PosY >= (itemPosY+itemSizeY) ) return nullptr;

    if(find_node == m_ActiveWindow)
    {
        return find_node;
    }

    uiWindow *node = SearchIn->m_Childs;
    while(node)
    {
        uiWindow* f = FindWindowAtPos(PosX, PosY, node);
        if ( f )
        {
            find_node = f;
            if(find_node == m_ActiveWindow)
            {
                //wxLogMessage("m_ActiveWindow id = %s  back", m_ActiveWindow->Id());
                break;
            }
        }

        node = node->m_Next;
    }
//    wxLogMessage("find window id = %s", SearchIn->Id());
    return find_node;
}


bool CanvasWindow::FindContainerByWindow(uiWindow* window)
{
    if (!window)
    	return false;

    int type = window->GetWindowType();
//    wxLogMessage("type->%d", type);
    if ((type == WINDOW_TYPE_SUB)||(type == WINDOW_TYPE_SIZER))
    {
//        wxLogMessage(" window id = %s", window->Id());
        m_MouseSelectedContainer = window;
        return true;
    }


    if (type == WINDOW_TYPE_CONTROL)
    {
        uiWindow* parent = window->m_Parent;
        while(parent)
        {
            if(FindContainerByWindow(parent))
            {
                return true;
            }
            parent = parent->m_Parent;
        }
    }

    return false;
}


/******************************************************************************
*  drag point
******************************************************************************/
CanvasWindow::DragPointData* CanvasWindow::FindDragPointAtPos(int PosX,int PosY)
{
    for ( size_t i=m_DragPoints.Count(); i-->0; )
    {
        DragPointData* DPD = m_DragPoints[i];
        int dpx = DPD->PosX - (m_DragBoxSize/2);
        int dpy = DPD->PosY - (m_DragBoxSize/2);

        if ( (PosX >= dpx) && (PosX < dpx+m_DragBoxSize) &&
             (PosY >= dpy) && (PosY < dpy+m_DragBoxSize) )
        {
            return DPD;
        }
    }

// TODO (SpOoN#1#): Search for edges

    return 0;
}

CanvasWindow::DragPointData* CanvasWindow::FindDragPointFromItem(uiWindow* Item)
{
    for ( size_t i = 0; i<m_DragPoints.Count(); i+= 8 )
    {
        if ( m_DragPoints[i]->Item == Item )
        {
            return m_DragPoints[i];
        }
    }
    return 0;
}

void CanvasWindow::ClearDragPoints()
{
	for (size_t i = m_DragPoints.Count(); i-- > 0;)
    {
        delete m_DragPoints[i];
    }
    m_DragPoints.Clear();
}

void CanvasWindow::AddDragPoints(uiWindow *activeWindow)
{
    int PosX, PosY;
    int SizeX, SizeY;

    if (FindAbsoluteRect(activeWindow, PosX, PosY, SizeX, SizeY))
    {
    	PosX += m_BorderSize;
    	PosY += m_BorderSize;

        bool Grey = false;//Item!=RootSelection;
        DragPointData* ItemPoints[DragBoxTypeCnt];

		for (int i = 0; i < DragBoxTypeCnt; ++i)
        {
            ItemPoints[i] = new DragPointData;
            ItemPoints[i]->Grey = Grey;
            ItemPoints[i]->PosX = PosX;
            ItemPoints[i]->PosY = PosY;
            ItemPoints[i]->Item = activeWindow;
            ItemPoints[i]->Type = (DragBoxType)i;

            if ( i == Top || i == Btm )
            {
                ItemPoints[i]->PosX += SizeX / 2;
            }
            else if ( i == RightTop || i == Right || i == RightBtm )
            {
                ItemPoints[i]->PosX += SizeX;
            }

            if ( i==Left || i == Right )
            {
                ItemPoints[i]->PosY += SizeY / 2;
            }
            else if ( i == LeftBtm || i == Btm || i == RightBtm )
            {
                ItemPoints[i]->PosY += SizeY;
            }

            ItemPoints[i]->DragInitPosX = ItemPoints[i]->PosX;
            ItemPoints[i]->DragInitPosY = ItemPoints[i]->PosY;
        }

		for (int i = 0; i < DragBoxTypeCnt; ++i)
        {
            memcpy(ItemPoints[i]->ItemPoints,ItemPoints,sizeof(ItemPoints[0]->ItemPoints));
            m_DragPoints.Add(ItemPoints[i]);
        }
    }
}


/******************************************************************************
*  key event
******************************************************************************/
void CanvasWindow::MoveWindowByStep(int stepX, int stepY)
{
    if ((stepX == 0 ) && (stepY == 0))
    {
        return;
    }

    for ( size_t i=0; i<m_DragPoints.Count(); i++ )
    {
        m_DragPoints[i]->PosX = m_DragPoints[i]->DragInitPosX + stepX;
        m_DragPoints[i]->PosY = m_DragPoints[i]->DragInitPosY + stepY;
    }

    PropertyRect *rect = (PropertyRect *)m_ActiveWindow->m_Properties.find(PROP_POSITION)->second;
    if (!rect)
        return ;

    GetRelativePoint(m_ActiveWindow, m_CurDragPoint->PosX, m_CurDragPoint->PosY);
    wxRect temp_rect(m_CurDragPoint->PosX, m_CurDragPoint->PosY, rect->GetWidth(), rect->GetHeight());
    m_ActiveWindow->SetProperty(PROP_POSITION, temp_rect);
    OnCanvasChanged();
}

void CanvasWindow::OnCanvasKeyUp(wxKeyEvent& event)
{
    wxString key;
    long keycode = event.GetKeyCode();
    m_CurDragPoint = FindDragPointFromItem(m_ActiveWindow);
    int move_x=0 , move_y =0;

    switch ( keycode )
    {
        case WXK_LEFT:
            key = "LEFT";
            move_x = -1;
            break;
        case WXK_UP:
            key = "UP";
            move_y = -1;
            break;
        case WXK_RIGHT:
            key = "RIGHT";
            move_x = 1;
            break;
        case WXK_DOWN:
            key = "DOWN";
            move_y = 1;
            break;

        default:
            key = "NULL";
        break;
    }

    MoveWindowByStep(move_x, move_y);
}

// ----------------------------------------------------------------------------
// SharedDCBufferManager: helper class maintaining backing store bitmap
// ----------------------------------------------------------------------------
class SharedDCBufferManager : public wxModule
{
public:
    SharedDCBufferManager() { }

    virtual bool OnInit() wxOVERRIDE { return true; }
    virtual void OnExit() wxOVERRIDE { wxDELETE(ms_buffer); }

    static wxBitmap* GetBuffer(wxDC* dc, int w, int h)
    {
        if ( ms_usingSharedBuffer )
            return DoCreateBuffer(dc, w, h);

        if ( !ms_buffer ||
                w > ms_buffer->GetScaledWidth() ||
                    h > ms_buffer->GetScaledHeight() )
        {
            delete ms_buffer;

            ms_buffer = DoCreateBuffer(dc, w, h);
        }

        ms_usingSharedBuffer = true;
        return ms_buffer;
    }

    static void ReleaseBuffer(wxBitmap* buffer)
    {
        if ( buffer == ms_buffer )
        {
            wxASSERT_MSG( ms_usingSharedBuffer, wxT("shared buffer already released") );
            ms_usingSharedBuffer = false;
        }
        else
        {
            delete buffer;
        }
    }

private:
    static wxBitmap* DoCreateBuffer(wxDC* dc, int w, int h)
    {
        const double scale = dc ? dc->GetContentScaleFactor() : 1.0;
        wxBitmap* const buffer = new wxBitmap;

        // we must always return a valid bitmap but creating a bitmap of
        // size 0 would fail, so create a 1*1 bitmap in this case
        buffer->CreateScaled(wxMax(w, 1), wxMax(h, 1), -1, scale);

        return buffer;
    }

    static wxBitmap *ms_buffer;
    static bool ms_usingSharedBuffer;

};

wxBitmap* SharedDCBufferManager::ms_buffer = NULL;
bool SharedDCBufferManager::ms_usingSharedBuffer = false;


// ============================================================================
// BufferedDC
// ============================================================================
void BufferedDC::UseBuffer(wxCoord w, wxCoord h)
{
    wxCHECK_RET( w >= -1 && h >= -1, "Invalid buffer size" );

    if ( !m_buffer || !m_buffer->IsOk() )
    {
        if ( w == -1 || h == -1 )
            m_dc->GetSize(&w, &h);

        m_buffer = SharedDCBufferManager::GetBuffer(m_dc, w, h);
        m_style |= wxBUFFER_USES_SHARED_BUFFER;
        m_area.Set(w,h);
    }
    else
        m_area = m_buffer->GetSize();

    SelectObject(*m_buffer);

    // now that the DC is valid we can inherit the attributes (fonts, colours,
    // layout direction, ...) from the original DC
    if ( m_dc && m_dc->IsOk() )
        CopyAttributes(*m_dc);
}

void BufferedDC::UnMask()
{
    wxCHECK_RET( m_dc, wxT("no underlying wxDC?") );
    wxASSERT_MSG( m_buffer && m_buffer->IsOk(), wxT("invalid backing store") );

    wxCoord x = 8,
            y = 8;

    // Ensure the scale matches the device
    SetUserScale(1.0, 1.0);

    if ( m_style & wxBUFFER_CLIENT_AREA )
        GetDeviceOrigin(&x, &y);

    // It's possible that the buffer may be bigger than the area that needs to
    // be drawn (the client size of the window is smaller than the bitmap, or
    // a shared bitmap has been reused for a smaller area, etc.) so avoid
    // blitting too much if possible, but only use the real DC size if the
    // wxBUFFER_VIRTUAL_AREA style is not set.
    int width = m_area.GetWidth(),
        height = m_area.GetHeight();

    if (!(m_style & wxBUFFER_VIRTUAL_AREA))
    {
        int widthDC,
            heightDC;
        m_dc->GetSize(&widthDC, &heightDC);
        width = wxMin(width, widthDC);
        height = wxMin(height, heightDC);
    }

    const wxPoint origin = GetLogicalOrigin();
    m_dc->Blit(-origin.x, -origin.y, width, height, this, -x, -y);
    m_dc = NULL;

    if ( m_style & wxBUFFER_USES_SHARED_BUFFER )
        SharedDCBufferManager::ReleaseBuffer(m_buffer);
}
