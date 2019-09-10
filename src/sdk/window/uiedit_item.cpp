/*
 * uiedit_item.cpp
 *
 *  Created on: 2019-8-9
 *      Author: Antony
 */

#include "window/uiedit.h"
#include "window/uiedit_item.h"


uiEditItem::uiEditItem(uiWindow *parent) : uiWindow(parent)
{
	/* override property */
	SetStringProperty(PROP_CLASS,		"UIEditItem");
	SetProperty(PROP_POSITION, wxRect(0, 0, EDIT_ITEM_DEFAULT_WIDTH, EDIT_ITEM_DEFAULT_HEIGHT));
	m_Properties.find(PROP_BKG_COLOR)->second->m_Visiable = false;
	/* specially for UIEditItem */
	m_Properties.insert(pair<wxString, PropertyBase*>("Edit Item Property", new PropertyFold("Edit Item Property")));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_EDIT_ITEM_HEAD_STRING, new PropertyInternalString("Null value")));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_EDIT_ITEM_FIELD_STRING, new PropertyInternalString("Null value")));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_EDIT_ITEM_HEAD_WIDTH, new PropertyInteger(400)));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_EDIT_ITEM_TYPE, new PropertyEnum(0)));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_FOCUS, new PropertyBool(false)));

	/* Marked as a control */
	m_type = WINDOW_TYPE_CONTROL;
	m_icon_index = ICON_CONTROL_EDIT_ITEM;
}


uiEditItem::~uiEditItem(void)
{

}


wxPGChoices uiEditItem::AddEnmuChildProperty(wxString key)
{
	wxPGChoices choice;
	if (key.IsSameAs(PROP_EDIT_ITEM_TYPE))
	{
		choice.Add("EDIT_FIELD_TYPE_SELECT", 0);
		choice.Add("EDIT_FIELD_TYPE_INPUT", 1);
	}
	return choice;
}


UIWindow* uiEditItem::guiWindow(void)
{
	UIEditItem	*window = create_edit_item(GetStrPropValue(PROP_EDIT_ITEM_HEAD_STRING));
	stb_window = window;
	UIEdit *edit = this->m_Parent->stb_window;
	UIEditData *edit_data = (UIEditData *)edit->private_data;
	UIListSizer *list_sizer = (UIListSizer *)edit_data->list_sizer;
	if (!strcmp(m_Parent->stb_window->class_name, EDIT_CLASS_NAME))
	{
		edit_add_window(m_Parent->stb_window, window);
		set_edit_item_field_type(window, GetEnumPropValue(PROP_EDIT_ITEM_TYPE));
		set_edit_item_header_text(window, GetStrPropValue(PROP_EDIT_ITEM_HEAD_STRING));
		set_edit_item_field_text(window, GetStrPropValue(PROP_EDIT_ITEM_FIELD_STRING));
		set_edit_item_high_light(window, GetBoolPropValue(PROP_FOCUS));
		set_edit_item_header_width(window, GetIntPropValue(PROP_EDIT_ITEM_HEAD_WIDTH));

		sizer_add_window(list_sizer, window, window->rect.size, 1, 1, UI_ALIGN_CENTER_VER|UI_SIZER_FLAG_BORDER_TOP|UI_SIZER_FLAG_EXPAND);
		return window;
	}
	return nullptr;
}


TiXmlElement* uiEditItem::GetXMLElement(void)
{
	TiXmlElement* element = uiWindow::GetXMLElement();
	/* append UIEdit property */
	element->SetAttribute("headWidth", 		m_Properties.find(PROP_EDIT_ITEM_HEAD_WIDTH)->second->xmlString());
	InsertTextChild(element, "headString", 	m_Properties.find(PROP_EDIT_ITEM_HEAD_STRING)->second->xmlString());
	InsertTextChild(element, "fieldString", m_Properties.find(PROP_EDIT_ITEM_FIELD_STRING)->second->xmlString());
	element->SetAttribute("ItemType", 		m_Properties.find(PROP_EDIT_ITEM_TYPE)->second->xmlString());
	element->SetAttribute("focus", 			m_Properties.find(PROP_FOCUS)->second->xmlString());

	return element;
}


void uiEditItem::SetProperties(const TiXmlElement* element)
{
	/* read base property */
	uiWindow::SetProperties(element);

	/* read UIEdit special property */
	const TiXmlElement *text = element->FirstChildElement("headString");
    if (dsC2U(text->GetText()).empty())
    {
        wxLogMessage("%s have no headString property!",element->Attribute("name"));
    }
    else
    {
        SetStringProperty(PROP_EDIT_ITEM_HEAD_STRING, text->GetText());
    }

	text = element->FirstChildElement("fieldString");
    if (dsC2U(text->GetText()).empty())
    {
        wxLogMessage("%s have no fieldString property!",element->Attribute("name"));
    }
    else
    {
        SetStringProperty(PROP_EDIT_ITEM_FIELD_STRING, text->GetText());
    }

	const char *value = element->Attribute("headWidth");
    if (dsC2U(value).empty())
    {
        wxLogMessage("%s have no headWidth property!",element->Attribute("name"));
    }
    else
    {
        SetIntegerProperty(PROP_EDIT_ITEM_HEAD_WIDTH, value);
    }

	value = element->Attribute("ItemType");
    if (dsC2U(value).empty())
    {
        wxLogMessage("%s have no ItemType property!",element->Attribute("name"));
    }
    else
    {
        SetEnumProperty(PROP_EDIT_ITEM_TYPE, value);
    }

	value = element->Attribute("focus");
    if (dsC2U(value).empty())
    {
        wxLogMessage("%s have no focus property!",element->Attribute("name"));
    }
    else
    {
        SetBoolProperty(PROP_FOCUS, value);
    }
}


void uiEditItem::OnBuildDeclarationsCode()
{
	AddDeclaration(wxString::Format(wxT("UIEdit *%s = create_edit_item(\"%s\");"), GetStrPropValue(PROP_VAR_NAME), GetName()));
}


void uiEditItem::OnBuildWindowStructureCode()
{
	uiWindow *Parent_win = m_Parent;
	if (Parent_win->GetClassName() == "UIEdit")
		AddStructure(wxString::Format(wxT("add_sub_window(%s, %s);"), Parent_win->GetStrPropValue(PROP_VAR_NAME), GetStrPropValue(PROP_VAR_NAME)));
}


void uiEditItem::OnBuildCreatingCode()
{
	uiWindow::OnBuildCreatingCode();
	uiWindow *Parent_win = m_Parent;
	if (Parent_win->GetClassName() == "UIEdit")
	{
		AddBuildingCode(wxString::Format(wxT("set_edit_item_field_type(%s, %d);\n"), GetStrPropValue(PROP_VAR_NAME), GetBoolPropValue(PROP_EDIT_ITEM_TYPE)));
		AddBuildingCode(wxString::Format(wxT("set_edit_item_header_text(%s, \"%s\");\n"), GetStrPropValue(PROP_VAR_NAME), GetStrPropValue(PROP_EDIT_ITEM_HEAD_STRING)));
		AddBuildingCode(wxString::Format(wxT("set_edit_item_field_text(%s, \"%s\");\n"), GetStrPropValue(PROP_VAR_NAME), GetStrPropValue(PROP_EDIT_ITEM_FIELD_STRING)));
		AddBuildingCode(wxString::Format(wxT("set_edit_item_high_light(%s, %d);\n"), GetStrPropValue(PROP_VAR_NAME), GetBoolPropValue(PROP_FOCUS)));
		AddBuildingCode(wxString::Format(wxT("set_edit_item_header_width(%s, %d);\n"), GetStrPropValue(PROP_VAR_NAME), GetIntPropValue(PROP_EDIT_ITEM_HEAD_WIDTH)));
		AddBuildingCode(wxString::Format(wxT("sizer_add_window(%s_data->list_sizer, %s, %s->rect.size, 1, 1, UI_ALIGN_CENTER_VER|UI_SIZER_FLAG_BORDER_TOP|UI_SIZER_FLAG_EXPAND);\n"),
		m_Parent->GetStrPropValue(PROP_VAR_NAME), GetStrPropValue(PROP_VAR_NAME), GetStrPropValue(PROP_VAR_NAME)));
	}
	AddBuildingCode("\n");
}


void uiEditItem::OnBuildIdCode()
{

}
