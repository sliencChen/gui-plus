/*
 * uilabel.cpp
 *
 *  Created on: 2019-4-18
 *      Author: Brant
 */
#include "sdk_common.h"
#include "window/uilabel.h"


uiLabel::uiLabel(uiWindow *parent) : uiWindow(parent)
{
	/* override property */
	SetStringProperty(PROP_CLASS,		"UILabel");
	SetColorProperty(PROP_BKG_COLOR,	wxColour(0, 0, 0, 0));
	SetProperty(PROP_POSITION, wxRect(0, 0, LABEL_DEFAULT_WIDTH, LABEL_DEFAULT_HEIGHT));

	/* specially for UILabel */
	m_Properties.insert(pair<wxString, PropertyBase*>("Label Property", new PropertyFold("Label Property")));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_TEXT, new PropertyInternalString("Label Text")));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_FORE_COLOR, new PropertyColor(0xff000000)));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_FONT_SIZE, new PropertyInteger(18)));

	/* Marked as a control */
	m_type = WINDOW_TYPE_CONTROL;
	m_icon_index = ICON_CONTROL_LABEL;
}


uiLabel::~uiLabel(void)
{

}

wxPGChoices uiLabel::AddEnmuChildProperty(wxString key)
{
	wxPGChoices choice;

	return choice;
}


UIWindow* uiLabel::guiWindow(void)
{
	UIWindow *window = create_label(GetStrPropValue(PROP_NAME));

	set_window_color(window, GetColorPropValue(PROP_BKG_COLOR));
	set_label_text_color(window, GetColorPropValue(PROP_FORE_COLOR));
	set_label_font_size(window, GetIntPropValue(PROP_FONT_SIZE));
	set_label_text(window, GetStrPropValue(PROP_TEXT));

    if (this->m_Parent->GetWindowType() == WINDOW_TYPE_SIZER)
    {
        this->stb_window = window;
        UISizer *sizer = this->m_Parent->stb_sizer;
        if (sizer)
        {
            sizer_add_window(sizer, window, ui_size(100, 50), 1, 5, UI_ALIGN_CENTER_VER|UI_SIZER_FLAG_BORDER_TOP);
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


TiXmlElement* uiLabel::GetXMLElement(void)
{
	TiXmlElement* element = uiWindow::GetXMLElement();

	/* append UILabel property */
	element->SetAttribute("foreColor", m_Properties.find(PROP_FORE_COLOR)->second->xmlString());
	element->SetAttribute("fontSize", m_Properties.find(PROP_FONT_SIZE)->second->xmlString());
	InsertTextChild(element, "text", m_Properties.find(PROP_TEXT)->second->xmlString());

	return element;
}


void uiLabel::SetProperties(const TiXmlElement* element)
{
	/* read base property */
	uiWindow::SetProperties(element);

	/* read UILabel special property */
	const char *value = element->Attribute("foreColor");
    if (dsC2U(value).empty())
    {
        wxLogMessage("%s have no foreColor property!",element->Attribute("name"));
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

	const TiXmlElement *text = element->FirstChildElement("text");
    if (dsC2U(text->GetText()).empty())
    {
        wxLogMessage("%s have no normalBmp property!",element->Attribute("name"));
    }
    else
    {
        SetStringProperty(PROP_TEXT, text->GetText());
    }
}


void uiLabel::OnBuildDeclarationsCode()
{
	AddDeclaration(wxString::Format(wxT("UILabel *%s = create_label(\"%s\");"), GetStrPropValue(PROP_VAR_NAME), GetName()));
}

void uiLabel::OnBuildCreatingCode()
{
	uiWindow::OnBuildCreatingCode();
	AddBuildingCode(wxString::Format(wxT("set_label_text(%s, \"%s\");\n"), GetStrPropValue(PROP_VAR_NAME), GetStrPropValue(PROP_TEXT)));
	AddBuildingCode(wxString::Format(wxT("set_label_text_color(%s, 0x%08X);\n"), GetStrPropValue(PROP_VAR_NAME), GetColorPropValue(PROP_FORE_COLOR)));
	AddBuildingCode(wxString::Format(wxT("set_label_font_size(%s, %d);\n"), GetStrPropValue(PROP_VAR_NAME), GetIntPropValue(PROP_FONT_SIZE)));
	AddBuildingCode("\n");
}

void uiLabel::OnBuildIdCode()
{

}
