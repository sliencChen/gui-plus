/*
 * pgtypes.cpp
 *
 *  Created on: 2019-4-22
 *      Author: Brant
 */

#include "sdk_common.h"
#include "colors.h"
#include "pgtypes.h"


//-------------------------------------------------------------------------------------------------
// RectPGProperty
//-------------------------------------------------------------------------------------------------
WX_PG_IMPLEMENT_VARIANT_DATA(wxRect)
wxPG_IMPLEMENT_PROPERTY_CLASS(RectPGProperty, wxPGProperty, TextCtrl)

RectPGProperty::RectPGProperty(const wxString& label, const wxString& name, const wxRect& value,
		const wxString& name_x, const wxString& name_y, const wxString& name_widht, const wxString& name_height) : wxPGProperty(label, name)
{
	SetValueI(value);

	AddPrivateChild(new wxIntProperty(name_x, wxPG_LABEL, value.x));
	AddPrivateChild(new wxIntProperty(name_y, wxPG_LABEL, value.y));
	AddPrivateChild(new wxIntProperty(name_widht, wxPG_LABEL, value.width));
	AddPrivateChild(new wxIntProperty(name_height, wxPG_LABEL, value.height));
}

RectPGProperty::~RectPGProperty(void)
{

}

void RectPGProperty::RefreshChildren(void)
{
	if (GetChildCount() <= 0)
		return;

	const wxRect& rect = wxRectRefFromVariant(m_value);
	Item(0)->SetValue((long)rect.x);
	Item(1)->SetValue((long)rect.y);
	Item(2)->SetValue((long)rect.width);
	Item(3)->SetValue((long)rect.height);
}

wxVariant RectPGProperty::ChildChanged(wxVariant& thisValue, int childIndex, wxVariant& childValue) const
{
	const wxRect &rect = wxRectRefFromVariant(m_value);
	wxRect newValue = rect;
	int childVal = childValue.GetLong();
	switch (childIndex)
	{
		case 0: newValue.SetX(childVal); break;
		case 1: newValue.SetY(childVal); break;
		case 2: newValue.SetWidth(childVal); break;
		case 3: newValue.SetHeight(childVal); break;
	}

	wxVariant newVariant;
	newVariant << newValue;

	return newVariant;
}



//-------------------------------------------------------------------------------------------------
// ColorPGProperty
//-------------------------------------------------------------------------------------------------
static wxPGChoices gs_ColorPGProperty_choicesCache;

ColorPGProperty::ColorPGProperty(const wxString& label, const wxString& name, const wxColourPropertyValue& value)
			   : wxSystemColourProperty(label, name, Colors::Get()->GetNames(), Colors::Get()->GetIds(), &gs_ColorPGProperty_choicesCache, value)
{
	SetAttribute(wxPG_COLOUR_HAS_ALPHA, true);
	Init(value.m_type, value.m_colour);
}

ColorPGProperty::~ColorPGProperty(void) {}


wxColour ColorPGProperty::GetColour(int index) const
{
	return Colors::Get()->GetColor(index);
}

wxSize ColorPGProperty::OnMeasureImage(int) const
{
    return wxPG_DEFAULT_IMAGE_SIZE;
}

void ColorPGProperty::OnCustomPaint(wxDC& dc, const wxRect& rect, wxPGPaintData& paintdata)
{
    wxColour col;

    if ( paintdata.m_choiceItem >= 0 &&
         paintdata.m_choiceItem < (int)m_choices.GetCount() &&
         (paintdata.m_choiceItem != GetCustomColourIndex()/* ||
          m_flags & wxPG_PROP_HIDE_CUSTOM_COLOUR*/) )
    {
        int colInd = m_choices[paintdata.m_choiceItem].GetValue();
        col = GetColour( colInd );
//        wxLogMessage(wxString::Format("paintdata.m_choiceItem: %d, colInd: %d", paintdata.m_choiceItem, colInd));
    }
    else if ( !IsValueUnspecified() )
    {
        col = GetVal().m_colour;
//        unsigned int type = GetVal().m_type;
//    	wxLogMessage(wxString::Format("type: %d, Color: %d, %d, %d, %d", type, col.Alpha(), col.Red(), col.Green(), col.Blue()));
    }

//    wxLogMessage(wxString::Format("Color: %d, %d, %d, %d", col.Alpha(), col.Red(), col.Green(), col.Blue()));

	if (col.IsOk())
    {
		if (col.GetRGBA() == 0x0)	/* transparent color */
		{
			dc.SetBrush(wxNullBrush);
			dc.SetPen(wxPen(wxColour(255, 0, 0), 1));
			dc.DrawRectangle(rect);
		}
		else if (col.GetRGBA() == 0x00FF00FF)	/* clear color */
		{
			dc.SetBrush(wxNullBrush);
			dc.DrawRectangle(rect);

			int width = rect.width / 2 - 2;
			int height = rect.height / 2 - 2;

			dc.SetPen(wxPen(wxColour(0, 0, 0), 1, wxPENSTYLE_TRANSPARENT));
			dc.SetBrush(wxColour(235, 235, 235));

			dc.DrawRectangle(rect.x + 2, rect.y + 2, width, height);
			dc.DrawRectangle(rect.x + rect.width - width - 2, rect.y + rect.height - height - 2, width, height);
			dc.SetBrush(wxColour(210, 210, 210));
			dc.DrawRectangle(rect.x + rect.width - width - 2, rect.y + 2, width, height);
			dc.DrawRectangle(rect.x + 2, rect.y + rect.height - height - 2, width, height);
		}
		else
		{
			dc.SetBrush(col);
			dc.DrawRectangle(rect);
		}
    }
}

//-------------------------------------------------------------------------------------------------
// SizeProperty
//-------------------------------------------------------------------------------------------------
wxPG_IMPLEMENT_PROPERTY_CLASS(SizeProperty, wxPGProperty, TextCtrl)

SizeProperty::SizeProperty( const wxString& label, const wxString& name,
    const wxSize& value, const wxString& first_name, const wxString& second_name) : wxPGProperty(label,name)
{
    SetValueI(value);
    AddPrivateChild( new wxIntProperty(first_name,wxPG_LABEL,value.x) );
    AddPrivateChild( new wxIntProperty(second_name,wxPG_LABEL,value.y) );
}

SizeProperty::~SizeProperty() { }

void SizeProperty::RefreshChildren()
{
    if ( !GetChildCount() ) return;
    const wxSize& size = wxSizeRefFromVariant(m_value);
    Item(0)->SetValue( (long)size.x );
    Item(1)->SetValue( (long)size.y );
}

wxVariant SizeProperty::ChildChanged( wxVariant& thisValue,
                                        int childIndex,
                                        wxVariant& childValue ) const
{
    wxSize& size = wxSizeRefFromVariant(thisValue);
    int val = childValue.GetLong();
    switch ( childIndex )
    {
        case 0: size.x = val; break;
        case 1: size.y = val; break;
    }
    wxVariant newVariant;
    newVariant << size;
    return newVariant;
}


// -----------------------------------------------------------------------
// PointProperty
// -----------------------------------------------------------------------

wxPG_IMPLEMENT_PROPERTY_CLASS(PointProperty,wxPGProperty,TextCtrl)

PointProperty::PointProperty( const wxString& label, const wxString& name,
    const wxPoint& value) : wxPGProperty(label,name)
{
    SetValueI(value);
    AddPrivateChild( new wxIntProperty("X",wxPG_LABEL,value.x) );
    AddPrivateChild( new wxIntProperty("Y",wxPG_LABEL,value.y) );
}

PointProperty::~PointProperty() { }

void PointProperty::RefreshChildren()
{
    if ( !GetChildCount() ) return;
    const wxPoint& point = wxPointRefFromVariant(m_value);
    Item(0)->SetValue( (long)point.x );
    Item(1)->SetValue( (long)point.y );
}

wxVariant PointProperty::ChildChanged( wxVariant& thisValue,
                                         int childIndex,
                                         wxVariant& childValue ) const
{
    wxPoint& point = wxPointRefFromVariant(thisValue);
    int val = childValue.GetLong();
    switch ( childIndex )
    {
        case 0: point.x = val; break;
        case 1: point.y = val; break;
    }
    wxVariant newVariant;
    newVariant << point;
    return newVariant;
}
