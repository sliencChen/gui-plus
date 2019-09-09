/*
 * property.h
 *
 *  Created on: 2019-4-11
 *      Author: Brant
 */

#ifndef PROPERTY_H_
#define PROPERTY_H_


#include <wx/colour.h>
#include <wx/string.h>
#include "util.h"
#include <wx/propgrid/property.h>

enum
{
	PropertyType_None,
	PropertyType_Integer,
	PropertyType_Bool,
	PropertyType_String,
	PropertyType_Rect,
	PropertyType_Color,
	PropertyType_Image,
	PropertyType_Logo,
	PropertyType_InternalString,
	PropertyType_Fold,
	PropertyType_Size,
	PropertyType_Enum,
	PropertyType_Num
};


class PropertyBase
{
public:
	virtual ~PropertyBase(void) {}

    virtual wxString toString(void) = 0;
    virtual const char* xmlString(void) = 0;

    int m_Type = PropertyType_None;
    bool m_Enable = true;
    bool m_Visiable = true;
};



class PropertyInteger : public PropertyBase
{
public:
    PropertyInteger(int value, bool enable = true, bool visiable = true);
    PropertyInteger(const char *string, int defaultValue = 0);

    wxString toString(void);
    const char* xmlString(void);

    int IntValue(void);
    void SetIntValue(int value);

private:
    int m_IntegerValue;
};

class PropertyBool : public PropertyBase
{
public:
    PropertyBool(bool value, bool enable = true, bool visiable = true);

    wxString toString(void);
    const char* xmlString(void);

    bool BoolValue(void);
    void SetBoolValue(bool value);

private:
    bool m_BoolValue;
};



class PropertyString : public PropertyBase
{
public:
	PropertyString(const wxString& value, bool enable = true, bool visiable = true);

    wxString toString(void);
    const char* xmlString(void);

    void SetStringValue(const wxString& value);
private:
    wxString m_String;
};



class PropertyRect : public PropertyBase
{
public:
    PropertyRect(const wxString& name_x = "X", const wxString& name_y = "Y", const wxString& name_widht = "Width", const wxString& name_height = "Height") \
    { m_Type = PropertyType_Rect; string_x = name_x; string_y = name_y; string_width = name_widht; string_height = name_height;}

    wxString toString(void) { return wxString::Format("%d, %d, %d, %d", GetX(), GetY(), GetWidth(), GetHeight()); }
    const char* xmlString(void) { return dsU2C(toString()); }

    int GetX(void) { return m_Rect.x; };
    int GetY(void) { return m_Rect.y; };
    int GetWidth(void) { return m_Rect.width; };
    int GetHeight(void) { return m_Rect.height; };

    wxRect Value(void) { return m_Rect; }

    void SetRect(const wxRect& rect) { m_Rect = rect; }
	wxString string_x;
	wxString string_y;
	wxString string_width;
	wxString string_height;
private:
    wxRect m_Rect;
//    int m_X = 0;
//    int m_Y = 0;
//    int m_Width = 100;
//    int m_Height = 100;
};



class PropertyColor : public PropertyBase
{
public:
    PropertyColor(void);
    PropertyColor(unsigned int color);
    PropertyColor(const char *colorString, unsigned int defaultValue = 0);

    wxString toString(void);
    const char* xmlString(void);

    wxColour Value(void);
    unsigned int UIntValue(void);
    wxString Name(void);

    void SetColor(unsigned int type, wxColour& color);
    void SetUInt(unsigned int color);

private:
    wxColour m_Color;
    wxString m_Name;
};



class PropertyImage : public PropertyBase
{
public:
	PropertyImage(void);
	PropertyImage(const char* bmp_name);

    wxString toString(void);
    const char* xmlString(void);

	wxString Value(void);

	void SetImage(wxString imageName) { m_Name = imageName; }

private:
	wxString m_Name;
};

class PropertyLogo : public PropertyBase
{
public:
	PropertyLogo(void);
	wxString toString(void);
	const char* xmlString(void);
	wxString Value(void);
	void SetLogo(wxString imageName) { m_Name = imageName; }
	void SetLogoFileName(wxString LogoName) { LogoFileName = LogoName; }
	int LogoIndex(void);
	void SetLogoIndex(int logoIndex) { m_LogoIndex = logoIndex; };
private:
	wxString m_Name;
	int m_LogoIndex;
	wxString LogoFileName;
};


class PropertyInternalString : public PropertyBase
{
public:
	PropertyInternalString(const wxString& value, bool enable = true, bool visiable = true);

    wxString toString(void);
    const char* xmlString(void);

    void SetStringValue(const wxString& value);
    const char* GetStringValue(void);
    int GetSaveIdValue(void);
    void SetSaveIdValue(int value);

private:
    int save_string_id_value;
    wxString m_String;
};

class PropertyFold : public PropertyBase
{
public:
	PropertyFold(const wxString& value, const wxString& property_name = "", bool enable = true, bool visiable = true);

	wxString toString(void);
	const char* xmlString(void);

	void SetStringValue(const wxString& value);
	wxString parent;
private:
	wxString m_String;
};


class PropertySize : public PropertyBase
{
public:
    PropertySize(const wxString& width = "Width", const wxString& height = "Height") { m_Type = PropertyType_Size; m_WidthString = width; m_HeightString = height;}

    wxString toString(void) { return wxString::Format("%d, %d", m_Size.GetWidth(), m_Size.GetHeight()); }
    const char* xmlString(void) { return dsU2C(toString()); }


    int GetWidth(void) { return m_Size.GetWidth(); };
    int GetHeight(void) { return m_Size.GetHeight(); };

    wxSize Value(void) { return m_Size; }

    void SetSize(const wxSize& prop_size) { m_Size = prop_size; }

    wxString m_WidthString;
    wxString m_HeightString;
private:
    wxSize m_Size;
};

class PropertyEnum : public PropertyBase
{
public:
    PropertyEnum(int value, bool enable = true, bool visiable = true);
    PropertyEnum(const char *string, int defaultValue = 0);

    wxString toString(void);
    const char* xmlString(void);

    int EnumValue(void);
    void SetEnumValue(int value);

private:
    int m_EnumValue;
};

#endif /* PROPERTY_H_ */
