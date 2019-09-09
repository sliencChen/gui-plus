/*
 * uitext_field.cpp
 *
 *  Created on: 2019-8-12
 *      Author: Antony
 */
#include "project.h"
#include "main.h"
#include "window/uitext_field.h"



uiTextField::uiTextField(uiWindow *parent) : uiWindow(parent)
{
	/* override property */
	SetStringProperty(PROP_CLASS,		"UITextField");
	SetColorProperty(PROP_BKG_COLOR,	wxColour(0, 0, 0, 0));
	SetProperty(PROP_POSITION, wxRect(0, 0, TEXTFIELD_DEFAULT_WIDTH, TEXTFIELD_DEFAULT_HEIGHT));

	/* specially for UIEdit */
	m_Properties.insert(pair<wxString, PropertyBase*>("Text Property", new PropertyFold("Text Property")));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_TEXT, new PropertyInternalString("Text")));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_FORE_COLOR, new PropertyColor(0xff000000)));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_FONT_SIZE, new PropertyInteger(18)));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_TEXT_SCALE, new PropertySize("scale_x", "scale_y")));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_TEXT_SUPPORT_MULTI, new PropertyBool(true)));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_TEXT_REGION_SIZE, new PropertySize("region_x", "region_y")));

	m_Properties.find(PROP_TEXT_REGION_SIZE)->second->m_Visiable = false;
	SetProperty(PROP_TEXT_SCALE, wxSize(100, 100));
	SetProperty(PROP_TEXT_REGION_SIZE, wxSize(TEXTFIELD_DEFAULT_WIDTH, TEXTFIELD_DEFAULT_HEIGHT));

	/* Marked as a control */
	m_type = WINDOW_TYPE_CONTROL;
	m_icon_index = ICON_CONTROL_TEXT_FIELD;
}


uiTextField::~uiTextField(void)
{

}

wxPGChoices uiTextField::AddEnmuChildProperty(wxString key)
{
	wxPGChoices choice;

	return choice;
}

UIWindow* uiTextField::guiWindow(void)
{
	UIWindow *window = create_text_field(GetStrPropValue(PROP_NAME));
	set_window_color(window, GetColorPropValue(PROP_BKG_COLOR));

	set_text_field_text_color(window, GetColorPropValue(PROP_FORE_COLOR));
	set_text_field_font_size(window, GetIntPropValue(PROP_FONT_SIZE));

	wxSize text_size = GetSizePropValue(PROP_TEXT_SCALE);
	set_text_field_font_scale(window, text_size.GetWidth(), text_size.GetHeight());
	set_text_field_multi_line(window, GetBoolPropValue(PROP_TEXT_SUPPORT_MULTI));
	if (GetBoolPropValue(PROP_TEXT_SUPPORT_MULTI) == true)
	{
		m_Properties.find(PROP_TEXT_REGION_SIZE)->second->m_Visiable = false;
	}
	else
	{
		text_size = GetSizePropValue(PROP_TEXT_REGION_SIZE);
		m_Properties.find(PROP_TEXT_REGION_SIZE)->second->m_Visiable = true;
		set_text_region_size(window, text_size.GetWidth(), text_size.GetHeight());
	}

	set_text_field_text(window, GetStrPropValue(PROP_TEXT), 0); //Must use it under the set region size

    if (this->m_Parent->GetWindowType() == WINDOW_TYPE_SIZER)
    {
        this->stb_window = window;
        UISizer *sizer = this->m_Parent->stb_sizer;
        if (sizer)
        {
            sizer_add_window(sizer, window, ui_size(TEXTFIELD_DEFAULT_WIDTH, TEXTFIELD_DEFAULT_HEIGHT), 0, 5, UI_ALIGN_CENTER_VER|UI_SIZER_FLAG_BORDER_TOP);
            uiWindow *parent = this->m_Parent->m_Parent;
            while(parent)
            {
                if (parent->GetWindowType() == WINDOW_TYPE_SUB)
                {
                    if (parent->stb_window)
                    {
                        container_add_sub_window(parent->stb_window, window);
                        container_layout_sub_window(parent->stb_window);
                    }
                    break;
                }
                parent = parent->m_Parent;
            }
            this->RepositionSubWindowRecursively(parent);
        }

    }
    else
    {
        wxRect rect = this->GetRectPropValue(PROP_POSITION);
        set_window_rect(window, ui_rect(rect.x, rect.y, rect.width, rect.height));
    }

	return window;
}


TiXmlElement* uiTextField::GetXMLElement(void)
{
	TiXmlElement* element = uiWindow::GetXMLElement();

	InsertTextChild(element, "text", 	m_Properties.find(PROP_TEXT)->second->xmlString());
	element->SetAttribute("textColor", 	m_Properties.find(PROP_FORE_COLOR)->second->xmlString());
	element->SetAttribute("fontSize", 	m_Properties.find(PROP_FONT_SIZE)->second->xmlString());
	element->SetAttribute("supportMulti", 	m_Properties.find(PROP_TEXT_SUPPORT_MULTI)->second->xmlString());

	element->InsertEndChild(TiXmlElement("scale"));
	TiXmlElement *sizeElement = element->FirstChildElement("scale");
	PropertySize *scale = (PropertySize *)m_Properties.find(PROP_TEXT_SCALE)->second;
	sizeElement->SetAttribute("scale_x", scale->GetWidth());
	sizeElement->SetAttribute("scale_y", scale->GetHeight());

	element->InsertEndChild(TiXmlElement("region"));
	sizeElement = element->FirstChildElement("region");
	scale = (PropertySize *)m_Properties.find(PROP_TEXT_REGION_SIZE)->second;
	sizeElement->SetAttribute("region_x", scale->GetWidth());
	sizeElement->SetAttribute("region_y", scale->GetHeight());


	return element;
}


void uiTextField::SetProperties(const TiXmlElement* element)
{
	/* read base property */
	uiWindow::SetProperties(element);

	/* read uiTextField special property */
	const TiXmlElement *text = element->FirstChildElement("text");
	if (dsC2U(text->GetText()).empty())
    {
        wxLogMessage("%s have no text property!",element->Attribute("name"));
    }
	else
	{
		SetStringProperty(PROP_TEXT, text->GetText());
	}

	const char *value = element->Attribute("textColor");
	if (dsC2U(value).empty())
    {
        wxLogMessage("%s have no textColor property!",element->Attribute("name"));
    }
	else
	{
		SetColorProperty(PROP_FORE_COLOR, value);
	}

	value = element->Attribute("fontSize");
	if (dsC2U(value).empty())
    {
        wxLogMessage("%s have no fontSize property!",element->Attribute("name"));
    }
	else
	{
		SetIntegerProperty(PROP_FONT_SIZE, value);
	}

	value = element->Attribute("supportMulti");
	if (dsC2U(value).empty())
    {
        wxLogMessage("%s have no supportMulti property!",element->Attribute("name"));
    }
	else
	{
		SetBoolProperty(PROP_TEXT_SUPPORT_MULTI, value);
	}

	wxSize scaleValue;
	int width = 0, height = 0;
	const TiXmlElement *size_ele = element->FirstChildElement("scale");
	if (size_ele)
	{
		size_ele->QueryIntAttribute("scale_x", &(width));
		size_ele->QueryIntAttribute("scale_y", &(height));
		scaleValue.SetWidth(width);
		scaleValue.SetHeight(height);
	}
	SetProperty(PROP_TEXT_SCALE, scaleValue);

	wxSize regionValue;
	size_ele = element->FirstChildElement("region");
	if (size_ele)
	{
		size_ele->QueryIntAttribute("region_x", &(width));
		size_ele->QueryIntAttribute("region_y", &(height));
		regionValue.SetWidth(width);
		regionValue.SetHeight(height);
	}
	SetProperty(PROP_TEXT_REGION_SIZE, regionValue);

}


void uiTextField::OnBuildDeclarationsCode()
{

	AddDeclaration(wxString::Format(wxT("UITextField *%s = create_text_field(\"%s\");"), GetStrPropValue(PROP_VAR_NAME), GetName()));
}

void uiTextField::OnBuildCreatingCode()
{
	uiWindow::OnBuildCreatingCode();
	AddBuildingCode("\n");
}

void uiTextField::OnBuildIdCode()
{

}




