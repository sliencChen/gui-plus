/*
 * uimenu.cpp
 *
 *  Created on: 2019-8-9
 *      Author: Antony
 */

#include "window/uimenu.h"


uiMenu::uiMenu(uiWindow *parent) : uiWindow(parent)
{
	/* override property */
	SetStringProperty(PROP_CLASS,		"UIMenu");
	SetProperty(PROP_POSITION, wxRect(0, 0, MENU_DEFAULT_WIDTH, MENU_DEFAULT_HEIGHT));
	m_Properties.find(PROP_BKG_COLOR)->second->m_Visiable = false;

	/* specially for UIMenu */
	m_Properties.insert(pair<wxString, PropertyBase*>("Menu Property", new PropertyFold("Menu Property")));
	m_Properties.insert(pair<wxString, PropertyBase*>("Menu Container Property", new PropertyFold("Menu Container Property")));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_CONTAINER_INSET, new PropertyRect("Top", "Bottom", "Left", "Right")));

	/* Marked as a sub */
	m_type = WINDOW_TYPE_SUB;
	m_icon_index = ICON_CONTROL_MENU;
}


uiMenu::~uiMenu(void)
{

}

wxPGChoices uiMenu::AddEnmuChildProperty(wxString key)
{
	wxPGChoices choice;

	return choice;
}

UIWindow* uiMenu::guiWindow(void)
{
	UIWindow *window = create_vertical_menu(GetStrPropValue(PROP_NAME));

	wxRect rect = GetRectPropValue(PROP_POSITION);
	set_window_rect(window, ui_rect(rect.x, rect.y, rect.width, rect.height));

	m_absolute_pos.x = rect.x;
	m_absolute_pos.y = rect.y;

	rect = GetRectPropValue(PROP_CONTAINER_INSET);
	UIInset inset = {rect.GetWidth(), rect.GetX(), rect.GetHeight(), rect.GetY()};
	set_menu_inset(window, inset);

	return window;
}


TiXmlElement* uiMenu::GetXMLElement(void)
{
	TiXmlElement* element = uiWindow::GetXMLElement();

	element->InsertEndChild(TiXmlElement("inset"));
	TiXmlElement *insetElement = element->FirstChildElement("inset");
	PropertyRect *position = (PropertyRect *)m_Properties.find(PROP_CONTAINER_INSET)->second;
	insetElement->SetAttribute("top", position->GetX());
	insetElement->SetAttribute("bottom", position->GetY());
	insetElement->SetAttribute("left", position->GetWidth());
	insetElement->SetAttribute("right", position->GetHeight());

	return element;
}


void uiMenu::SetProperties(const TiXmlElement* element)
{
	/* read base property */
	uiWindow::SetProperties(element);

	/* read UIEdit special property */
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


void uiMenu::OnBuildDeclarationsCode()
{

	AddDeclaration(wxString::Format(wxT("UIEdit *%s = create_edit(\"%s\");"), GetStrPropValue(PROP_VAR_NAME), GetName()));
}

void uiMenu::OnBuildCreatingCode()
{
	uiWindow::OnBuildCreatingCode();
	AddBuildingCode("\n");
}

void uiMenu::OnBuildIdCode()
{

}




