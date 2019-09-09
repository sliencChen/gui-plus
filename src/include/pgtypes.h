/*
 * pgtypes.h
 *
 *  Created on: 2019-4-22
 *      Author: Brant
 */

#ifndef _PG_TYPES_H_
#define _PG_TYPES_H_


#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>


//-------------------------------------------------------------------------------------------------
// RectPGProperty
//-------------------------------------------------------------------------------------------------
WX_PG_DECLARE_VARIANT_DATA_EXPORTED(wxRect,WXDLLIMPEXP_PROPGRID)

class RectPGProperty : public wxPGProperty
{
	WX_PG_DECLARE_PROPERTY_CLASS(RectPGProperty);

public:
	RectPGProperty(const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL, const wxRect& value = wxRect(), \
			const wxString& name_x = "X", const wxString& name_y = "Y", const wxString& name_widht = "Width", const wxString& name_height = "Height");
	virtual ~RectPGProperty();

	virtual wxVariant ChildChanged(wxVariant& thisValue, int childIndex, wxVariant& childValue) const wxOVERRIDE;
	virtual void RefreshChildren(void) wxOVERRIDE;

protected:
	void SetValueI(const wxRect& rect) { m_value = WXVARIANT(rect); }
};



//-------------------------------------------------------------------------------------------------
// ColorPGProperty
//-------------------------------------------------------------------------------------------------
class ColorPGProperty : public wxSystemColourProperty
{
public:
	ColorPGProperty(const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL, const wxColourPropertyValue& value = wxColourPropertyValue());
	~ColorPGProperty(void);

	wxColour GetColour(int index) const;
    wxSize OnMeasureImage(int item) const wxOVERRIDE;
    void OnCustomPaint(wxDC& dc, const wxRect& rect, wxPGPaintData& paintdata) wxOVERRIDE;
};


//-------------------------------------------------------------------------------------------------
// SizeProperty
//-------------------------------------------------------------------------------------------------
class SizeProperty : public wxPGProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS(SizeProperty)
public:

	SizeProperty( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL,
					const wxSize& value = wxSize(), const wxString& first_name = "Width", const wxString& second_name = "Height");
    virtual ~SizeProperty();

    virtual wxVariant ChildChanged( wxVariant& thisValue,
                                    int childIndex,
                                    wxVariant& childValue ) const wxOVERRIDE;
    virtual void RefreshChildren() wxOVERRIDE;

protected:

    // I stands for internal
    void SetValueI( const wxSize& value )
    {
        m_value = WXVARIANT(value);
    }
};


//-------------------------------------------------------------------------------------------------
// PointProperty
//-------------------------------------------------------------------------------------------------
class PointProperty : public wxPGProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS(PointProperty)
public:

    PointProperty( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL,
                     const wxPoint& value = wxPoint() );
    virtual ~PointProperty();

    virtual wxVariant ChildChanged( wxVariant& thisValue,
                                    int childIndex,
                                    wxVariant& childValue ) const wxOVERRIDE;
    virtual void RefreshChildren() wxOVERRIDE;

protected:

    // I stands for internal
    void SetValueI( const wxPoint& value )
    {
        m_value = WXVARIANT(value);
    }
};


#endif /* _PG_TYPES_H_ */
