/*
 * uipage.cpp
 *
 *  Created on: 2019-4-18
 *      Author: Brant
 */

#include "window/uipage.h"


uiPage::uiPage(void) : uiWindow(nullptr)
{
	/* override property */
	SetStringProperty(PROP_CLASS, "UIPage");
	SetColorProperty(PROP_BKG_COLOR, wxColour(255, 255, 255, 255));
	SetProperty(PROP_POSITION, wxRect(0, 0, PAGE_DEFAULT_WIDTH, PAGE_DEFAULT_HEIGHT));
	m_Properties.find(PROP_POSITION)->second->m_Enable = false;

	m_Properties.insert(pair<wxString, PropertyBase*>("Page Property", new PropertyFold("Page Property")));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_SHOW_LOGO, new PropertyLogo()));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_STOP_CHANNEL, new PropertyBool(0)));
	m_Properties.insert(pair<wxString, PropertyBase*>("Page Container Property", new PropertyFold("Page Container Property")));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_CONTAINER_INSET, new PropertyRect("Top", "Bottom", "Left", "Right")));
	/* Marked as a sub window */
	m_type = WINDOW_TYPE_SUB;
	m_icon_index = ICON_CONTROL_PAGE;
}


wxPGChoices uiPage::AddEnmuChildProperty(wxString key)
{
	wxPGChoices choice;

	return choice;
}

UIWindow* uiPage::guiWindow(void)
{
	UIWindow *window = create_page(GetStrPropValue(PROP_NAME));

	set_window_color(window, GetColorPropValue(PROP_BKG_COLOR));
	set_page_need_play_logo(window, GetLogoIndexPropValue(PROP_SHOW_LOGO));
	set_page_need_stop_channel(window, GetBoolPropValue(PROP_STOP_CHANNEL));

	wxRect rect = GetRectPropValue(PROP_CONTAINER_INSET);
	UIInset inset = {rect.GetWidth(), rect.GetX(), rect.GetHeight(), rect.GetY()};
	set_page_container_inset(window, inset);
	return window;
}

TiXmlElement* uiPage::GetXMLElement(void)
{
	TiXmlElement* element = uiWindow::GetXMLElement();

	/* append uiPage property */
	element->SetAttribute("logo", m_Properties.find(PROP_SHOW_LOGO)->second->xmlString());
	element->SetAttribute("StopChannel", m_Properties.find(PROP_STOP_CHANNEL)->second->xmlString());

	element->InsertEndChild(TiXmlElement("inset"));
	TiXmlElement *insetElement = element->FirstChildElement("inset");
	PropertyRect *position = (PropertyRect *)m_Properties.find(PROP_CONTAINER_INSET)->second;
	insetElement->SetAttribute("top", position->GetX());
	insetElement->SetAttribute("bottom", position->GetY());
	insetElement->SetAttribute("left", position->GetWidth());
	insetElement->SetAttribute("right", position->GetHeight());

	return element;
}


void uiPage::SetProperties(const TiXmlElement* element)
{
	uiWindow::SetProperties(element);

	const char *value = element->Attribute("logo");
    if (dsC2U(value).empty())
    {
        wxLogMessage("%s have no logo property!",element->Attribute("name"));
    }
    else
    {
       SetLogoProperty(PROP_SHOW_LOGO, value);
    }

	value = element->Attribute("StopChannel");
	if (dsC2U(value).empty())
    {
        wxLogMessage("%s have no StopChannel property!",element->Attribute("name"));
    }
    else
    {
        SetBoolProperty(PROP_STOP_CHANNEL, value);
    }

    wxRect insetValue;
	const TiXmlElement *inset = element->FirstChildElement("inset");
	if (inset)
	{
		inset->QueryIntAttribute("top", &(insetValue.x));
		inset->QueryIntAttribute("bottom", &(insetValue.y));
		inset->QueryIntAttribute("left", &(insetValue.width));
		inset->QueryIntAttribute("right", &(insetValue.height));
	}
	SetProperty(PROP_CONTAINER_INSET, insetValue);

}


void uiPage::OnBuildDeclarationsCode()
{
	AddDeclaration(wxString::Format(wxT("UIPage *%s = create_page(\"%s\");"), GetStrPropValue(PROP_VAR_NAME), GetName()));
}

void uiPage::OnBuildCreatingCode()
{
	uiWindow::OnBuildCreatingCode();
	AddBuildingCode(wxString::Format(wxT("set_page_need_play_logo(%s， %d);\n"), GetStrPropValue(PROP_VAR_NAME), GetLogoIndexPropValue(PROP_SHOW_LOGO)));

	if (GetBoolPropValue(PROP_STOP_CHANNEL) == 1)
	{
		AddBuildingCode(wxString::Format(wxT("set_page_need_stop_channel(%s， TRUE);\n"), GetStrPropValue(PROP_VAR_NAME)));
	}
	else if (GetBoolPropValue(PROP_STOP_CHANNEL) == 0)
	{
		AddBuildingCode(wxString::Format(wxT("set_page_need_stop_channel(%s， FALSE);\n"), GetStrPropValue(PROP_VAR_NAME)));
	}
	AddBuildingCode("\n");
}

void uiPage::OnBuildIdCode()
{

}
