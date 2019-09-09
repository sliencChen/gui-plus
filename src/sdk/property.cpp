/*
 * property.cpp
 *
 *  Created on: 2019-4-20
 *      Author: Brant
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sdk_common.h"
#include "colors.h"
#include "property.h"


//-------------------------------------------------------------------------------------------------
// PropertyInteger
//-------------------------------------------------------------------------------------------------
PropertyInteger::PropertyInteger(int value, bool enable, bool visiable)
{
	m_Type = PropertyType_Integer;
	m_Enable = enable;

	m_Visiable = visiable;
	m_IntegerValue = value;
}

PropertyInteger::PropertyInteger(const char *string, int defaultValue)
{
	m_Type = PropertyType_Integer;

	if (!string || strlen(string) <= 0)
		m_IntegerValue = defaultValue;
	else
		m_IntegerValue = atoi(string);
}


wxString PropertyInteger::toString(void)
{
	return wxString::Format("%d", m_IntegerValue);
}

const char* PropertyInteger::xmlString(void)
{
    static char stringValue[16];

    snprintf(stringValue, sizeof(stringValue), "%d", m_IntegerValue);

	return (const char *)stringValue;
}


int PropertyInteger::IntValue(void)
{
	return m_IntegerValue;
}

void PropertyInteger::SetIntValue(int value)
{
	m_IntegerValue = value;
}



PropertyBool::PropertyBool(bool value, bool enable, bool visiable)
{
	m_Type = PropertyType_Bool;
	m_Enable = enable;

	m_Visiable = visiable;
	m_BoolValue = value;
}


wxString PropertyBool::toString(void)
{
	return wxString::Format("%d", m_BoolValue);
}

const char* PropertyBool::xmlString(void)
{
	static char stringValue[8];

	snprintf(stringValue, sizeof(stringValue), "%d", m_BoolValue);

	return (const char *)stringValue;
}


bool PropertyBool::BoolValue(void)
{
	return m_BoolValue;
}

void PropertyBool::SetBoolValue(bool value)
{
	m_BoolValue = value;
}



//-------------------------------------------------------------------------------------------------
// PropertyString
//-------------------------------------------------------------------------------------------------
PropertyString::PropertyString(const wxString& value, bool enable, bool visiable)
{
	m_Type = PropertyType_String;
	m_Enable = enable;

	m_Visiable = visiable;
	m_String = value;
}


wxString PropertyString::toString(void)
{
	return m_String;
}

const char* PropertyString::xmlString(void)
{
	return dsU2C(m_String);
}

void PropertyString::SetStringValue(const wxString& value)
{
	m_String = value;
}

//-------------------------------------------------------------------------------------------------
// PropertyRect
//-------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------
// PropertyColor
//-------------------------------------------------------------------------------------------------
PropertyColor::PropertyColor(void)
{
	m_Type = PropertyType_Color;

	m_Color = wxColour((unsigned long)0);
	m_Name.Empty();
}

PropertyColor::PropertyColor(unsigned int color)
{
	m_Type = PropertyType_Color;

	SetUInt(color);
	m_Name.Empty();
}

PropertyColor::PropertyColor(const char *colorString, unsigned int defaultValue)
{
	m_Type = PropertyType_Color;

	if (!colorString || strlen(colorString) <= 0)
		PropertyColor(defaultValue);

	if (strncmp(colorString, "0x", 2) == 0 || strncmp(colorString, "0X", 2) == 0)
	{
		SetUInt(HexStringValue(colorString));
	}
	else if (strncmp(colorString, "@color/", strlen("@color/")) == 0)
	{
		m_Name = colorString + strlen("@color/");
		unsigned int id = Colors::Get()->GetColorId(m_Name);
		if (id == Colors::CustomColorId())	/* warning, not a system color */
		{
			SetUInt(0xffff00ff);	/* pink color for missed */
		}
		else
		{
			m_Color = Colors::Get()->GetColor(id);
		}
	}
}


wxString PropertyColor::toString(void)
{
	return wxString::Format("%d, %d, %d, %d%", m_Color.Alpha(), m_Color.Red(), m_Color.Green(), m_Color.Blue());
}

const char* PropertyColor::xmlString(void)
{
	static char string[64];

	if (Colors::Get()->IsCorrectColorName(m_Name))
		snprintf(string, sizeof(string), "@color/%s", dsU2C(m_Name));
	else
		snprintf(string, sizeof(string), "0x%08X", UIntValue());

	return string;
}


wxColour PropertyColor::Value(void)
{
	return m_Color;
}

unsigned int PropertyColor::UIntValue(void)
{
	unsigned char a, r, g, b;

	a = m_Color.Alpha();
	r = m_Color.Red();
	g = m_Color.Green();
	b = m_Color.Blue();

	return (a << 24) | (r << 16) | (g << 8) | b;
}

wxString PropertyColor::Name(void)
{
	return m_Name;
}


void PropertyColor::SetColor(unsigned int type, wxColour& color)
{
//	wxLogMessage(wxString::Format("Set Color Index: %d, Value: %d, %d, %d, %d", type, color.Alpha(), color.Red(), color.Green(), color.Blue()));

	if (type == Colors::CustomColorId())
		m_Name.Empty();
	else
		m_Name = Colors::Get()->GetName(type);

	m_Color = color;
}

void PropertyColor::SetUInt(unsigned int color)
{
	unsigned char a, r, g, b;

	a = (unsigned char)((color & 0xFF000000) >> 24);
    r = (unsigned char)((color & 0x00FF0000) >> 16);
    g = (unsigned char)((color & 0x0000FF00) >> 8);
    b = (unsigned char)((color & 0x000000FF));

    m_Color = wxColour(r, g, b, a);
}



//-------------------------------------------------------------------------------------------------
// PropertyImage
//-------------------------------------------------------------------------------------------------
PropertyImage::PropertyImage(void)
{
	m_Type = PropertyType_Image;

	m_Name = wxEmptyString;
}

PropertyImage::PropertyImage(const char* bmp_name)
{
	m_Type = PropertyType_Image;

	m_Name = bmp_name;
}

wxString PropertyImage::toString(void)
{
	return m_Name;
}

const char* PropertyImage::xmlString(void)
{
	return dsU2C(m_Name);
}

wxString PropertyImage::Value(void)
{
	return m_Name;
}


//-------------------------------------------------------------------------------------------------
// PropertyLogo
//-------------------------------------------------------------------------------------------------
PropertyLogo::PropertyLogo(void)
{
	m_Type = PropertyType_Logo;

	m_Name = wxEmptyString;
	m_LogoIndex = -1;
}

wxString PropertyLogo::toString(void)
{
	return m_Name;
}

const char* PropertyLogo::xmlString(void)
{
	return dsU2C(m_Name);
}

wxString PropertyLogo::Value(void)
{
	return m_Name;
}


int PropertyLogo::LogoIndex(void)
{
	//return the index by file_name(buf)
	return m_LogoIndex;
}


//-------------------------------------------------------------------------------------------------
// PropertyInternalString
//-------------------------------------------------------------------------------------------------
PropertyInternalString::PropertyInternalString(const wxString& value, bool enable, bool visiable)
{
	m_Type = PropertyType_InternalString;
	m_Enable = enable;

	m_Visiable = visiable;
	m_String = value;
	save_string_id_value = -1;
}

wxString PropertyInternalString::toString(void)
{
	if (m_String.StartsWith(dsC2U("@string/"), &m_String))
	{
		return dsC2U(StringManager::Get()->SetRealStringByIDString((char *)dsU2C(m_String)));
	}
	return m_String;
}

const char* PropertyInternalString::xmlString(void)
{
	static char string[64];

	if (StringManager::Get()->check_is_internal_string((char *)dsU2C(m_String)))
	{
		sprintf(string, "@string/%s", StringManager::Get()->SetIDStringByRealString((char *)dsU2C(m_String)));
		return string;
	}

	return dsU2C(m_String);
}

const char* PropertyInternalString::GetStringValue(void)
{
	return dsU2C(m_String);
}

void PropertyInternalString::SetStringValue(const wxString& value)
{
	m_String = value;
	if (m_String.StartsWith(dsC2U("@string/"), &m_String))
	{
		wxLogMessage(m_String);
		m_String = dsC2U(StringManager::Get()->SetRealStringByIDString((char *)dsU2C(m_String)));
		wxLogMessage(m_String);
	}
}

int PropertyInternalString::GetSaveIdValue(void)
{
	return save_string_id_value;
}

void PropertyInternalString::SetSaveIdValue(int value)
{
	save_string_id_value = value;
}


PropertyFold::PropertyFold(const wxString& value, const wxString& property_name, bool enable, bool visiable)
{
	m_Type = PropertyType_Fold;
	m_Enable = enable;

	m_Visiable = visiable;
	m_String = value;
	parent = property_name;
}


wxString PropertyFold::toString(void)
{
	return m_String;
}

const char* PropertyFold::xmlString(void)
{
	return dsU2C(m_String);
}

void PropertyFold::SetStringValue(const wxString& value)
{
	m_String = value;
}

//-------------------------------------------------------------------------------------------------
// PropertyEnum
//-------------------------------------------------------------------------------------------------
PropertyEnum::PropertyEnum(int value, bool enable, bool visiable)
{
	m_Type = PropertyType_Enum;
	m_Enable = enable;

	m_Visiable = visiable;
	m_EnumValue = value;
}

PropertyEnum::PropertyEnum(const char *string, int defaultValue)
{
	m_Type = PropertyType_Enum;

	if (!string || strlen(string) <= 0)
		m_EnumValue = defaultValue;
	else
		m_EnumValue = atoi(string);
}

wxString PropertyEnum::toString(void)
{
	return wxString::Format("%d", m_EnumValue);
}

const char* PropertyEnum::xmlString(void)
{
    static char stringValue[16];

    snprintf(stringValue, sizeof(stringValue), "%d", m_EnumValue);

	return (const char *)stringValue;
}


int PropertyEnum::EnumValue(void)
{
	return m_EnumValue;
}

void PropertyEnum::SetEnumValue(int value)
{
	m_EnumValue = value;
}



