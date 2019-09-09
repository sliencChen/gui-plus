/*
 * uiedit.cpp
 *
 *  Created on: 2019-8-6
 *      Author: Antony
 */

#include "window/uiedit.h"
#include "window/uiedit_item.h"

uiEdit::uiEdit(uiWindow *parent) : uiWindow(parent)
{
	/* override property */
	SetStringProperty(PROP_CLASS,		"UIEdit");
	SetProperty(PROP_POSITION, wxRect(0, 0, EDIT_DEFAULT_WIDTH, EDIT_DEFAULT_HEIGHT));
	m_Properties.find(PROP_BKG_COLOR)->second->m_Visiable = false;

	/* specially for UIEdit */
	m_Properties.insert(pair<wxString, PropertyBase*>("Edit Property", new PropertyFold("Edit Property")));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_EDIT_PAGE_MAX_ITEMS, new PropertyInteger(10)));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_EDIT_AUTO_SIZE, new PropertyBool(true)));
	m_Properties.insert(pair<wxString, PropertyBase*>("Edit Container Property", new PropertyFold("Edit Container Property")));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_CONTAINER_INSET, new PropertyRect("Top", "Bottom", "Left", "Right")));

	/* Marked as a sub */
	m_type = WINDOW_TYPE_SUB;
	m_icon_index = ICON_CONTROL_EDIT;
}


uiEdit::~uiEdit(void)
{

}

wxPGChoices uiEdit::AddEnmuChildProperty(wxString key)
{
	wxPGChoices choice;

	return choice;
}

void uiEdit::AddUpdateChildProperty(const TiXmlElement* element)
{

}

UIWindow* uiEdit::guiWindow(void)
{
	UIWindow *window = create_edit(GetStrPropValue(PROP_NAME));
	stb_window = window;
	uiWindow *child = m_Childs;

	wxRect rect = GetRectPropValue(PROP_POSITION);
	set_window_rect(window, ui_rect(rect.x, rect.y, rect.width, rect.height));

	m_absolute_pos.x = rect.x;
	m_absolute_pos.y = rect.y;

	rect = GetRectPropValue(PROP_CONTAINER_INSET);
	UIInset inset = {rect.GetWidth(), rect.GetX(), rect.GetHeight(), rect.GetY()};
	set_edit_inset(window, inset);
	set_edit_max_item(window, GetIntPropValue(PROP_EDIT_PAGE_MAX_ITEMS));

	while (child)
	{
		(uiEditItem*)child->guiWindow();
		child = child->m_Next;
	}

	if (m_Childs)
	{
		set_container_auto_size(window, GetBoolPropValue(PROP_EDIT_AUTO_SIZE, TRUE));
		set_edit_sizer(window);
	}

	/*relocation after sizer*/
	child = m_Childs;
	while (child)
	{
		wxRect childRectValue(child->stb_window->rect.point.x, child->stb_window->rect.point.y, child->stb_window->rect.size.width, child->stb_window->rect.size.height);
		child->SetProperty(PROP_POSITION, childRectValue);
		child = child->m_Next;
	}
	wxRect rectValue(window->rect.point.x, window->rect.point.y, window->rect.size.width, window->rect.size.height);
	SetProperty(PROP_POSITION, rectValue);

	return window;
}


TiXmlElement* uiEdit::GetXMLElement(void)
{
	TiXmlElement* element = uiWindow::GetXMLElement();
	element->SetAttribute("edit_page_max_items", m_Properties.find(PROP_EDIT_PAGE_MAX_ITEMS)->second->xmlString());
	element->SetAttribute("edit_auto_size", m_Properties.find(PROP_EDIT_AUTO_SIZE)->second->xmlString());
	element->InsertEndChild(TiXmlElement("inset"));
	TiXmlElement *insetElement = element->FirstChildElement("inset");
	PropertyRect *position = (PropertyRect *)m_Properties.find(PROP_CONTAINER_INSET)->second;
	insetElement->SetAttribute("top", position->GetX());
	insetElement->SetAttribute("bottom", position->GetY());
	insetElement->SetAttribute("left", position->GetWidth());
	insetElement->SetAttribute("right", position->GetHeight());

	return element;
}


void uiEdit::SetProperties(const TiXmlElement* element)
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
	const char *value = element->Attribute("edit_auto_size");
	if (value)
        SetBoolProperty(PROP_EDIT_AUTO_SIZE, value);
}


void uiEdit::OnBuildDeclarationsCode()
{
	AddDeclaration(wxString::Format(wxT("UIEdit *%s = create_edit(\"%s\");"), GetStrPropValue(PROP_VAR_NAME), GetName()));
}

void uiEdit::OnBuildCreatingCode()
{
	uiWindow::OnBuildCreatingCode();
	AddBuildingCode("\n");
}

void uiEdit::OnBuildIdCode()
{

}

void uiEdit::BuildCode(wxsCoderContext* Context)
{
    wxsCoderContext* Store = GetCoderContext();
    Store = Context;
    long FlagsStore = Context->m_Flags;
//    OnUpdateFlags(Context->m_Flags);
    OnBuildDeclarationsCode();
    OnBuildWindowStructureCode();
    OnBuildCreatingCode();
    OnBuildIdCode();
//    OnBuildHeadersCode();
//    OnBuildEventsConnectingCode();

	uiWindow *child = m_Childs;
	while (child)
	{
		child->BuildCode(Context);
		child = child->m_Next;

	}
//    OnBuildXRCFetchingCode();
    Context->m_Flags = FlagsStore;
}
