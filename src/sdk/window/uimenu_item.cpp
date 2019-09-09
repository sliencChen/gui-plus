/*
 * uimenu_item.cpp
 *
 *  Created on: 2019-8-9
 *      Author: Antony
 */

//#include "window/uimenu.h"
#include "window/uimenu_item.h"


uiMenuItem::uiMenuItem(uiWindow *parent) : uiWindow(parent)
{
	/* override property */
	SetStringProperty(PROP_CLASS,		"UIMenuItem");
	m_Properties.find(PROP_BKG_COLOR)->second->m_Visiable = false;
	//SetProperty(PROP_POSITION, wxRect(0, 0, MENU_ITEM_DEFAULT_WIDTH, MENU_ITEM_DEFAULT_HEIGHT));

	/* specially for UIMenuItem */
	m_Properties.insert(pair<wxString, PropertyBase*>("Menu Item Property", new PropertyFold("Menu Item Property")));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_TEXT, new PropertyInternalString("Null value")));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_LIGHT_BMP, new PropertyImage()));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_NORMAL_BMP, new PropertyImage()));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_ITEM_SIZE_WIDTH, new PropertyInteger(900)));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_FOCUS, new PropertyBool(true)));

	/* Marked as a control */
	m_type = WINDOW_TYPE_CONTROL;
	m_icon_index = ICON_CONTROL_MENU_ITEM;
}


uiMenuItem::~uiMenuItem(void)
{

}

wxPGChoices uiMenuItem::AddEnmuChildProperty(wxString key)
{
	wxPGChoices choice;

	return choice;
}

UIWindow* uiMenuItem::guiWindow(void)
{
	UIWindow *window = create_menu_item(GetStrPropValue(PROP_NAME));

    set_menu_item_text(window, GetStrPropValue(PROP_TEXT));
    set_menu_item_light_icon(window, GetImagePropValue(PROP_LIGHT_BMP));
    set_menu_item_normal_icon(window, GetImagePropValue(PROP_NORMAL_BMP));
	set_menu_item_highlight(window, GetBoolPropValue(PROP_FOCUS));

    if (this->m_Parent->GetWindowType() == WINDOW_TYPE_SIZER)
    {
        this->stb_window = window;
        UISizer *sizer = this->m_Parent->stb_sizer;
        if (sizer)
        {
            sizer_add_window(sizer, window, ui_size(MENU_ITEM_DEFAULT_WIDTH, MENU_ITEM_DEFAULT_HEIGHT), 0, 5, UI_ALIGN_CENTER_VER|UI_SIZER_FLAG_BORDER_TOP);
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


TiXmlElement* uiMenuItem::GetXMLElement(void)
{
	TiXmlElement* element = uiWindow::GetXMLElement();


	/* append UIMenuItem property */

	InsertTextChild(element, "text", 	m_Properties.find(PROP_TEXT)->second->xmlString());
	element->SetAttribute("lightBmp", 	m_Properties.find(PROP_LIGHT_BMP)->second->xmlString());
	element->SetAttribute("normalBmp", 	m_Properties.find(PROP_NORMAL_BMP)->second->xmlString());
	element->SetAttribute("totalWidth",	m_Properties.find(PROP_ITEM_SIZE_WIDTH)->second->xmlString());
	element->SetAttribute("focus", 		m_Properties.find(PROP_FOCUS)->second->xmlString());

	return element;
}


void uiMenuItem::SetProperties(const TiXmlElement* element)
{
	/* read base property */
	uiWindow::SetProperties(element);

	/* read UIMenuItem special property */
	const TiXmlElement *text = element->FirstChildElement("text");
    if (dsC2U(text->GetText()).empty())
    {
        wxLogMessage("%s have no text property!",element->Attribute("name"));
    }
    else
    {
        SetStringProperty(PROP_TEXT, text->GetText());
    }

	const char *value = element->Attribute("lightBmp");
    if (dsC2U(value).empty())
    {
        wxLogMessage("%s have no lightBmp property!",element->Attribute("name"));
    }
    else
    {
        SetImageProperty(PROP_LIGHT_BMP, value);
    }

	value = element->Attribute("normalBmp");
    if (dsC2U(value).empty())
    {
        wxLogMessage("%s have no normalBmp property!",element->Attribute("name"));
    }
    else
    {
        SetImageProperty(PROP_NORMAL_BMP, value);
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


void uiMenuItem::OnBuildDeclarationsCode()
{
	AddDeclaration(wxString::Format(wxT("UIMenuItem *%s = create_menu_item(\"%s\");"), GetStrPropValue(PROP_VAR_NAME), GetName()));
}

void uiMenuItem::OnBuildCreatingCode()
{
	uiWindow::OnBuildCreatingCode();
	AddBuildingCode(wxString::Format(wxT("set_menu_item_text(%s, \"%s\");\n"), GetStrPropValue(PROP_VAR_NAME), GetStrPropValue(PROP_TEXT)));
	AddBuildingCode(wxString::Format(wxT("set_menu_item_light_icon(%s, \"%s\");\n"), GetStrPropValue(PROP_VAR_NAME), GetImagePropValue(PROP_LIGHT_BMP)));
	AddBuildingCode(wxString::Format(wxT("set_menu_item_normal_icon(%s, \"%d\");\n"), GetStrPropValue(PROP_VAR_NAME), GetImagePropValue(PROP_NORMAL_BMP)));
	AddBuildingCode(wxString::Format(wxT("set_menu_item_highlight(%s, \"%d\");\n"), GetStrPropValue(PROP_VAR_NAME), GetBoolPropValue(PROP_FOCUS)));

	AddBuildingCode("\n");
}

void uiMenuItem::OnBuildIdCode()
{

}


