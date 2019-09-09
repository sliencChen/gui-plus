/*
 * uiline.cpp
 *
 *  Created on: 2019-8-2
 *      Author: Antony
 */

#include "window/uilist.h"
//#include "control/list.h"


uiList::uiList(uiWindow *parent) : uiWindow(parent)
{
	/* override property */
	SetStringProperty(PROP_CLASS,		"UIList");
	SetProperty(PROP_POSITION, wxRect(0, 0, LIST_DEFAULT_WIDTH, LIST_DEFAULT_HEIGHT));
	m_Properties.find(PROP_BKG_COLOR)->second->m_Visiable = false;
	/* specially for UIList */

	m_Properties.insert(pair<wxString, PropertyBase*>("List Property", new PropertyFold("List Property")));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_TOTAL_NUM, new PropertyInteger(20)));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_PAGE_MAX_ITEN, new PropertyInteger(5)));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_ITEM_SIZE_WIDTH, new PropertyInteger(525)));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_ITEM_HEIGHT, new PropertyInteger(36)));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_ITEM_SPACE, new PropertyInteger(2)));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_OFFSET, new PropertyInteger(0)));

	/* Marked as a control */
	m_type = WINDOW_TYPE_CONTROL;
	m_icon_index = ICON_CONTROL_LIST;
}


uiList::~uiList(void)
{

}

wxPGChoices uiList::AddEnmuChildProperty(wxString key)
{
	wxPGChoices choice;

	return choice;
}

UIWindow* uiList::guiWindow(void)
{
	UIWindow *window = create_list("list");

	set_list_total_num(window, GetIntPropValue(PROP_TOTAL_NUM));
	set_list_page_max_item(window, GetIntPropValue(PROP_PAGE_MAX_ITEN));
	set_list_item_height(window, GetIntPropValue(PROP_ITEM_HEIGHT));
	set_list_item_space(window, GetIntPropValue(PROP_ITEM_SPACE));
	set_list_total_width(window, GetIntPropValue(PROP_ITEM_SIZE_WIDTH));
	set_list_offset(window, GetIntPropValue(PROP_OFFSET));

	wxRect rect = GetRectPropValue(PROP_POSITION);
	set_window_rect(window, ui_rect(rect.x, rect.y, rect.width, rect.height));

	return window;
}


TiXmlElement* uiList::GetXMLElement(void)
{
	TiXmlElement* element = uiWindow::GetXMLElement();

	/* append UIList property */
	element->SetAttribute("totalNum", m_Properties.find(PROP_TOTAL_NUM)->second->xmlString());
	element->SetAttribute("pageMaxItem", m_Properties.find(PROP_PAGE_MAX_ITEN)->second->xmlString());
	element->SetAttribute("totalWidth", m_Properties.find(PROP_ITEM_SIZE_WIDTH)->second->xmlString());
	element->SetAttribute("itemHeight", m_Properties.find(PROP_ITEM_HEIGHT)->second->xmlString());
	element->SetAttribute("space", m_Properties.find(PROP_ITEM_SPACE)->second->xmlString());
	element->SetAttribute("offset", m_Properties.find(PROP_OFFSET)->second->xmlString());

	return element;
}


void uiList::SetProperties(const TiXmlElement* element)
{
	/* read base property */
	uiWindow::SetProperties(element);

	/* read UILabel special property */
	const char *value = element->Attribute("totalNum");
    if (dsC2U(value).empty())
    {
        wxLogMessage("%s have no totalNum property!",element->Attribute("name"));
    }
    else
    {
        SetIntegerProperty(PROP_TOTAL_NUM, value);
    }

	value = element->Attribute("pageMaxItem");
    if (dsC2U(value).empty())
    {
        wxLogMessage("%s have no pageMaxItem property!",element->Attribute("name"));
    }
    else
    {
        SetIntegerProperty(PROP_PAGE_MAX_ITEN, value);
    }

	value = element->Attribute("totalWidth");
    if (dsC2U(value).empty())
    {
        wxLogMessage("%s have no totalWidth property!",element->Attribute("name"));
    }
    else
    {
        SetIntegerProperty(PROP_ITEM_SIZE_WIDTH, value);
    }

	value = element->Attribute("itemHeight");
    if (dsC2U(value).empty())
    {
        wxLogMessage("%s have no itemHeight property!",element->Attribute("name"));
    }
    else
    {
        SetIntegerProperty(PROP_ITEM_HEIGHT, value);
    }

	value = element->Attribute("space");
    if (dsC2U(value).empty())
    {
        wxLogMessage("%s have no space property!",element->Attribute("name"));
    }
    else
    {
        SetIntegerProperty(PROP_ITEM_SPACE, value);
    }

	value = element->Attribute("offset");
    if (dsC2U(value).empty())
    {
        wxLogMessage("%s have no offset property!",element->Attribute("name"));
    }
    else
    {
        SetIntegerProperty(PROP_OFFSET, value);
    }
}


void uiList::OnBuildDeclarationsCode()
{
	AddDeclaration(wxString::Format(wxT("UIList *%s = create_list(\"%s\");"), GetStrPropValue(PROP_VAR_NAME), GetName()));
}

void uiList::OnBuildCreatingCode()
{
	uiWindow::OnBuildCreatingCode();
	AddBuildingCode(wxString::Format(wxT("set_list_total_num(%s, \"%d\");\n"), GetStrPropValue(PROP_VAR_NAME), GetIntPropValue(PROP_TOTAL_NUM)));
	AddBuildingCode(wxString::Format(wxT("set_list_page_max_item(%s, \"%d\");\n"), GetStrPropValue(PROP_VAR_NAME), GetIntPropValue(PROP_PAGE_MAX_ITEN)));
	AddBuildingCode(wxString::Format(wxT("set_list_item_height(%s, \"%d\");\n"), GetStrPropValue(PROP_VAR_NAME), GetIntPropValue(PROP_ITEM_HEIGHT)));
	AddBuildingCode(wxString::Format(wxT("set_list_item_space(%s, \"%d\");\n"), GetStrPropValue(PROP_VAR_NAME), GetIntPropValue(PROP_ITEM_SPACE)));
	AddBuildingCode(wxString::Format(wxT("set_list_offset(%s, \"%d\");\n"), GetStrPropValue(PROP_VAR_NAME), GetIntPropValue(PROP_OFFSET)));
	AddBuildingCode("\n");
}

void uiList::OnBuildIdCode()
{

}


