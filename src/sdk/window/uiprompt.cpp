/*
 * uiprompt.cpp
 *
 *  Created on: 2019-8-11
 *      Author: Antony
 */

#include "window/uiprompt.h"


uiPrompt::uiPrompt(uiWindow *parent) : uiWindow(parent)
{
	/* override property */
	SetStringProperty(PROP_CLASS,		"UIPrompt");
	SetProperty(PROP_POSITION, wxRect(0, 0, PROMPT_DEFAULT_WIDTH, PROMPT_DEFAULT_HEIGHT));
	m_Properties.find(PROP_BKG_COLOR)->second->m_Visiable = false;
	/* specially for UIPrompt */
	m_Properties.insert(pair<wxString, PropertyBase*>("Prompt Property", new PropertyFold("Prompt Property")));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_TITLE_STRING, new PropertyInternalString("Null value")));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_CONTENT, new PropertyInternalString("Null value")));
	m_Properties.insert(pair<wxString, PropertyBase*>(PROP_DRUATION_TIME, new PropertyInteger(2000)));

	/* Marked as a control */
	m_type = WINDOW_TYPE_SUB;
	m_icon_index = ICON_CONTROL_PROMPT;
}


uiPrompt::~uiPrompt(void)
{

}

wxPGChoices uiPrompt::AddEnmuChildProperty(wxString key)
{
	wxPGChoices choice;

	return choice;
}

UIWindow* uiPrompt::guiWindow(void)
{
	UIWindow *window =  create_prompt(GetStrPropValue(PROP_NAME));

	//window_use_style(prompt, "Default");
	set_prompt_title(window, GetStrPropValue(PROP_TITLE_STRING));
	set_prompt_content(window, GetStrPropValue(PROP_CONTENT));
	set_prompt_druation(window, GetIntPropValue(PROP_DRUATION_TIME));

	wxRect rect = GetRectPropValue(PROP_POSITION);
	set_window_rect(window, ui_rect(rect.x, rect.y, rect.width, rect.height));

	m_absolute_pos.x = rect.x;
	m_absolute_pos.y = rect.y;
	return window;
}


TiXmlElement* uiPrompt::GetXMLElement(void)
{
	TiXmlElement* element = uiWindow::GetXMLElement();

	InsertTextChild(element, "title", 	m_Properties.find(PROP_TITLE_STRING)->second->xmlString());
	InsertTextChild(element, "content", m_Properties.find(PROP_CONTENT)->second->xmlString());
	element->SetAttribute("druation", 		m_Properties.find(PROP_DRUATION_TIME)->second->xmlString());


	return element;
}


void uiPrompt::SetProperties(const TiXmlElement* element)
{
	/* read base property */
	uiWindow::SetProperties(element);

	/* read UIPrompt special property */
	const TiXmlElement *text = element->FirstChildElement("title");
	if (dsC2U(text->GetText()).empty())
    {
        wxLogMessage("%s have no title property!",element->Attribute("name"));
    }
    else
    {
        SetStringProperty(PROP_TITLE_STRING, text->GetText());
    }

	text = element->FirstChildElement("content");
    if (dsC2U(text->GetText()).empty())
    {
        wxLogMessage("%s have no content property!",element->Attribute("name"));
    }
    else
    {
        SetStringProperty(PROP_CONTENT, text->GetText());
    }

	const char *value = element->Attribute("druation");
    if (dsC2U(text->GetText()).empty())
    {
        wxLogMessage("%s have no druation property!",element->Attribute("name"));
    }
    else
    {
        SetIntegerProperty(PROP_DRUATION_TIME, value);
    }


}


void uiPrompt::OnBuildDeclarationsCode()
{

	AddDeclaration(wxString::Format(wxT("UIPrompt *%s = create_prompt(\"%s\");"), GetStrPropValue(PROP_VAR_NAME), GetName()));
}

void uiPrompt::OnBuildCreatingCode()
{
	uiWindow::OnBuildCreatingCode();
	AddBuildingCode(wxString::Format(wxT("set_prompt_title(%s, \"%d\");\n"), GetStrPropValue(PROP_VAR_NAME), GetStrPropValue(PROP_TITLE_STRING)));
	AddBuildingCode(wxString::Format(wxT("set_prompt_content(%s, \"%d\");\n"), GetStrPropValue(PROP_VAR_NAME), GetStrPropValue(PROP_CONTENT)));
	AddBuildingCode(wxString::Format(wxT("set_prompt_druation(%s, \"%d\");\n"), GetStrPropValue(PROP_VAR_NAME), GetIntPropValue(PROP_DRUATION_TIME)));

	AddBuildingCode("\n");
}

void uiPrompt::OnBuildIdCode()
{

}




